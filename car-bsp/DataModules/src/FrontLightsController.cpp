/*
 * FrontLightsController.cpp
 *
 *  Created on: Jun 13, 2022
 *      Author: John Carr
 */

#include <FrontLightsController.hpp>

namespace SolarGators {
namespace DataModules {

FrontLightsController::FrontLightsController()
{ }

FrontLightsController::~FrontLightsController()
{ }

void FrontLightsController::SetAcknowledgement(bool val)
{
	acknowledgement_ = val;
}

void FrontLightsController::SetThrottleVal(uint16_t val)
{
  throttle_ = val;
}

void FrontLightsController::SetBreaksVal(uint16_t val)
{
	breaks_ = val;
}

} /* namespace DataModules */
} /* namespace SolarGators */
