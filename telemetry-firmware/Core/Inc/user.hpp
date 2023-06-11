
#ifndef USER_HPP_
#define USER_HPP_

#include "main.h"

// IMU Includes
#include "lsm6dsr.h"
#include "custom_bus.h"

// Drivers
#include "CAN.hpp"
#include "RFD900x.hpp"
#include "PitComms.hpp"
#include "LTC2630.hpp"
#include "LED.hpp"

// Data Module Info
#include "DataModuleInfo.hpp"

// Data Modules
#include "Steering.hpp"
#include "FrontLights.hpp"
#include "RearLights.hpp"
#include "OrionBMS.hpp"
#include "Proton1.hpp"
#include "Mitsuba.hpp"
#include "PowerBoard.hpp"
#include "Mppt.hpp"
#include "GPS.hpp"
#include "GPS_Driver.hpp"

extern CAN_HandleTypeDef hcan;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern SPI_HandleTypeDef hspi2;

SolarGators::Drivers::CANDriver CANController(&hcan, 0);
SolarGators::Drivers::RFD900x rfd(&huart2);
SolarGators::Drivers::PitComms pit(&rfd);
enum {
	level0 = 0,
	level1 = 84,
	level2 = 180,
	level3 = 255
} regenLevelEnum;

// DACs
SolarGators::Drivers::LTC2630 accel(&hspi2, Accel_CS_GPIO_Port, Accel_CS_Pin, SolarGators::Drivers::OperatingMode::Bit8);
SolarGators::Drivers::LTC2630 regen(&hspi2, Regen_CS_GPIO_Port, Regen_CS_Pin, SolarGators::Drivers::OperatingMode::Bit8);

// Mitsuba Controls
SolarGators::Drivers::LED eco("Eco Enable", Eco_En_GPIO_Port, Eco_En_Pin);
SolarGators::Drivers::LED reverse("Reverse Enable", Reverse_EN_GPIO_Port, Reverse_EN_Pin);

// IMU
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

// Objects we want to get status on
// TODO: Steering
SolarGators::DataModules::FrontLights FLights;
SolarGators::DataModules::RearLights RLights;
SolarGators::DataModules::Steering Steering;
SolarGators::DataModules::GPS Gps;
SolarGators::DataModules::PowerBoard Pboard(SolarGators::DataModuleInfo::POWER_BOARD_ID, 0);
SolarGators::DataModules::MitsubaRequest Motor_Tx(SolarGators::DataModuleInfo::MOTORTX_RL_MSG_ID);

// Objects that we will want to send data to the pit on
SolarGators::DataModules::OrionBMSRx0 BMS_Rx_0(SolarGators::DataModuleInfo::BMS_RX0_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx1 BMS_Rx_1(SolarGators::DataModuleInfo::BMS_RX1_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx2 BMS_Rx_2(SolarGators::DataModuleInfo::BMS_RX2_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx3 BMS_Rx_3(SolarGators::DataModuleInfo::BMS_RX3_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx4 BMS_Rx_4(SolarGators::DataModuleInfo::BMS_RX4_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx5 BMS_Rx_5(SolarGators::DataModuleInfo::BMS_RX5_MSG_ID, 0);

SolarGators::DataModules::MitsubaRx0 Motor_Rx_0(SolarGators::DataModuleInfo::MOTORRX0_RL_MSG_ID, 0x04);
SolarGators::DataModules::MitsubaRx1 Motor_Rx_1(SolarGators::DataModuleInfo::MOTORRX1_RL_MSG_ID, 0x04);
SolarGators::DataModules::MitsubaRx2 Motor_Rx_2(SolarGators::DataModuleInfo::MOTORRX2_RL_MSG_ID, 0x04);

//registering mppt data modules
//there are several more modules than this, but i only want to register essential info from each
SolarGators::DataModules::Mpptx0 MPPT0_Rx_0(SolarGators::DataModuleInfo::MPPT0_RX0_MSG_ID);
SolarGators::DataModules::Mpptx0 MPPT1_Rx_0(SolarGators::DataModuleInfo::MPPT1_RX0_MSG_ID);
SolarGators::DataModules::Mpptx0 MPPT2_Rx_0(SolarGators::DataModuleInfo::MPPT2_RX0_MSG_ID);

SolarGators::DataModules::Mpptx1 MPPT0_Rx_1(SolarGators::DataModuleInfo::MPPT0_RX1_MSG_ID);
SolarGators::DataModules::Mpptx1 MPPT1_Rx_1(SolarGators::DataModuleInfo::MPPT1_RX1_MSG_ID);
SolarGators::DataModules::Mpptx1 MPPT2_Rx_1(SolarGators::DataModuleInfo::MPPT2_RX1_MSG_ID);

SolarGators::DataModules::Mpptx5 MPPT0_Rx_5(SolarGators::DataModuleInfo::MPPT0_RX5_MSG_ID);
SolarGators::DataModules::Mpptx5 MPPT1_Rx_5(SolarGators::DataModuleInfo::MPPT1_RX5_MSG_ID);
SolarGators::DataModules::Mpptx5 MPPT2_Rx_5(SolarGators::DataModuleInfo::MPPT2_RX5_MSG_ID);


void UART3_RX_Handler();

//bool CANStatusFlag = false;

#endif
