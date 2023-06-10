
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
#include "FrontLightsController.hpp"
#include "OrionBMS.hpp"
#include "DataModuleInfo.hpp"

// Datamodules
SolarGators::DataModules::Steering LightsState;
SolarGators::DataModules::FrontLightsController FLights;
SolarGators::DataModules::OrionBMSRx4 bmsCodes(SolarGators::DataModuleInfo::BMS_RX4_MSG_ID, 0);

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

SolarGators::Drivers::fpoint_3d_t ACCEL;
extern SPI_HandleTypeDef hspi1;

// Throttle
SolarGators::Drivers::MCP33151 throttle(Throttle_CS_GPIO_Port, Throttle_CS_Pin, &hspi1);
uint16_t THROTTLE_VAL = 0;

// Brakes
SolarGators::Drivers::MCP33151 breaks(Break_CS_GPIO_Port, Break_CS_Pin, &hspi1);
uint16_t BREAKS_VAL = 0;

// Leds
SolarGators::Drivers::LED lt_indicator    ("LT Indicator", LT_GPIO_Port, LT_GPIO_Pin);
SolarGators::Drivers::LED rt_indicator    ("RT Indicator", RT_GPIO_Port, RT_GPIO_Pin);
SolarGators::Drivers::LED hlr_indicator   ("HLR Indicator", HLR_GPIO_Port, HLR_GPIO_Pin);
SolarGators::Drivers::LED hll_indicator   ("HLL Indicator", HLL_GPIO_Port, HLL_GPIO_Pin);
SolarGators::Drivers::LED fault_indicator ("Fault Indicator", FI_GPIO_Port, FI_GPIO_Pin);
SolarGators::Drivers::LED horn            ("Horn", HRN_GPIO_Port, HRN_GPIO_Pin);
SolarGators::Drivers::LED headlightR      ("R Headlight", RHeadlight_GPIO_Port, RHeadlight_Pin);
SolarGators::Drivers::LED headlightL      ("L Headlight", LHeadlight_GPIO_Port, LHeadlight_Pin);

#endif
