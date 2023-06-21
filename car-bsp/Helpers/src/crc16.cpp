/*
 * crc16.cpp
 *
 *  Created on: Jun 20, 2023
 *      Author: Raymond Bernardo
 */

#include "crc16.hpp"

namespace SolarGators::Helpers {

uint16_t crc16(uint8_t* data_p, uint32_t length) {
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    return crc;
}

}
