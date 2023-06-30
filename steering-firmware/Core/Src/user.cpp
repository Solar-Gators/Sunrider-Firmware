/*
 * user.cpp
 *
 *  Created on: Apr 5, 2023
 *      Author: Taylor Gerke
 */

#include "user.hpp"
#include "etl/to_string.h"
#include "etl/format_spec.h"
#include "etl/string_utilities.h"
#include "UI.hpp"
#include "fatfs.h"

#define LENGTH 100
uint16_t sd_counter = 0;

using namespace SolarGators;

extern "C" void CPP_UserSetup(void);
extern "C" void CPP_HandleGPIOInterrupt(uint16_t GPIO_Pin);

void UpdateSignals();
void ReadButtonPress();
void UpdateUI();
void SendCanMsgs();
void HandleEco();
void HandleHeadLights();
void HandleCruise();
void HandleReverse();
void UpdateAudio();

void process_SD_card2(void);

osSemaphoreId_t lcdSem = osSemaphoreNew(1, 1, NULL);
osSemaphoreId_t spkrSem = osSemaphoreNew(1, 1, NULL);
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

bool lt_indicator = false;
bool rt_indicator = false;
bool eco_indicator = true;
bool rev_indicator = false;

FATFS fs;
FIL fil;
FRESULT fresult;
FSIZE_t fil_addr;

uint32_t times[LENGTH];
uint16_t motorRPMs[LENGTH];
uint16_t motorTemps[LENGTH];
float voltages[LENGTH];
float currents[LENGTH];
float SOCs[LENGTH];

bool sdInit = false;
bool isOn = false;

