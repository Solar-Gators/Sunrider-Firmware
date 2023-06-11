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
  CANController.AddRxModule(&FLights);
  CANController.AddRxModule(&bmsCurrent);
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

  if(LightsState.GetHazardsStatus())
    {
      if (lt_indicator.IsOn())
      {
        lt_indicator.TurnOff();
        rt_indicator.TurnOff();
      }
      else
      {
        lt_indicator.TurnOn();
        rt_indicator.TurnOn();
      }
    }
    else if(LightsState.GetRightTurnStatus())
      rt_indicator.Toggle();
    else if(LightsState.GetLeftTurnStatus())
      lt_indicator.Toggle();

    if(!LightsState.GetHazardsStatus() && !LightsState.GetRightTurnStatus())
      rt_indicator.TurnOff();

    if(!LightsState.GetHazardsStatus() && !LightsState.GetLeftTurnStatus())
      lt_indicator.TurnOff();

    if(LightsState.GetHeadlightsStatus()){
  	  //should add code here to keep lights on but dim
    }

    if(FLights.GetBreaksVal() > 30){
  	  rt_indicator.TurnOn();
  	  lt_indicator.TurnOn();
    }

  osMutexRelease(LightsState.mutex_id_);

}

void ReadIMU()
{
  LSM6DSR_Axes_t accel_info;
  LSM6DSR_ACC_GetAxes(&imu, &accel_info);
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
		}
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
