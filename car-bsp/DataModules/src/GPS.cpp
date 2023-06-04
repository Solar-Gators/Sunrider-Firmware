/*
 * GPS.cpp
 *
 *  Created on: Jun 4, 2023
 *      Author: Raymond Bernardo
 */

#include <GPS.hpp>
#include <string.h>

namespace SolarGators {
namespace DataModules {
namespace {
  static constexpr uint32_t ID = 0x567;
  static constexpr uint32_t SIZE = 2;
}

GPS::GPS():
        DataModule(ID, 0, SIZE),
        latitude("0"),
        longitude("0"),
        speed("0"),
        trueCourse("0")
{ }

GPS::~GPS()
{ }

const char* GPS::getLatitude()
{
  return latitude;
}

const char* GPS::getLongitude()
{
  return longitude;
}

const char* GPS::getSpeed()
{
  return speed;
}

const char* GPS::getTrueCourse()
{
  return trueCourse;
}

void GPS::ToByteArray(uint8_t* buff) const
{
    // not going to implement for now
}
void GPS::FromByteArray(uint8_t* buff)
{
  if(!strstr((char*)buff, "$GPRMC")) return;

	const char* p = (char*)buff;
	p = strchr(p, ',') + 1; //Skip header
	p = strchr(p, ',') + 1; //Skip date
	//V == invalid
	if(p[0] == 'V') return;
	//Handle Latitude
	p = strchr(p, ',') + 1; //Go to lat
	strncpy(latitude, p, 9);
	p = strchr(p, ',') + 1; //Go to lat-dir
	strncpy(latitude + 9, p, 1);
	*(latitude + 10) = '\0';
	//Handle Longitude
	p = strchr(p, ',') + 1; //Go to lon
	strncpy(longitude, p, 10);
	p = strchr(p, ',') + 1; //Go to lon-dir
	strncpy(longitude + 10, p, 1);
	*(longitude + 11) = '\0';
	//Handle speed
	p = strchr(p, ',') + 1; //Go to speed
	uint8_t speedDigits = 0;
	speedDigits = strcspn(p, ",");
	strncpy(speed, p, speedDigits); //Copy to speed
	speed[speedDigits] = '\0';
	//Handle true course
	p = strchr(p, ',') + 1; //Go to true course
	uint8_t trueCourseDigits = 0;
	trueCourseDigits = strcspn(p, ",");
	strncpy(trueCourse, p, trueCourseDigits); //Copy to true course
	trueCourse[trueCourseDigits] = '\0';
}


#ifdef IS_TELEMETRY
void GPS::PostTelemetry(PythonScripts* scripts) {
    PythonHttp http;
    http.init();
    http.addData("heading", getTrueCourse());
    http.addData("latitude", getLatitude());
    http.addData("longitude", getLongitude());
    http.addData("speed", getSpeed());
    scripts->send("gps", http.getParameters());
    http.flush();
}
#endif

} /* namespace DataModules */
} /* namespace SolarGators */
