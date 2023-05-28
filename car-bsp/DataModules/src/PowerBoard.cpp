/*
 * PowerBoard.cpp
 *
 *  Created on: Apr 23, 2023
 *      Author: yasha
 */

#include <PowerBoard.hpp>

namespace SolarGators {
namespace DataModules {

//Data for power board information
PowerBoard::PowerBoard(uint32_t can_id, uint32_t telem_id):
        DataModule(can_id, 0, this->Size),
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

void PowerBoard::setSupBatVoltage(uint16_t SupBatVoltage){
	SupBatVoltage_ = SupBatVoltage;
}

void PowerBoard::setSupBatPower(uint16_t SupBatPower){
	SupBatPower_ = SupBatPower;
}

void PowerBoard::setMainBatPower(uint16_t MainBatPower){
	MainBatPower_ = MainBatPower;
}

void PowerBoard::setPowerSource(uint8_t PowerSource){
	PowerSource_ = PowerSource;
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

} /* namespace DataModules */
} /* namespace SolarGators */
