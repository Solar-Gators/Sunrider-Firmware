
#ifndef USER_HPP_
#define USER_HPP_

#include "main.h"
#include "lsm6dsr.h"
#include "custom_bus.h"
#include "Steering.hpp"
#include "CAN.hpp"
#include "LED.hpp"
#include "MPU6050.hpp"
#include "MCP33151.hpp"
#include "RearLights.hpp"
#include "FrontLights.hpp"
#include "OrionBMS.hpp"
#include "DataModuleInfo.hpp"

// Datamodules
SolarGators::DataModules::Steering LightsState;
SolarGators::DataModules::FrontLights FLights;
SolarGators::DataModules::RearLights RLights;
SolarGators::DataModules::OrionBMSRx4 bmsCodes(SolarGators::DataModuleInfo::BMS_RX4_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx2 bmsCurrent(SolarGators::DataModuleInfo::BMS_RX2_MSG_ID, 0);

extern CAN_HandleTypeDef hcan;
SolarGators::Drivers::CANDriver CANController(&hcan, 0);
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

extern SPI_HandleTypeDef hspi1;
// Leds
SolarGators::Drivers::LED lt_indicator    ("LT Indicator", LT_GPIO_Port, LT_GPIO_Pin);
SolarGators::Drivers::LED rt_indicator    ("RT Indicator", RT_GPIO_Port, RT_GPIO_Pin);
SolarGators::Drivers::LED contactor_relay ("Contactor Relay", RELAY_EN_GPIO_Port, RELAY_EN_Pin);
SolarGators::Drivers::LED tlr_indicator   ("TLR Indicator", TLR_GPIO_Port, TLR_GPIO_Pin);
SolarGators::Drivers::LED strobeLight	  ("Strobe", STROBE_GPIO_Port, STROBE_GPIO_Pin);


#endif
