/*
 * user.cpp
 *
 *  Created on: Jan 17, 2022
 *      Author: John Carr, Yash B
 */

#include "user.hpp"

#include "etl/string.h"
#include "etl/to_string.h"
#include "etl/format_spec.h"

using namespace SolarGators;

extern "C" void CPP_UserSetup(void);
extern "C" void strobeCheck(void);
extern "C" IWDG_HandleTypeDef hiwdg;
void UpdateSignals(void);
void SendCanMsgs();
void ReadADC();

// OS Configs
osTimerId_t signal_timer_id;
osTimerAttr_t signal_timer_attr =
{
    .name = "Lights Timer"
};

/* Definitions for the ADC Thread */
osTimerId_t adc_timer_id;
osTimerAttr_t adc_timer_attr =
{
    .name = "ADCs"
};
/* Definitions for CAN Tx Thread */
osTimerId_t can_tx_timer_id;
osTimerAttr_t can_tx_timer_attr =
{
    .name = "CAN Tx"
};
void CPP_UserSetup(void)
{
  // Setup the CAN controller to receive lights state from the steering wheel
  CANController.AddRxModule(&LightsState);
  CANController.AddRxModule(&bmsCodes);
  CANController.AddRxModule(&FLights);
  CANController.AddRxModule(&bmsCurrent);
  for(uint8_t i = 0; i < BUFF_SIZE; i++){
	  FLights.breaksBuffer[i] = 0;
  }
  // Start Thread that Handles Turn Signal LEDs
  signal_timer_id = osTimerNew((osThreadFunc_t)UpdateSignals, osTimerPeriodic, NULL, &signal_timer_attr);
  if (signal_timer_id == NULL)
  {
      Error_Handler();
  }
  osTimerStart(signal_timer_id, 500);

  adc_timer_id = osTimerNew((osThreadFunc_t)ReadADC, osTimerPeriodic, NULL, &adc_timer_attr);
  if (adc_timer_id == NULL)
  {
      Error_Handler();
  }

  osTimerStart(adc_timer_id, 500);
  // Start Thread that sends CAN Data
  can_tx_timer_id = osTimerNew((osThreadFunc_t)SendCanMsgs, osTimerPeriodic, NULL, &can_tx_timer_attr);
  if (can_tx_timer_id == NULL)
  {
      Error_Handler();
  }
  CANController.Init();
  osTimerStart(can_tx_timer_id, 1000); // 10Hz we'll probably want to make this ~100Hz?

}

void SendCanMsgs()
{
  CANController.Send(&RLights);
}

void UpdateSignals(void)
{
  osMutexAcquire(LightsState.mutex_id_, osWaitForever);

  HAL_IWDG_Refresh(&hiwdg);

  bool breakVal = FLights.GetBreaksVal();

  // State, 0: off, 1: toggle, 2: on
  uint32_t leftState = 0;
  uint32_t rightState = 0;

  if (breakVal) {
	  leftState = 2;
	  rightState = 2;
	  tlr_indicator.TurnOn();
  }
  else
	  tlr_indicator.TurnOff();

  if (LightsState.GetLeftTurnStatus() || LightsState.GetHazardsStatus())
  	  leftState = 1;
  if (LightsState.GetRightTurnStatus() || LightsState.GetHazardsStatus())
	  rightState = 1;

  switch (leftState) {
  case 1:
	  lt_indicator.Toggle();
	  break;
  case 2:
	  lt_indicator.TurnOn();
	  break;
  default:
	  lt_indicator.TurnOff();
	  break;
  }

  switch (rightState) {
    case 1:
  	  rt_indicator.Toggle();
  	  break;
    case 2:
  	  rt_indicator.TurnOn();
  	  break;
    default:
  	  rt_indicator.TurnOff();
  	  break;
    }

  osMutexRelease(LightsState.mutex_id_);

}

void ReadADC()
{
		//Check position of the kill switch, add it to data module, cut contactor if necessary
		//Check for BMS condition, cut off contactor if necessary
		if(HAL_GPIO_ReadPin(KILL_SW_GPIO_Port, KILL_SW_Pin)){
			RLights.setKillSwStatus(false);
		} else{
			RLights.setKillSwStatus(true);
			RLights.setContactorStatus(false);
			contactor_relay.TurnOff();
		}
		//This contains the regulation critical full car trip if pack is charging and charge temp limit is exceeded
		if((bmsCurrent.getPackCurrent() > 0) && bmsCodes.isChargeenableRelayFault()){
			contactor_relay.TurnOff();
			RLights.setContactorStatus(false);
		}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CANController.SetRxFlag();
  HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

//i really wanna call this glizzy check but that would be confusing soooo - yash
//what can ya do i guess
void strobeCheck(){
	//check to activate strobe if discharge enable relay has been faulted, or kill sw
	if (bmsCodes.isInternalCellCommunicationFault() ||
			bmsCodes.isCellBalancingStuckOffFault() ||
			//bmsCodes.isWeakCellFault() ||
			bmsCodes.isLowCellVoltageFault() ||
			bmsCodes.isCellOpenWiringFault() ||
			bmsCodes.isCurrentSensorFault() ||
			bmsCodes.isCellVoltageOver5vFault() ||
			bmsCodes.isCellBankFault() ||
			bmsCodes.isWeakPackFault() ||
			bmsCodes.isFanMonitorFault() ||
			//bmsCodes.isThermistorFault() ||
			bmsCodes.isCanCommunicationFault() ||
			bmsCodes.isRedundantPowerSupplyFault() ||
			bmsCodes.isHighVoltageIsolationFault() ||
			bmsCodes.isInvalidInputSupplyVoltageFault() ||
			bmsCodes.isChargeenableRelayFault() ||
			bmsCodes.isDischargeenableRelayFault() ||
			bmsCodes.isChargerSafetyRelayFault() ||
			bmsCodes.isInternalHardwareFault() ||
			bmsCodes.isInternalHeatsinkThermistorFault() ||
			bmsCodes.isInternalLogicFault() ||
			bmsCodes.isHighestCellVoltageTooHighFault() ||
			bmsCodes.isLowestCellVoltageTooLowFault() ||
			bmsCodes.isPackTooHotFault() ||
			(!RLights.getContactorStatus())) {
		//strobeLight.strobe(3);
		strobeLight.Toggle();
		RLights.setContactorStatus(false);
		contactor_relay.TurnOff();
	}
	else{
		strobeLight.TurnOff();
	}
}
