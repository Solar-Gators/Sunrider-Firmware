/*
 * PowerBoard.hpp
 *
 *  Created on: Apr 23, 2023
 *      Author: yasha
 */

#ifndef SOLARGATORSBSP_STM_CAN_DECODER_SRC_DATAMODULES_INC_POWERBOARD_HPP_
#define SOLARGATORSBSP_STM_CAN_DECODER_SRC_DATAMODULES_INC_POWERBOARD_HPP_

#include <DataModule.hpp>

namespace SolarGators {
namespace DataModules {

class PowerBoard: public DataModule {
public:
  PowerBoard(uint32_t can_id, uint32_t telem_id);
  ~PowerBoard();
  float GetSupBatVoltage();
  float GetSupBatPower();
  float GetMainBatPower();
  void setSupBatVoltage(uint16_t SupBatVoltage);
  void setSupBatPower(uint16_t SupBatPower);
  void setMainBatPower(uint16_t MainBatPower);
  void setPowerSource(uint8_t PowerSource);
  uint8_t GetPowerSource();
  // CAN Functions
  void ToByteArray(uint8_t* buff) const;
  void FromByteArray(uint8_t* buff);
  static constexpr uint8_t Size = 7;
protected:
  uint16_t SupBatVoltage_; //has been converted from float
  uint16_t SupBatPower_;   //has been converted from float
  uint16_t MainBatPower_;  //has been converted from float
  uint8_t PowerSource_;    //bool for sup/main, 0 for sup, 1 for main
};

} /* namespace DataModules */
} /* namespace SolarGators */



#endif /* SOLARGATORSBSP_STM_CAN_DECODER_SRC_DATAMODULES_INC_POWERBOARD_HPP_ */
