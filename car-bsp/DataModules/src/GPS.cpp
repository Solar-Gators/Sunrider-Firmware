/*
 * GPS.cpp
 *
 *  Created on: Jun 4, 2023
 *    Author: Raymond Bernardo
 */

#include <GPS.hpp>
#include <string.h>
#include <cmath>
#ifdef IS_TELEMETRY
#include <stdio.h>
#endif

namespace SolarGators {
namespace DataModules {
namespace {
  static constexpr uint32_t ID = 0x567;
  static constexpr uint32_t SIZE = 15;
}

GPS::GPS():
    DataModule(ID, 0, GPS_TRANSMISSION_SIZE),
    latitude(0),
    longitude(0),
    speed(0),
    trueCourse(0)
{ }

GPS::~GPS()
{ }

float GPS::getLatitude()
{
  return latitude;
}

float GPS::getLongitude()
{
  return longitude;
}

float GPS::getSpeed()
{
  return speed;
}

float GPS::getTrueCourse()
{
  return trueCourse;
}

void GPS::ToByteArray(uint8_t* buff) const
{
  // not going to implement for now
  memcpy(buff, lastTransmission, GPS_TRANSMISSION_SIZE);
}

double ddmToDd(double degrees, double minutes) {
  // do abs(degrees)
  degrees = degrees < 0 ? -degrees : degrees;

  double dd = degrees + minutes / 60.0;
  return degrees < 0 ? -dd : dd;
}


void GPS::FromByteArray(uint8_t* buff)
{
  const char* start = (char*)buff; // protocol
  if(!strstr(start, "$GPRMC")) return;
  const char* end;

  start = strchr((char*)start, ',') + 1;
  start = strchr(start, ',') + 1; // skip timestamp
  start = strchr(start, ',') + 1; // status
  //V == invalid
  if(start[0] == 'V') return;

  memcpy(lastTransmission, buff, GPS_TRANSMISSION_SIZE);

  // don't need to parse anything on the car,
  // since we sent the entire message over
  #ifdef IS_TELEMETRY
  // Latitude
  end = strchr(start, ',');
  char temp[10];
  strncpy(temp, start, 2);
  temp[2] = '\0';
  double latitude_ddm_deg = atof(temp);
  double latitude_ddm_min = atof(start + 2);
  start = end + 1;

  double latitude_dd = ddmToDd(latitude_ddm_deg, latitude_ddm_min);
  if (*start == 'S') {
    latitude_dd = -latitude_dd;
  }
  this->latitude = (float)latitude_dd;
  start = strchr(start, ',') + 1;

  // Longitude
  end = strchr(start, ',');
  strncpy(temp, start, 3);
  temp[3] = '\0';
  double longitude_ddm_deg = atof(temp);
  double longitude_ddm_min = atof(start + 3);
  start = end + 1;

  double longitude_dd = ddmToDd(longitude_ddm_deg, longitude_ddm_min);
  if (*start == 'W') {
    longitude_dd = -longitude_dd;
  }
  this->longitude = (float)longitude_dd;
  start = strchr(start, ',') + 1;

  // Speed
  end = strchr(start, ',');
  double speed_knots = atof(start);
  this->speed = (float)speed_knots;
  start = end + 1;

  // True course
  end = strchr(start, ',');
  double true_course_deg = atof(start);
  this->trueCourse = (float)true_course_deg;
#endif
}


#ifdef IS_TELEMETRY
void GPS::PostTelemetry(PythonScripts* scripts) {
  if (getLatitude() > 0) {
    PythonHttp http;
    http.init();
    http.addData("heading", getTrueCourse());
    http.addData("latitude", getLatitude());
    http.addData("longitude", getLongitude());
    http.addData("speed", getSpeed());
    scripts->send("gps/rx0", http.getParameters());
    http.flush();
  }
}
#endif

} /* namespace DataModules */
} /* namespace SolarGators */
