/*
 * FrontLights.hpp
 *
 *  Created on: Jan 17, 2022
 *      Author: John Carr
 */

#ifndef SOLARGATORSBSP_STM_DATAMODULES_INC_FRONTLIGHTS_HPP_
#define SOLARGATORSBSP_STM_DATAMODULES_INC_FRONTLIGHTS_HPP_

#include <DataModule.hpp>

namespace SolarGators {
namespace DataModules {

class FrontLights: public DataModule {
public:
  FrontLights();
  ~FrontLights();
  uint16_t GetThrottleVal() const;

  // CAN Functions
  void ToByteArray(uint8_t* buff) const;
  void FromByteArray(uint8_t* buff);
  #ifdef IS_TELEMETRY
  void PostTelemetry(PythonScripts* scripts);
  #endif

protected:
  uint16_t throttle_;
  // TODO: Accelerometer values
};

} /* namespace DataModules */
} /* namespace SolarGators */

#endif /* SOLARGATORSBSP_STM_DATAMODULES_INC_FRONTLIGHTS_HPP_ */
