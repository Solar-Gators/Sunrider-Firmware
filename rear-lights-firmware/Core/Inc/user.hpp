/*
 * user.hpp
 *
 *  Created on: Jun 17, 2022
 *      Author: John Carr
 */

#ifndef INC_USER_HPP_
#define INC_USER_HPP_

#include "main.h"
#include "lsm6dsr.h"
#include "custom_bus.h"
#include "Steering.hpp"
#include "RearLightsController.hpp"
#include "CAN.hpp"
#include "LED.hpp"

// DataModules
SolarGators::DataModules::Steering LightsState;
SolarGators::DataModules::RearLightsController RLights;

// CAN Bus Stuff
extern CAN_HandleTypeDef hcan;
SolarGators::Drivers::CANDriver CANController(&hcan, 0);

// IMU Stuff
LSM6DSR_IO_t imu_bus =
    {
        .Init = BSP_I2C2_Init,
        .DeInit = BSP_I2C2_DeInit,
        .BusType = LSM6DSR_I2C_BUS,
        .Address = LSM6DSR_I2C_ADD_H,
        .WriteReg = BSP_I2C2_WriteReg,
        .ReadReg = BSP_I2C2_ReadReg,
        .GetTick = BSP_GetTick
    };
LSM6DSR_Object_t imu;

SolarGators::Drivers::LED lt_indicator    ("LT Indicator", LT_GPIO_Port, LT_GPIO_Pin);
SolarGators::Drivers::LED rt_indicator    ("RT Indicator", RT_GPIO_Port, RT_GPIO_Pin);


#endif /* INC_USER_HPP_ */
