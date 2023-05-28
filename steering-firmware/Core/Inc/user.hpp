

#ifndef USER_HPP_
#define USER_HPP_

#include "main.h"
#include "SteeringController.hpp"
#include "Mitsuba.hpp"
#include "OrionBMS.hpp"
#include "DataModuleInfo.hpp"
#include "Button.hpp"
#include "CAN.hpp"
#include "ILI9341.hpp"
#include "ILI9341_CMD.hpp"


SolarGators::DataModules::SteeringController LightsState;
SolarGators::DataModules::MitsubaRequest McReq(SolarGators::DataModuleInfo::MOTORTX_RL_MSG_ID);
SolarGators::DataModules::OrionBMSRx0 BMS_Rx_0(SolarGators::DataModuleInfo::BMS_RX0_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx2 BMS_Rx_2(SolarGators::DataModuleInfo::BMS_RX2_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx4 BMS_Rx_4(SolarGators::DataModuleInfo::BMS_RX4_MSG_ID, 0);
SolarGators::DataModules::MitsubaRx0 Motor_Rx_0(SolarGators::DataModuleInfo::MOTORRX0_RL_MSG_ID, 0x04);
SolarGators::DataModules::MitsubaRx2 Motor_Rx_2(SolarGators::DataModuleInfo::MOTORRX2_RL_MSG_ID, 0x04);

// Forward Declares
extern CAN_HandleTypeDef hcan;
extern TIM_HandleTypeDef htim2;

// CAN Driver
SolarGators::Drivers::CANDriver CANController(&hcan, 0);

// Buttons
// Left Side
SolarGators::Drivers::Button left_turn    ("Left Turn", LT_Button_GPIO_Port, LT_Button_Pin);
SolarGators::Drivers::Button cruise_minus ("Cruise Minus", Cruise_M_GPIO_Port, Cruise_M_Pin);
SolarGators::Drivers::Button eco          ("Eco Mode", Eco_Button_GPIO_Port, Eco_Button_Pin);
SolarGators::Drivers::Button headlights   ("Headlights", HL_Button_GPIO_Port, HL_Button_Pin);
SolarGators::Drivers::Button hazards      ("Hazards", Haz_Button_GPIO_Port, Haz_Button_Pin);
// Right Side
SolarGators::Drivers::Button right_turn   ("Right Turn", RT_Button_GPIO_Port, RT_Button_Pin);
SolarGators::Drivers::Button cruise_plus  ("Cruise Plus", Cruise_P_GPIO_Port, Cruise_P_Pin);
//SolarGators::Drivers::Button horn         ("Horn", Horn_Button_GPIO_Port, Horn_Button_Pin);
SolarGators::Drivers::Button cruise       ("Cruise", Cruise_Button_GPIO_Port, Cruise_Button_Pin);
SolarGators::Drivers::Button reverse      ("Reverse", Reverse_Button_GPIO_Port, Reverse_Button_Pin);
// Leds
// Left Side
SolarGators::Drivers::LED lt_indicator    ("LT Indicator", LT_Led_GPIO_Port, LT_Led_Pin);
SolarGators::Drivers::LED eco_indicator   ("ECO Indicator", Eco_Led_GPIO_Port, Eco_Led_Pin);
SolarGators::Drivers::LED hl_indicator    ("HL Indicator", HL_Led_GPIO_Port, HL_Led_Pin);
// Right Side
SolarGators::Drivers::LED rt_indicator    ("RT Indicator", RT_Led_GPIO_Port, RT_Led_Pin);
SolarGators::Drivers::LED cr_indicator    ("CR Indicator", Cruise_Led_GPIO_Port, Cruise_Led_Pin);
SolarGators::Drivers::LED rev_indicator   ("Rev Indicator", Reverse_Led_GPIO_Port, Reverse_Led_Pin);

#endif
