/*
 * user.cpp
 *
 *  Created on: Jun 17, 2022
 *      Author: John Carr
 */

#include "user.hpp"

extern "C" void CPP_UserSetup(void);

void UpdateSignals(void);
void SendCanMsgs();
void ReadIMU();
void CheckBreaks(void);

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
  // Start Thread that Handles Turn Signal LEDs
  signal_timer_id = osTimerNew((osThreadFunc_t)UpdateSignals, osTimerPeriodic, NULL, &signal_timer_attr);
  if (signal_timer_id == NULL)
  {
      Error_Handler();
  }
  osTimerStart(signal_timer_id, 500);
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
  // Start Thread that Handles Reads the Breaks
  break_timer_id = osTimerNew((osThreadFunc_t)CheckBreaks, osTimerPeriodic, NULL, &break_timer_attr);
  if (break_timer_id == NULL)
  {
      Error_Handler();
  }
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
  osMutexRelease(LightsState.mutex_id_);
}

void SendCanMsgs()
{
  CANController.Send(&RLights);
}

void ReadIMU()
{
  LSM6DSR_Axes_t accel_info;
  LSM6DSR_ACC_GetAxes(&imu, &accel_info);
}

void CheckBreaks()
{
  // TODO check if breaks are pressed
  RLights.SetBreakReleased();
}
