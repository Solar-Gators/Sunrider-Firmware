#include <map>
#include <iostream>
#include <sstream>
#include <cstring>
#include <csignal>
#include "Mppt.hpp"

int smokeTest() {
    uint8_t data[8];
    uint8_t result[8];


    std::cout << "MPPT_RX_0 Test" << std::endl;
    SolarGators::DataModules::Mpptx0 MPPT_Rx_0(0);
    float inputVoltage = 1.1;
    float inputCurrent = 2.2;
    memcpy(&data[0], &inputVoltage, sizeof(inputVoltage));
    memcpy(&data[4], &inputCurrent, sizeof(inputCurrent));
    MPPT_Rx_0.FromByteArray(data);
    MPPT_Rx_0.ToByteArray(result);

    std::cout << "input volt: " << MPPT_Rx_0.getInputVoltage() << std::endl;
    std::cout << "input curr: " << MPPT_Rx_0.getInputCurrent() << std::endl;
    for (int i = 0; i < 8; i++) {
        printf("%02x ", data[i]);
    }
    std::cout << std::endl;
    for (int i = 0; i < 8; i++) {
        printf("%02x ", result[i]);
    }
    std::cout << std::endl;


    if (MPPT_Rx_0.getInputVoltage() != inputVoltage) {
        std::cout << "Input voltage is wrong!" << std::endl;
        return -1;
    }

    if (MPPT_Rx_0.getInputCurrent() != inputCurrent) {
        std::cout << "Input current is wrong!" << std::endl;
        return -1;
    }

    if (memcmp(data, result, 8)) {
        std::cout << "ToByteArray output is not the same as the input!" << std::endl;
        return -1;
    }

    std::cout << std::endl << "MPPT_RX_1 Test" << std::endl;
    SolarGators::DataModules::Mpptx1 MPPT_Rx_1(0);
    float outputVoltage = 1.1;
    float outputCurrent = 2.2;
    memcpy(&data[0], &outputVoltage, sizeof(outputVoltage));
    memcpy(&data[4], &outputCurrent, sizeof(outputCurrent));
    MPPT_Rx_1.FromByteArray(data);
    MPPT_Rx_1.ToByteArray(result);

    std::cout << "output volt: " << MPPT_Rx_1.getOutputVoltage() << std::endl;
    std::cout << "output curr: " << MPPT_Rx_1.getOutputCurrent() << std::endl;
    for (int i = 0; i < 8; i++) {
        printf("%02x ", data[i]);
    }
    std::cout << std::endl;
    for (int i = 0; i < 8; i++) {
        printf("%02x ", result[i]);
    }
    std::cout << std::endl;

    if (MPPT_Rx_1.getOutputVoltage() != outputVoltage) {
        std::cout << "Input voltage is wrong!" << std::endl;
        return -1;
    }

    if (MPPT_Rx_1.getOutputCurrent() != outputCurrent) {
        std::cout << "Input current is wrong!" << std::endl;
        return -1;
    }

    if (memcmp(data, result, 8)) {
        std::cout << "ToByteArray output is not the same as the input!" << std::endl;
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
}
