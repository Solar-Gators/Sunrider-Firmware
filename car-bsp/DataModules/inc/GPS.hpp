/*
 * GPS.hpp
 *
 *  Created on: Jun 4, 2023
 *      Author: Raymond Bernardo
 */

#ifndef SOLARGATORSBSP_STM_DATAMODULES_INC_GPS_HPP_
#define SOLARGATORSBSP_STM_DATAMODULES_INC_GPS_HPP_

#include "DataModule.hpp"

#define GPS_LATITUDE_BYTES 11
#define GPS_LONGITUDE_BYTES 12
#define GPS_SPEED_BYTES 10
#define GPS_TRUECOURSE_BYTES 10
#define GPS_MSG_MAX_BYTES (GPS_LATITUDE_BYTES + GPS_LONGITUDE_BYTES + GPS_SPEED_BYTES + GPS_TRUECOURSE_BYTES)
#define GPS_TRANSMISSION_SIZE 75

namespace SolarGators {
namespace DataModules {

class GPS: public DataModule {
public:
  GPS();
  virtual ~GPS();
  float getLatitude();
  float getLongitude();
  float getSpeed();
  float getTrueCourse();

  #ifdef IS_TELEMETRY
    	void PostTelemetry(PythonScripts* scripts);
		uint8_t getMpptNo();
  #endif

  // CAN Functions
  void ToByteArray(uint8_t* buff) const;
  void FromByteArray(uint8_t* buff);
protected:
  uint8_t lastTransmission[GPS_TRANSMISSION_SIZE];
  float latitude; //Format for lat lon is: lat: DDMM.MMMM(N/S) lon: DDDMM.MMMM(E/W)
  float longitude; //4042.6142,N 07400.4168,W. = +40  42.6142', -74  00.4168' in google maps
  float speed; //This is in knots
  float trueCourse; //This is in degrees
};

} /* namespace DataModules */
} /* namespace SolarGators */

#endif /* SOLARGATORSBSP_STM_DATAMODULES_INC_GPS_HPP_ */
