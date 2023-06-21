/*
 * crc16.hpp
 *
 *  Created on: Jun 20, 2023
 *      Author: Raymond Bernardo
 */

#ifndef SOLARGATORSBSP_HELPERS_CRC16
#define SOLARGATORSBSP_HELPERS_CRC16

#include <cstdint>

namespace SolarGators::Helpers {

uint16_t crc16(uint8_t* data_p, uint32_t length);

}

#endif /* SOLARGATORSBSP_HELPERS_CRC16 */
