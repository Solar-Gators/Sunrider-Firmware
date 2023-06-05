/*
 * user.cpp
 *
 *  Created on: Jan 29, 2022
 *      Author: John Carr
 */

#include "user.hpp"
#include "PID.hpp"

using namespace SolarGators;

extern "C" void CPP_UserSetup(void);

void SendCanMsgs();
void SendTelemetryData();
void UpdateThrottle();
uint8_t CalcRegen(float* acceleration);

// OS Configs
/* Definitions for CAN Tx Thread */
osTimerId_t can_tx_timer_id;
osTimerAttr_t can_tx_timer_attr =
{
    .name = "CAN Tx"
};
osTimerId_t telem_tx_timer_id;
osTimerAttr_t telem_tx_timer_attr =
{
    .name = "Telemetry"
};
// Global variables for speed control
osTimerId_t speed_control_timer_id;
osTimerAttr_t speed_control_timer_attr =
{
    .name = "Speed"
};
static constexpr uint32_t speed_control_period = 10;
static constexpr float Pgain=1275;  // proportional control gain
static constexpr float Igain=2;     // integral control gain
static constexpr float Dgain=50;    // derivative control gain

SolarGators::Drivers::PID regen_controller(Pgain, Igain, Dgain, speed_control_period);

void CPP_UserSetup(void)
{
	//the below code initializes but doesn't actually add them to scheduler yet
  // Initialize routine that sends telemetry data
  telem_tx_timer_id = osTimerNew((osThreadFunc_t)SendTelemetryData, osTimerPeriodic, NULL, &telem_tx_timer_attr);
  if (telem_tx_timer_id == NULL)
  {
      Error_Handler();
  }
  // Initialize routine that sends CAN Data
  can_tx_timer_id = osTimerNew((osThreadFunc_t)SendCanMsgs, osTimerPeriodic, NULL, &can_tx_timer_attr);
  if (can_tx_timer_id == NULL)
  {
      Error_Handler();
  }
  // Initialize routine that updates regen and throttle
  speed_control_timer_id = osTimerNew((osThreadFunc_t)UpdateThrottle, osTimerPeriodic, NULL, &speed_control_timer_attr);
  if (speed_control_timer_id == NULL)
  {
      Error_Handler();
  }
  // Register initialize driver for IMU
//  if(LSM6DSR_RegisterBusIO(&imu, &imu_bus))
//  {
//    Error_Handler();
//  }
//  if(LSM6DSR_Init(&imu))
//  {
//    Error_Handler();
//  }
//  if(LSM6DSR_ACC_Enable(&imu))
//  {
//    Error_Handler();
//  }
  // Front Lights (for throttle)
  //we add these modules to the etl map, binds can id and actual module together
  CANController.AddRxModule(&FLights);
  CANController.AddRxModule(&RLights);
  // Mitsuba Stuff
  CANController.AddRxModule(&Motor_Rx_0);
  CANController.AddRxModule(&Motor_Rx_1);
  CANController.AddRxModule(&Motor_Rx_2);
  // Orion Stuff
  CANController.AddRxModule(&BMS_Rx_0);
  CANController.AddRxModule(&BMS_Rx_1);
  CANController.AddRxModule(&BMS_Rx_2);
  CANController.AddRxModule(&BMS_Rx_3);
  CANController.AddRxModule(&BMS_Rx_4);
  CANController.AddRxModule(&BMS_Rx_5);

  // MPPTs
  CANController.AddRxModule(&MPPT0_Rx_0);
  CANController.AddRxModule(&MPPT0_Rx_1);
  CANController.AddRxModule(&MPPT0_Rx_5);
  CANController.AddRxModule(&MPPT1_Rx_0);
  CANController.AddRxModule(&MPPT1_Rx_1);
  CANController.AddRxModule(&MPPT1_Rx_5);
  CANController.AddRxModule(&MPPT2_Rx_0);
  CANController.AddRxModule(&MPPT2_Rx_1);
  CANController.AddRxModule(&MPPT2_Rx_5);

  // Ready CAN
  CANController.Init();

  // Ready GPS
  GPS_init(huart4.Instance);
  GPS_startReception();
  // Start Timers
  osTimerStart(telem_tx_timer_id, 1000);  // Pit Transmission
  osTimerStart(can_tx_timer_id, 2000);    // CAN Tx Transmission
  // Initialize DACs
  accel.SetRefVcc();
  regen.SetRefVcc();
  // Initialize PID Controller
  regen_controller.SetOutLimits(0, 255);
  regen_controller.SetIntegLimits(0, 255);
  regen_controller.SetFilter(0);
  // Start the thread that will update the motor controller
  osTimerStart(speed_control_timer_id, speed_control_period);    // Mitsuba throttle and regen
}

void SendCanMsgs()
{
  // Request Mitsuba Data
  Motor_Tx.SetRequestAllFrames();
  CANController.Send(&Motor_Tx);
}

void SendTelemetryData()
{
  pit.SendDataModule(Gps);
  pit.SendDataModule(Motor_Rx_0);
  pit.SendDataModule(Motor_Rx_1);
  pit.SendDataModule(Motor_Rx_2);

  pit.SendDataModule(BMS_Rx_0);
  pit.SendDataModule(BMS_Rx_1);
  pit.SendDataModule(BMS_Rx_2);
  pit.SendDataModule(BMS_Rx_3);
  pit.SendDataModule(BMS_Rx_4);
  pit.SendDataModule(BMS_Rx_5);

  pit.SendDataModule(MPPT0_Rx_0);
  pit.SendDataModule(MPPT0_Rx_1);
  pit.SendDataModule(MPPT0_Rx_5);
  pit.SendDataModule(MPPT1_Rx_0);
  pit.SendDataModule(MPPT1_Rx_1);
  pit.SendDataModule(MPPT1_Rx_5);
  pit.SendDataModule(MPPT2_Rx_0);
  pit.SendDataModule(MPPT2_Rx_1);
  pit.SendDataModule(MPPT2_Rx_5);

  pit.SendDataModule(FLights);
  pit.SendDataModule(RLights);
}

void UpdateThrottle()
{
  uint8_t adjThrottleVal = static_cast<uint8_t>(FLights.GetThrottleVal() >> 5);
  // Probs dont want to do the below would be better to drop two bits then map 12 bits to 18 bits
  if (adjThrottleVal > 200) {
	  adjThrottleVal = 200;
  }
  accel.WriteAndUpdate(adjThrottleVal); // shift over b\c we are sending 14 bit ADC to 8 bit DAC
  // If the throttle is 0 then we should regen so that we are hitting a 0.2g *deceleration*
  // Read IMU to get accel info for PID
//  LSM6DSR_Axes_t accel_info;
//  LSM6DSR_ACC_GetAxes(&imu, &accel_info);
  // Calculate regen value
  // Write regen value to motor controller
//  regen_controller.Update(0.2, accel_info.x);
//  if(adjThrottleVal == 0)
//  {
//    regen.WriteAndUpdate(regenVal);
//  }
  if(Steering.GetEcoEnabledStatus())
  {
    eco.TurnOn();
  }
  else
  {
    eco.TurnOff();
  }
  if(Steering.GetReverseStatus())
  {
    // TODO
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CANController.SetRxFlag();
  HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}


void UART4_RX_Handler()
{
  bool finishedProcessing;
  char* data = GPS_RxCpltCallback(&finishedProcessing);
  if (finishedProcessing) {
    Gps.FromByteArray((uint8_t*)data);
  }
}
