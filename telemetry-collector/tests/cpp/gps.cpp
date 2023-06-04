#include <map>
#include <iostream>
#include <sstream>
#include <csignal>
#include "GPS.hpp"

int main(int argc, char **argv) {
    SolarGators::DataModules::GPS GPS_Rx_0;
    uint8_t* testCoord = (uint8_t*)"$GPRMC,123519,A,4807.0380,N,01131.0000,E,022.4,084.4,230394,003.1,W*6A\0";

    GPS_Rx_0.FromByteArray(testCoord);
    uint8_t buff2[100];
    GPS_Rx_0.ToByteArray(buff2);

    std::cout << "lat: " << GPS_Rx_0.getLatitude() << std::endl;
    std::cout << "lo: " << GPS_Rx_0.getLongitude() << std::endl;
    std::cout << "speed: " << GPS_Rx_0.getSpeed() << std::endl;
    std::cout << "heading: " << GPS_Rx_0.getTrueCourse() << std::endl;
    std::cout << "data: " << buff2 << std::endl;

    if (strcmp(GPS_Rx_0.getLatitude(), "48.117300")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    if (strcmp(GPS_Rx_0.getLongitude(), "11.516667")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    if (strcmp(GPS_Rx_0.getSpeed(), "22.40")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    if (strcmp(GPS_Rx_0.getTrueCourse(), "84.40")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    if (strcmp((char*)testCoord, (char*)buff2)) {
        std::cout << "ToByteArray output is not the same as the input!" << std::endl;
        return -1;
    }

    return 0;
}