// OS Configs
osTimerId_t signal_timer_id;
osTimerAttr_t signal_timer_attr =
{
    .name = "lights_led"
};
/* Definitions for UI Updater */
osThreadId_t ui_thread_id;
uint32_t ui_stack[ 256 ];
StaticTask_t ui_control_block;
const osThreadAttr_t ui_thread_attributes = {
  .name = "UI",
  .cb_mem = &ui_control_block,
  .cb_size = sizeof(ui_control_block),
  .stack_mem = &ui_stack[0],
  .stack_size = sizeof(ui_stack),
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for SD Logger */
osThreadId_t sd_thread_id;
uint32_t sd_stack[ 256 ];
StaticTask_t sd_control_block;
const osThreadAttr_t sd_thread_attributes = {
  .name = "UI",
  .cb_mem = &sd_control_block,
  .cb_size = sizeof(sd_control_block),
  .stack_mem = &sd_stack[0],
  .stack_size = sizeof(sd_stack),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for CAN Tx Thread */
osTimerId_t can_tx_timer_id;
osTimerAttr_t can_tx_timer_attr =
{
    .name = "CAN Tx"
};

osTimerId_t btn_press_timer_id;
osTimerAttr_t btn_press_timer_attr =
{
    .name = "Button Press"
};

osTimerId_t sd_card_timer_id;
osTimerAttr_t sd_card_timer_attr =
{
    .name = "SD Card"
};

uint32_t PULSE = 500;

bool hazardsPressed = false;
bool cruiseEnPressed = false;

// Wheel Diameter in miles
static constexpr float WHEEL_DIAM_IN = 23.071;
static constexpr float WHEEL_DIAM_FT = WHEEL_DIAM_IN / 12;
static constexpr float WHEEL_DIAM_MI = (WHEEL_DIAM_IN / 63360) * 3.14;

SolarGators::DataModules::DataModule* BMS_Rx_0_ptr;
SolarGators::DataModules::DataModule* BMS_Rx_2_ptr;
SolarGators::DataModules::DataModule* BMS_Rx_4_ptr;
SolarGators::DataModules::DataModule* Motor_Rx_0_ptr;
SolarGators::DataModules::DataModule* Motor_Rx_2_ptr;
SolarGators::DataModules::DataModule* FLights_ptr;
SolarGators::DataModules::DataModule* PowerBoard_ptr;

void CPP_UserSetup(void)
{
  // Setup Actions
  // Note: These binds really abuse the stack and we should figure out a way to avoid them
  //       since we are heavily constrained.
  // Note: No longer using binds, using etl::delegate should solve our stack problem but still
  //       needs to be tested. This is compile time determined so hoping for better performance also.
  {
    using namespace SolarGators::DataModules;
    // Left Side
    left_turn.action_ = etl::delegate<void(void)>::create<SteeringController, LightsState, &SteeringController::ToggleLeftTurnSignal>();
    cruise_minus.action_ = etl::delegate<void(void)>::create<SteeringController, LightsState, &SteeringController::DecreaseCruiseSpeed>();
    eco.action_ = etl::delegate<void(void)>::create<HandleEco>();
    headlights.action_ = etl::delegate<void(void)>::create<HandleHeadLights>();
    //hazards.action_ = etl::delegate<void(void)>::create<SteeringController, LightsState, &SteeringController::ToggleHazards>();
    // Right Side
    right_turn.action_ = etl::delegate<void(void)>::create<SteeringController, LightsState, &SteeringController::ToggleRightTurnSignal>();
    cruise_plus.action_ = etl::delegate<void(void)>::create<SteeringController, LightsState, &SteeringController::IncreaseCruiseSpeed>();
    //horn.action_ = etl::delegate<void(void)>::create<SteeringController, LightsState, &SteeringController::ToggleHorn>();
    //cruise.action_ = etl::delegate<void(void)>::create<HandleCruise>();
    reverse.action_ = etl::delegate<void(void)>::create<HandleReverse>();
  }
  // Add to Button Group
  // Left side
  LightsState.AddButton(&left_turn);
  LightsState.AddButton(&cruise_minus);
  LightsState.AddButton(&eco);
  LightsState.AddButton(&headlights);
  LightsState.AddButton(&hazards);
  // Right side
  LightsState.AddButton(&right_turn);
  LightsState.AddButton(&cruise_plus);
  //LightsState.AddButton(&horn);
  LightsState.AddButton(&cruise);
  LightsState.AddButton(&reverse);
//   Load the CAN Controller
  CANController.AddRxModule(&BMS_Rx_0);
  BMS_Rx_0_ptr = &BMS_Rx_0;
  //CANController.AddRxModule(&BMS_Rx_2);
  BMS_Rx_2_ptr = &BMS_Rx_2;
  CANController.AddRxModule(&BMS_Rx_4);
  BMS_Rx_4_ptr = &BMS_Rx_4;
  CANController.AddRxModule(&Motor_Rx_0);
  Motor_Rx_0_ptr = &Motor_Rx_0;
  CANController.AddRxModule(&Motor_Rx_2);
  Motor_Rx_2_ptr = &Motor_Rx_2;
  CANController.AddRxModule(&PowerBoard);
  PowerBoard_ptr = &PowerBoard;
//   TEST WITH FRONT LIGHTS
  // CANController.AddRxModule(&FLights);
  FLights_ptr = &FLights;
  CANController.Init();

  // Start Thread that Handles Turn Signal LEDs
  signal_timer_id = osTimerNew((osThreadFunc_t)UpdateSignals, osTimerPeriodic, NULL, &signal_timer_attr);
  if (signal_timer_id == NULL)
  {
      Error_Handler();
  }
  osTimerStart(signal_timer_id, 500);
  // Start Thread that updates screen
  ui_thread_id = osThreadNew((osThreadFunc_t)UpdateUI, NULL, &ui_thread_attributes);
  if (ui_thread_id == NULL)
  {
      Error_Handler();
  }
  // Start Thread for SD logger
    sd_thread_id = osThreadNew((osThreadFunc_t)process_SD_card2, NULL, &sd_thread_attributes);
    if (sd_thread_id == NULL)
    {
        Error_Handler();
    }
//  // Start Thread that sends CAN Data
//  can_tx_timer_id = osTimerNew((osThreadFunc_t)SendCanMsgs, osTimerPeriodic, NULL, &can_tx_timer_attr);
//  if (can_tx_timer_id == NULL)
//  {
//      Error_Handler();
//  }
//  osTimerStart(can_tx_timer_id, 20);

  // Start Thread that checks for button presses
	btn_press_timer_id = osTimerNew((osThreadFunc_t)ReadButtonPress, osTimerPeriodic, NULL, &btn_press_timer_attr);
	if (btn_press_timer_id == NULL)
	{
		Error_Handler();
	}

  osTimerStart(btn_press_timer_id, 12);

}

void ReadButtonPress() {

	// Check if horn is pressed
	GPIO_PinState horn = HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin);
	if(horn == GPIO_PIN_RESET) {
		LightsState.EnableHorn();
	} else {
		LightsState.DisableHorn();
	}
	if (!HAL_GPIO_ReadPin(BTN8_GPIO_Port, BTN8_Pin)) {
		if (hazardsPressed) {
			LightsState.ToggleHazards();
			hazardsPressed = false;
		} else {
			hazardsPressed = true;
		}
	}

//	if (HAL_GPIO_ReadPin(BTN7_GPIO_Port, BTN7_Pin)) {
//		if (cruiseEnPressed) {
//			LightsState.ToggleCruise();
//			cruiseEnPressed = false;
//		} else {
//			cruiseEnPressed = true;
//		}
//	}

}




void UpdateSignals()
{
  osMutexAcquire(LightsState.mutex_id_, osWaitForever);

  if (LightsState.GetHazardsStatus()) {
	  lt_indicator = !lt_indicator;
      rt_indicator = !rt_indicator;
  } else if (LightsState.GetRightTurnStatus()) {
	  rt_indicator = !rt_indicator;
  } else if (LightsState.GetLeftTurnStatus()) {
	  lt_indicator = !lt_indicator;
  } else {
	  lt_indicator = false;
	  rt_indicator = false;
  }

  osMutexRelease(LightsState.mutex_id_);
	if (LightsState.GetLeftTurnStatus() || LightsState.GetRightTurnStatus() || LightsState.GetHazardsStatus()) {

		osMutexAcquire(spkrSem, osWaitForever);

		if (isOn) {
			__HAL_TIM_SET_PRESCALER(&htim2, 20);
			isOn = false;
		} else {
			__HAL_TIM_SET_PRESCALER(&htim2, 10);
			isOn = true;
		}

		HAL_TIM_Base_Start(&htim2);
		HAL_TIM_Base_Start_IT(&htim3);
		osMutexRelease(spkrSem);


	} else {
		osMutexAcquire(spkrSem, osWaitForever);
		HAL_TIM_Base_Stop(&htim2);
		__HAL_TIM_SET_PRESCALER(&htim2, 10);
		isOn = false;
		osMutexRelease(spkrSem);
	}

}

SolarGators::Drivers::ILI9341 Display(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT);

void UpdateUI()
{
  // HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  // LCD
  osSemaphoreAcquire(lcdSem, osWaitForever);
  Display.Init();

  // This will initialize the UI
  SolarGators::Drivers::UI ui(Display);
  osSemaphoreRelease(lcdSem);

  // Set Format
  static constexpr etl::format_spec format(10,5,1,false,false,false,false,'0');
  while(1)
  {
    etl::string<5> buff;
    // Update Accel
    etl::to_string(PowerBoard.GetSupBatVoltage(), buff, format, false);
	osSemaphoreAcquire(lcdSem, osWaitForever);
	ui.UpdateSquare(0, buff);
	osSemaphoreRelease(lcdSem);

    // Update SOC
//    etl::to_string(BMS_Rx_4.getPackSoc(), buff, format, false);
//    osSemaphoreAcquire(lcdSem, osWaitForever);
//    ui.UpdateSquare(1, buff);
//    osSemaphoreRelease(lcdSem);

	etl::to_string(PowerBoard.GetSupBatPower(), buff, format, false);
	osSemaphoreAcquire(lcdSem, osWaitForever);
	ui.UpdateSquare(1, buff);
	osSemaphoreRelease(lcdSem);

    // Update Voltage
    etl::to_string(BMS_Rx_0.getPackSumVolt(), buff, format, false);
    osSemaphoreAcquire(lcdSem, osWaitForever);
    ui.UpdateSquare(2, buff);
    osSemaphoreRelease(lcdSem);
    // Update Current
//    etl::to_string(BMS_Rx_2.getPackCurrent(), buff, format, false);
//    osSemaphoreAcquire(lcdSem, osWaitForever);
//    ui.UpdateSquare(3, buff);
//    osSemaphoreRelease(lcdSem);

    etl::to_string(PowerBoard.GetMainBatPower(), buff, format, false);
	osSemaphoreAcquire(lcdSem, osWaitForever);
	ui.UpdateSquare(3, buff);
	osSemaphoreRelease(lcdSem);

    // Update Speed
    float speed = Motor_Rx_0.GetMotorRPM() * WHEEL_DIAM_MI * 60;
    osSemaphoreAcquire(lcdSem, osWaitForever);
    ui.UpdateSpeed(speed);
    osSemaphoreRelease(lcdSem);
    //draw trip codes
    osSemaphoreAcquire(lcdSem, osWaitForever);
    ui.UpdateMitsubaTrip(&Motor_Rx_2);
    ui.UpdateBMSTrip(&BMS_Rx_4);
    osSemaphoreRelease(lcdSem);
    osDelay(40); // Aim for 20hz
  }
}

void SendCanMsgs()
{
  // Send the lights state
  CANController.Send(&LightsState);
  // Request motor controller data
  McReq.SetRequestAllFrames();
  CANController.Send(&McReq);
}

void CPP_HandleGPIOInterrupt(uint16_t GPIO_Pin)
{
  LightsState.HandlePress(GPIO_Pin);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CANController.SetRxFlag();
  HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void HandleEco()
{
  LightsState.ToggleEco();
}
void HandleHeadLights()
{
  LightsState.ToggleHeadlights();
}
void HandleCruise()
{
  LightsState.ToggleCruise();
}
void HandleReverse()
{
  LightsState.ToggleReverse();
}

void process_SD_card2(void) {

	char nameArray[10] = {'l', 'o', 'g', '0', '0', '.', 't', 'x', 't'};

//	char buffer[5];
//	char num0 = '0';
//	char num1 = '0';
//
//	if (!sdInit && HAL_GPIO_ReadPin(SD_Detect_GPIO_Port, SD_Detect_Pin)) {
//
//		osSemaphoreAcquire(lcdSem, osWaitForever);
//		Display.Reset();
//
//		fresult = f_mount(&fs, "", 0);
//		fresult = f_open(&fil, "lognum.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
//		f_gets(buffer, sizeof(buffer), &fil);
//		fresult = f_close(&fil);
//
//		if (buffer[1]) {
//			nameArray[4] = buffer[1];
//		}
//
//		if (buffer[0]) {
//			nameArray[3] = buffer[0];
//		}
//
//		//osDelay(5000);
//		uint32_t count = 0;
//		while (count < 100000) {
//			count++;
//		}
//
//		if (buffer[0] && buffer[1]) {
//
//			switch (buffer[1]) {
//			case '0':
//				num1 = '1';
//				break;
//			case '1':
//				num1 = '2';
//				break;
//			case '2':
//				num1 = '3';
//				break;
//			case '3':
//				num1 = '4';
//				break;
//			case '4':
//				num1 = '5';
//				break;
//			case '5':
//				num1 = '6';
//				break;
//			case '6':
//				num1 = '7';
//				break;
//			case '7':
//				num1 = '8';
//				break;
//			case '8':
//				num1 = '9';
//				break;
//			case '9':
//				num1 = '0';
//				break;
//			default:
//				break;
//			}
//
//			if (num1 == '0') {
//				switch (buffer[0]) {
//				case '0':
//					num0 = '1';
//					break;
//				case '1':
//					num0 = '2';
//					break;
//				case '2':
//					num0 = '3';
//					break;
//				case '3':
//					num0 = '4';
//					break;
//				case '4':
//					num0 = '5';
//					break;
//				case '5':
//					num0 = '6';
//					break;
//				case '6':
//					num0 = '7';
//					break;
//				case '7':
//					num0 = '8';
//					break;
//				case '8':
//					num0 = '9';
//					break;
//				case '9':
//					num0 = '0';
//					break;
//				default:
//					break;
//				}
//			}
//
//			fresult = f_open(&fil, "lognum.txt", FA_OPEN_ALWAYS | FA_WRITE);
//			fresult = (FRESULT)f_putc(num0, &fil);
//			fresult = (FRESULT)f_putc(num1, &fil);
//			fresult = f_close(&fil);
//			fresult = f_mount(NULL, "", 0);
//		} else {
//			fresult = f_open(&fil, "lognum.txt", FA_OPEN_ALWAYS | FA_WRITE);
//			fresult = (FRESULT)f_putc('0', &fil);
//			fresult = (FRESULT)f_putc('1', &fil);
//			fresult = f_close(&fil);
//			fresult = f_mount(NULL, "", 0);
//		}
		sdInit = true;
//		Display.Resume();
//		osSemaphoreRelease(lcdSem);
//	}

	while(1) {


	//char *ptr = &nameArray[0];

	if (sd_counter < LENGTH) {

		times[sd_counter] = xTaskGetTickCount();
		motorRPMs[sd_counter] = Motor_Rx_0.GetMotorRPM();
		motorTemps[sd_counter] = Motor_Rx_0.GetFetTemp();
		currents[sd_counter] = BMS_Rx_2.getPackCurrent();
		voltages[sd_counter] = BMS_Rx_0.getPackSumVolt();
		SOCs[sd_counter] = PowerBoard.GetSupBatVoltage();
		sd_counter++;

	} else {

		sd_counter = 0;

		if (HAL_GPIO_ReadPin(SD_Detect_GPIO_Port, SD_Detect_Pin) && sdInit) {

			osSemaphoreAcquire(lcdSem, osWaitForever);
			Display.Reset();

			fresult = f_mount(&fs, "", 0);

			if (fresult == FR_OK) {

				fresult = f_open(&fil, nameArray, FA_OPEN_ALWAYS | FA_WRITE);

				fresult = f_lseek(&fil, fil.fptr);

				if (fil_addr) {
					fresult = f_lseek(&fil, fil_addr);
				} else {
					fresult = (FRESULT)f_puts("Time (ms),Supp Voltage,Battery Voltage,Battery Current,Motor RPM,Motor Temp\n", &fil);
				}

				static constexpr etl::format_spec format(10,5,1,false,false,false,false,'0');
				etl::string<5> buff;

				for (int i = 0; i < LENGTH; i++) {

					etl::to_string(times[i], buff, format, false);
					fresult = (FRESULT)f_puts(buff.c_str(), &fil);
					fresult = (FRESULT)f_putc(',', &fil);
					etl::to_string(SOCs[i], buff, format, false);
					fresult = (FRESULT)f_puts(buff.c_str(), &fil);
					fresult = (FRESULT)f_putc(',', &fil);
					etl::to_string(voltages[i], buff, format, false);
					fresult = (FRESULT)f_puts(buff.c_str(), &fil);
					fresult = (FRESULT)f_putc(',', &fil);
					etl::to_string(currents[i], buff, format, false);
					fresult = (FRESULT)f_puts(buff.c_str(), &fil);
					fresult = (FRESULT)f_putc(',', &fil);
					etl::to_string(motorRPMs[i], buff, format, false);
					fresult = (FRESULT)f_puts(buff.c_str(), &fil);
					fresult = (FRESULT)f_putc(',', &fil);
					etl::to_string(motorTemps[i], buff, format, false);
					fresult = (FRESULT)f_puts(buff.c_str(), &fil);
					fresult = (FRESULT)f_puts("\n", &fil);

				}

//				etl::to_string(BMS_Rx_4.getPackSoc(), buff, format, false);
//
//				float myFloat = 15.23;
//				char soc = (char)myFloat;
//
//				fresult = (FRESULT)f_puts(buff.c_str(), &fil);
//				fresult = (FRESULT)f_puts("\n", &fil);
//				myFloat /= 100;
//				soc = (char)myFloat;
//				fresult = (FRESULT)f_putc(soc, &fil);
//				fresult = (FRESULT)f_puts("\n", &fil);
//
//				fresult = (FRESULT)f_putc(nameArray[2], &fil);
//				fresult = (FRESULT)f_puts("\n", &fil);
				//fresult = (FRESULT)f_puts("hello\n\n", &fil);

				fil_addr = fil.fptr;

				fresult = f_close(&fil);

				fresult = f_mount(NULL, "", 0);

				Display.Resume();
				osSemaphoreRelease(lcdSem);


		}

		times[sd_counter] = xTaskGetTickCount();
		motorRPMs[sd_counter] = Motor_Rx_0.GetMotorRPM();
		motorTemps[sd_counter] = Motor_Rx_0.GetFetTemp();
		currents[sd_counter] = BMS_Rx_2.getPackCurrent();
		voltages[sd_counter] = BMS_Rx_0.getPackSumVolt();
		SOCs[sd_counter] = PowerBoard.GetSupBatVoltage();;
		sd_counter++;

	}

	osDelay(20);

	}

	}
}
