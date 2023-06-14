/*
 * FrontLights.cpp
 *
 *  Created on: Jan 17, 2022
 *      Author: John Carr
 */

#include <FrontLights.hpp>

namespace SolarGators {
namespace DataModules {
namespace {
  static constexpr uint32_t ID = 0x234;
  static constexpr uint32_t SIZE = 4;
}

FrontLights::FrontLights():
        DataModule(ID, 0, SIZE),
        throttle_(0),
		breaks_(0),
		buffCtr(0)
{ }

FrontLights::~FrontLights()
{ }

uint16_t FrontLights::GetThrottleVal() const
{
  return throttle_;
}

uint16_t FrontLights::GetBreaksVal() const
{
	return breaks_;
}

void FrontLights::ToByteArray(uint8_t* buff) const
{
  buff[0] = static_cast<uint8_t>(throttle_);
  buff[1] = static_cast<uint8_t>(throttle_ >> 8);
  buff[2] = static_cast<uint8_t>(breaks_);
  buff[3] = static_cast<uint8_t>(breaks_ >> 8);
}
void FrontLights::FromByteArray(uint8_t* buff)
{
  throttle_ = static_cast<uint16_t>(buff[1]) << 8 | buff[0];
  breaks_ = static_cast<uint16_t>(buff[3]) << 8 | buff[2];
}

#ifdef IS_TELEMETRY
void FrontLights::PostTelemetry(PythonScripts* scripts) {

}
#endif

} /* namespace DataModules */
} /* namespace SolarGators */
