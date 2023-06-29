/*
 * RearLights.cpp
 *
 *  Created on: Jun 17, 2022
 *      Author: John Carr
 */

#include <RearLights.hpp>

namespace SolarGators {
namespace DataModules {

namespace {
  static constexpr uint32_t ID = 0x3e0;
  static constexpr uint32_t SIZE = 1;
}

RearLights::RearLights():
            DataModule(ID, 0, SIZE),
            break_(false),
			contactor_status_(true)

{

  // TODO Auto-generated constructor stub

}

RearLights::~RearLights() {
  // TODO Auto-generated destructor stub
}


bool RearLights::isBreakPressed() const
{
  return break_;
}

void RearLights::ToByteArray(uint8_t* buff) const
{
  buff[0] = static_cast<uint8_t>(break_);
  buff[0] |= (static_cast<uint8_t>(contactor_status_) << 1);
  buff[0] |= (static_cast<uint8_t>(killsw_) << 2);

}

bool RearLights::getKillSwStatus() const{
       return killsw_;
}

void RearLights::setKillSwStatus(bool status){
       killsw_ = status;
}

bool RearLights::getContactorStatus() const{
       return contactor_status_;
}

void RearLights::setContactorStatus(bool status){
       contactor_status_ = status;
}

void RearLights::FromByteArray(uint8_t* buff)
{
  break_ = static_cast<bool>(buff[0] & 0x1);
  contactor_status_ = static_cast<bool>(buff[0] & (0x1<<1));
  killsw_ = static_cast<bool>(buff[0] & (0x1<<2));

}

} /* namespace DataModules */
} /* namespace SolarGators */
