/*
 * user.cpp
 *
 *  Created on: Jun 17, 2022
 *      Author: John Carr, Yash Bhat
 */

#include "user.hpp"

extern "C" void CPP_UserSetup(void);

void UpdateSignals(void);
void SendCanMsgs();
//void ReadIMU();
void KillConditions();

// OS Configs
osTimerId_t signal_timer_id;
osTimerAttr_t signal_timer_attr =
{
  .name = "Lights Timer"
};
/* Definitions for Kill Switch and Fault Thread*/
osTimerId_t kill_sw_timer_id;
osTimerAttr_t kill_sw_timer_attr = {
		.name = "Kill_Sw"
};
/* Definitions for IMU Thread */
osTimerId_t imu_timer_id;
osTimerAttr_t imu_timer_attr =
{
  .name = "IMU"
};
/* Definitions for the checking break Thread */
osTimerId_t break_timer_id;
osTimerAttr_t break_timer_attr =
{
  .name = "Break"
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
  CANController.AddRxModule(&FLights);
  // Start Thread that Handles Turn Signal LEDs
  signal_timer_id = osTimerNew((osThreadFunc_t)UpdateSignals, osTimerPeriodic, NULL, &signal_timer_attr);
  if (signal_timer_id == NULL)
  {
      Error_Handler();
  }
  osTimerStart(signal_timer_id, 500);
  kill_sw_timer_id = osTimerNew((osThreadFunc_t)KillConditions, osTimerPeriodic, NULL, &kill_sw_timer_attr);
  if(kill_sw_timer_id == NULL){
	  Error_Handler();
  }
  osTimerStart(kill_sw_timer_id, 250);

  CANController.Init();
  /*
  // Start Thread that Handles Reads the IMU
  imu_timer_id = osTimerNew((osThreadFunc_t)ReadIMU, osTimerPeriodic, NULL, &imu_timer_attr);
  if (imu_timer_id == NULL)
  {
      Error_Handler();
  }

  if(LSM6DSR_RegisterBusIO(&imu, &imu_bus))
  {
    Error_Handler();
  }
  if(LSM6DSR_Init(&imu))
  {
    Error_Handler();
  }
  if(LSM6DSR_ACC_Enable(&imu))
  {
    Error_Handler();
  }
  osTimerStart(imu_timer_id, 500);
	*/
  // Start Thread that Handles Reads the Breaks
  // do we need to check breaks constantly? we will need to read can, and that should update internal datamodule
  /*
  break_timer_id = osTimerNew((osThreadFunc_t)CheckBreaks, osTimerPeriodic, NULL, &break_timer_attr);
  if (break_timer_id == NULL)
  {
      Error_Handler();
  }
  */

  //osTimerStart(break_timer_id, 500);

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

  if(FLights.GetBreaksVal() > 10){
	  rt_indicator.TurnOn();
	  lt_indicator.TurnOn();
  }
  osMutexRelease(LightsState.mutex_id_);
}

void SendCanMsgs()
{
	CANController.Send(&RLights);
}
/*
void ReadIMU()
{
  LSM6DSR_Axes_t accel_info;
  LSM6DSR_ACC_GetAxes(&imu, &accel_info);
}
*/

void KillConditions(){
	//Check position of the kill switch, add it to data module, cut contactor if necessary
	//Check for BMS condition, cut off contactor if necessary
	if(HAL_GPIO_ReadPin(KILL_SW_GPIO_Port, KILL_SW_Pin)){
		RLights.setKillSwStatus(false);
	} else{
		RLights.setKillSwStatus(true);
		RLights.setContactorStatus(false);
		HAL_GPIO_WritePin(HORN_EN_GPIO_Port, HORN_EN_Pin, GPIO_PIN_RESET);
	}
	//This contains the regulation critical full car trip if pack is charging and charge temp limit is exceeded
	if((bmsCurrent.getPackCurrent() > 0) && bmsCodes.isChargeenableRelayFault()){
		HAL_GPIO_WritePin(HORN_EN_GPIO_Port, HORN_EN_Pin, GPIO_PIN_RESET);
		RLights.setContactorStatus(false);
	}
}
