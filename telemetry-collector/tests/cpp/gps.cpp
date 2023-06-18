#include <map>
#include <iostream>
#include <sstream>
#include <csignal>
#include "GPS.hpp"

int smokeTest() {
    // Test a valid GPRMC message
    SolarGators::DataModules::GPS GPS_Rx_0;
    uint8_t* testCoord = (uint8_t*)"$GPRMC,123519,A,4807.0380,N,01131.0000,E,022.4,084.4,230394,003.1,W*6A";

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

int invalidTest() {
    // Test an invalid GPRMC message
    SolarGators::DataModules::GPS GPS_Rx_0;
    uint8_t* testCoord = (uint8_t*)"$GPRMC,,V,,,,,,,,,N*53";

    GPS_Rx_0.FromByteArray(testCoord);
    uint8_t buff2[100];
    GPS_Rx_0.ToByteArray(buff2);

    std::cout << "lat: " << GPS_Rx_0.getLatitude() << std::endl;
    std::cout << "lo: " << GPS_Rx_0.getLongitude() << std::endl;
    std::cout << "speed: " << GPS_Rx_0.getSpeed() << std::endl;
    std::cout << "heading: " << GPS_Rx_0.getTrueCourse() << std::endl;
    std::cout << "data: " << buff2 << std::endl;

    return 0;
}


int emptyModule() {
    // Test an empty module
    SolarGators::DataModules::GPS GPS_Rx_0;

    uint8_t buff2[100];
    GPS_Rx_0.ToByteArray(buff2);

    std::cout << "lat: " << GPS_Rx_0.getLatitude() << std::endl;
    std::cout << "lo: " << GPS_Rx_0.getLongitude() << std::endl;
    std::cout << "speed: " << GPS_Rx_0.getSpeed() << std::endl;
    std::cout << "heading: " << GPS_Rx_0.getTrueCourse() << std::endl;
    std::cout << "data: " << buff2 << std::endl;

    if (strcmp(GPS_Rx_0.getLatitude(), "0")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    if (strcmp(GPS_Rx_0.getLongitude(), "0")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    if (strcmp(GPS_Rx_0.getSpeed(), "0")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    if (strcmp(GPS_Rx_0.getTrueCourse(), "0")) {
        std::cout << "Lat is incorrect!" << std::endl;
        return -1;
    }

    return 0;
}


int main(int argc, char **argv) {

    std::cout << "Running Smoke Test.." << std::endl;
    if (smokeTest() == -1) {
        std::cout << "Failed!" << std::endl;
        return -1;
    }
    std::cout << "Passed." << std::endl << std::endl;

    std::cout << "Running Empty Test.." << std::endl;
    if (invalidTest() == -1) {
        std::cout << "Failed!" << std::endl;
        return -1;
    }
    std::cout << "Passed." << std::endl << std::endl;

    std::cout << "Running Empty Module Test.." << std::endl;
    if (emptyModule() == -1) {
        std::cout << "Failed!" << std::endl;
        return -1;
    }
    std::cout << "Passed." << std::endl << std::endl;

    return 0;
}
