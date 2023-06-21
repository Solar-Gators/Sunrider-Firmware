/*
 * PitComms.cpp
 *
 *  Created on: Feb 3, 2022
 *      Author: John Carr
 */

#include "PitComms.hpp"
#include "crc16.hpp"

namespace SolarGators {
namespace Drivers {

PitComms::PitComms(SolarGators::Drivers::Radio* radio):radio_(radio)
{
  radio_->Init();
}

PitComms::~PitComms()
{
  // TODO Auto-generated destructor stub
}

void PitComms::SendDataModule(SolarGators::DataModules::DataModule& data_module)
{
  // Start Condition
  osMutexAcquire(data_module.mutex_id_, osWaitForever);
  radio_->SendByte(START_CHAR);
  // Only Sending one Datamodule
  radio_->SendByte(EscapeData((data_module.can_id_ & 0xFF000000) >> 24));
  radio_->SendByte(EscapeData((data_module.can_id_ & 0x00FF0000) >> 16));
  radio_->SendByte(EscapeData((data_module.can_id_ & 0x0000FF00) >> 8));
  radio_->SendByte(EscapeData(data_module.can_id_ & 0x000000FF));
  radio_->SendByte(EscapeData(data_module.instance_id_));
  radio_->SendByte(EscapeData(data_module.size_));
  // Temporary buffer
  uint8_t buff[100];
  data_module.ToByteArray(buff);
  // Calculate and send CRC
  uint16_t crc = SolarGators::Helpers::crc16(buff, data_module.size_);
  uint8_t lo_crc = crc & 0xFF;
  uint8_t hi_crc = (crc >> 8) & 0xFF;
  EscapeData(hi_crc);
  radio_->SendByte(hi_crc);
  EscapeData(lo_crc);
  radio_->SendByte(lo_crc);
  // Send Buffer
  for (uint16_t i = 0; i < data_module.size_; ++i) {
    EscapeData(buff[i]);
    radio_->SendByte(buff[i]);
  }
  // End condition
  radio_->SendByte(END_CHAR);
  osMutexRelease(data_module.mutex_id_);
}

inline uint8_t PitComms::EscapeData(uint8_t data)
{
  if(data == START_CHAR || data == END_CHAR || data == ESC_CHAR)
  {
    radio_->SendByte(ESC_CHAR);
  }
  return data;
}

} /* namespace Drivers */
} /* namespace SolarGators */
