/*
 * FrontLights.hpp
 *
 *  Created on: Jan 17, 2022
 *      Author: John Carr
 */

#ifndef SOLARGATORSBSP_STM_DATAMODULES_INC_FRONTLIGHTS_HPP_
#define SOLARGATORSBSP_STM_DATAMODULES_INC_FRONTLIGHTS_HPP_

#include <DataModule.hpp>
#define BUFF_SIZE 4

namespace SolarGators {
namespace DataModules {

class FrontLights: public DataModule {
public:
  FrontLights();
  ~FrontLights();
  uint16_t GetThrottleVal() const;
  uint16_t GetBreaksVal() const;
  bool GetAcknowledgementVal() const;
  uint8_t buffCtr;
  uint16_t breaksBuffer[BUFF_SIZE];
  // CAN Functions
  void ToByteArray(uint8_t* buff) const;
  void FromByteArray(uint8_t* buff);
  #ifdef IS_TELEMETRY
  void PostTelemetry(PythonScripts* scripts);
  #endif

protected:
  uint16_t throttle_;
  uint16_t breaks_;
  bool acknowledgement_;
  // TODO: Accelerometer values
};

} /* namespace DataModules */
} /* namespace SolarGators */

#endif /* SOLARGATORSBSP_STM_DATAMODULES_INC_FRONTLIGHTS_HPP_ */
