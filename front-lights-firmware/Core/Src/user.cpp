/*
 * user.cpp
 *
 *  Created on: Jan 17, 2022
 *      Author: John Carr
 */

#include "user.hpp"

#include "etl/string.h"
#include "etl/to_string.h"
#include "etl/format_spec.h"

using namespace SolarGators;
extern "C" IWDG_HandleTypeDef hiwdg;
extern "C" void CPP_UserSetup(void);

void UpdateSignals(void);
void SendCanMsgs();
void ReadIMU();
void ReadADC();
bool FaultPresent();

// OS Configs
osTimerId_t signal_timer_id;
osTimerAttr_t signal_timer_attr =
{
    .name = "Lights Timer"
};
/* Definitions for IMU Thread */
osTimerId_t imu_timer_id;
osTimerAttr_t imu_timer_attr =
{
    .name = "IMU"
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
  // Start Thread that Handles Turn Signal LEDs
  signal_timer_id = osTimerNew((osThreadFunc_t)UpdateSignals, osTimerPeriodic, NULL, &signal_timer_attr);
  if (signal_timer_id == NULL)
  {
      Error_Handler();
  }
  osTimerStart(signal_timer_id, 500);
  // Start Thread that Handles Reads the IMU
//  imu_timer_id = osTimerNew((osThreadFunc_t)ReadIMU, osTimerPeriodic, NULL, &imu_timer_attr);
//  if (imu_timer_id == NULL)
//  {
//      Error_Handler();
//  }
//
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
//  osTimerStart(imu_timer_id, 500);
  // Start Thread that Handles Reads the IMU
  adc_timer_id = osTimerNew((osThreadFunc_t)ReadADC, osTimerPeriodic, NULL, &adc_timer_attr);
  if (adc_timer_id == NULL)
  {
      Error_Handler();
  }
  // Initialize the ADC for throttle and breaks
  throttle.Init();
  breaks.Init();
  osTimerStart(adc_timer_id, 8);    // Needs to be >1x the rate we are sending
  // Start Thread that sends CAN Data
  can_tx_timer_id = osTimerNew((osThreadFunc_t)SendCanMsgs, osTimerPeriodic, NULL, &can_tx_timer_attr);
  if (can_tx_timer_id == NULL)
  {
      Error_Handler();
  }
  CANController.Init();
  osTimerStart(can_tx_timer_id, 50); // 10Hz we'll probably want to make this ~100Hz?
  throttle.Init();
}

void SendCanMsgs()
{
  CANController.Send(&FLights);
  HAL_IWDG_Refresh(&hiwdg);
}

