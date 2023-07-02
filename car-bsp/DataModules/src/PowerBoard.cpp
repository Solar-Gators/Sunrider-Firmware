/*
 * FrontLights.cpp
 *
 *  Created on: Jan 17, 2022
 *      Author: John Carr
 */

#include <PowerBoard.hpp>
#include "DataModuleInfo.hpp"

namespace SolarGators {
namespace DataModules {
namespace {
  static constexpr uint32_t SIZE = 7;
}

PowerBoard::PowerBoard():
        DataModule(SolarGators::DataModuleInfo::POWER_BOARD_ID, 0, SIZE),
        SupBatVoltage_(0),
		SupBatPower_(0),
		MainBatPower_(0),
		PowerSource_(0)
{ }

PowerBoard::~PowerBoard()
{ }

float PowerBoard::GetSupBatVoltage()
{
  return ((float)SupBatVoltage_)/100;
}

float PowerBoard::GetSupBatPower()
{
  return ((float)SupBatPower_)/100;
}

float PowerBoard::GetMainBatPower()
{
	return ((float)MainBatPower_)/100;
}

uint8_t PowerBoard::GetPowerSource()
{
	return PowerSource_;
}


void PowerBoard::ToByteArray(uint8_t* buff)const
{
	buff[0] = SupBatVoltage_ >> 8;
	buff[1] = SupBatVoltage_ & 0xFF;
	buff[2] = SupBatPower_ >> 8;
	buff[3] = SupBatPower_ & 0xFF;
	buff[4] = MainBatPower_ >> 8;
	buff[5] = MainBatPower_ & 0xFF;
	buff[6] = PowerSource_;
}

void PowerBoard::FromByteArray(uint8_t* buff)
{
	SupBatVoltage_ = (static_cast<uint16_t>(buff[0]) << 8) | buff[1];
	SupBatPower_   = (static_cast<uint16_t>(buff[2]) << 8) | buff[3];
	MainBatPower_  = (static_cast<uint16_t>(buff[4]) << 8) | buff[5];
	PowerSource_ = buff[6];
}

#ifdef IS_TELEMETRY
  void PowerBoard::PostTelemetry(PythonScripts* scripts) {
    PythonHttp http;
	http.init();
	http.addData("SupBatVoltage_", getLowCellVolt());
	http.addData("SupBatPower_", getHighCellVolt());
	http.addData("MainBatPower_", getAvgCellVolt());
	if (GetPowerSource()) {
		scripts->send("powerBoard/rx0", http.getParameters());
	}
	else {
		scripts->send("powerBoard/rx1", http.getParameters());
	}
	http.flush();
  }
#endif

} /* namespace DataModules */
} /* namespace SolarGators */
