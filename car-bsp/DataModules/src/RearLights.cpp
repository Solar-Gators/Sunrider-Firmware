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
  static constexpr uint32_t ID = 0x2345;
  static constexpr uint32_t SIZE = 1;
}

RearLights::RearLights():
            DataModule(ID, 0, SIZE),
            break_(false) {
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

}
void RearLights::FromByteArray(uint8_t* buff)
{
  break_ = static_cast<bool>(buff[0] & 0x1);
}

} /* namespace DataModules */
} /* namespace SolarGators */