void UpdateSignals(void)
{
  osMutexAcquire(LightsState.mutex_id_, osWaitForever);
  if(LightsState.GetHazardsStatus())
  {
    if (lt_indicator.IsOn())
    {
      lt_indicator.TurnOff();
	  hll_indicator.TurnOff();
	  hlr_indicator.TurnOff();
      rt_indicator.TurnOff();
    }
    else
    {
      lt_indicator.TurnOn();
      rt_indicator.TurnOn();
      hll_indicator.TurnOn();
      hlr_indicator.TurnOn();
    }
  }
  else if(LightsState.GetRightTurnStatus()) {

	  if (LightsState.GetHeadlightsStatus()) {
		  lt_indicator.TurnOn();
	  	  hll_indicator.TurnOn();
	  } else {
		  lt_indicator.TurnOff();
		  hll_indicator.TurnOff();
	  }

	  if (rt_indicator.IsOn())
	  {
		rt_indicator.TurnOff();
		hlr_indicator.TurnOff();
	  }
	  else
	  {
		rt_indicator.TurnOn();
		hlr_indicator.TurnOn();
	  }
  }
  else if(LightsState.GetLeftTurnStatus()) {

	  if (LightsState.GetHeadlightsStatus()) {
		  rt_indicator.TurnOn();
		  hlr_indicator.TurnOn();
	  } else {
		  rt_indicator.TurnOff();
		  hlr_indicator.TurnOff();
	  }

	  if (lt_indicator.IsOn())
	  {
		lt_indicator.TurnOff();
	    hll_indicator.TurnOff();
	  }
	  else
	  {
		lt_indicator.TurnOn();
		hll_indicator.TurnOn();
	  }

  }

  if(!LightsState.GetHazardsStatus() && !LightsState.GetRightTurnStatus()) {
	  if (LightsState.GetHeadlightsStatus()) {
		  rt_indicator.TurnOn();
		  hlr_indicator.TurnOn();
	  } else {
		  rt_indicator.TurnOff();
		  hlr_indicator.TurnOff();
	  }
  }


  if(!LightsState.GetHazardsStatus() && !LightsState.GetLeftTurnStatus()) {

	  if (LightsState.GetHeadlightsStatus()) {
		  lt_indicator.TurnOn();
		  hll_indicator.TurnOn();
	  } else {
		  lt_indicator.TurnOff();
		  hll_indicator.TurnOff();
	  }

  }

  if (LightsState.GetHeadlightsStatus())
  {
    //hlr_indicator.TurnOn();
    headlightR.TurnOn();
    headlightL.TurnOn();
    if (!LightsState.GetLeftTurnStatus() && !LightsState.GetHazardsStatus()) {
		 hll_indicator.TurnOn();
	 } else if (!LightsState.GetRightTurnStatus() && !LightsState.GetHazardsStatus()) {
		 hlr_indicator.TurnOn();
	 }
  } else {
     if (!LightsState.GetLeftTurnStatus() && !LightsState.GetHazardsStatus() && !LightsState.GetRightTurnStatus()) {
    	 hll_indicator.TurnOff();
    	 hlr_indicator.TurnOff();
     }
     headlightR.TurnOff();
     headlightL.TurnOff();
  }
//  if(FaultPresent())
//  {
//    fault_indicator.TurnOn();
//  }
//  else
//  {
//    fault_indicator.TurnOff();
//  }

  if(LightsState.GetHornStatus())
  {
    horn.TurnOn();
  }
  else
  {
    horn.TurnOff();
  }

  osMutexRelease(LightsState.mutex_id_);
  // TODO: REMOVE ONLY FOR DEBUG
//  etl::string<20> buff = "Throttle: ";
//  etl::to_string(THROTTLE_VAL, buff, etl::format_spec().width(4).fill(0), true);
//  buff.append("\r\n");
//  CDC_Transmit_FS((uint8_t*)buff.c_str(), 18);
}

void ReadIMU()
{
  LSM6DSR_Axes_t accel_info;
  LSM6DSR_ACC_GetAxes(&imu, &accel_info);
}

void ReadADC()
{
  // Read Throttle
  // If the throttle is pressed
  if(HAL_GPIO_ReadPin(Throttle_Switch_GPIO_Port, Throttle_Switch_Pin))
  {
    FLights.SetThrottleVal(0);
  }
  else
  {
    FLights.SetThrottleVal(throttle.Read());
  }
  THROTTLE_VAL = throttle.Read() >> 5; // FOR DEBUG

  // Read Breaks
  uint16_t breaksVal = breaks.Read();
  FLights.SetBreaksVal(breaksVal);
  //BREAKS_VAL = breaks.Read() >> 5; // FOR DEBUGin or buggin

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CANController.SetRxFlag();
  HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

bool FaultPresent()
{
  return
    bmsCodes.isInternalCellCommunicationFault() |
    bmsCodes.isCellBalancingStuckOffFault() |
    bmsCodes.isWeakCellFault() |
    bmsCodes.isLowCellVoltageFault() |
    bmsCodes.isCellOpenWiringFault() |
    bmsCodes.isCurrentSensorFault() |
    bmsCodes.isCellVoltageOver5vFault() |
    bmsCodes.isCellBankFault() |
    bmsCodes.isWeakPackFault() |
    bmsCodes.isFanMonitorFault() |
//    bmsCodes.isThermistorFault() |
    bmsCodes.isCanCommunicationFault() |
    bmsCodes.isRedundantPowerSupplyFault() |
    bmsCodes.isHighVoltageIsolationFault() |
    bmsCodes.isInvalidInputSupplyVoltageFault() |
//    bmsCodes.isChargeenableRelayFault() |
    bmsCodes.isDischargeenableRelayFault() |
    bmsCodes.isChargerSafetyRelayFault() |
    bmsCodes.isInternalHardwareFault() |
    bmsCodes.isInternalHeatsinkThermistorFault() |
    bmsCodes.isInternalLogicFault() |
    bmsCodes.isHighestCellVoltageTooHighFault() |
    bmsCodes.isLowestCellVoltageTooLowFault() |
    bmsCodes.isPackTooHotFault();
}
