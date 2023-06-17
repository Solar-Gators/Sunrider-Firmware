/*
 * GPS.hpp
 *
 *  Created on: Apr 21, 2022
 *      Author: John Carr
 */

#ifndef SOLARGATORSBSP_DRIVERS_INC_GPS_HPP_
#define SOLARGATORSBSP_DRIVERS_INC_GPS_HPP_

#include "main.h"
#include <cmsis_os.h>

namespace SolarGators {
namespace Drivers {

class Gps {
public:
  Gps(UART_HandleTypeDef* uart_instance);
  ~Gps();
  void HandleReceive();
  void rxCpltCallback();
  void startReception();

private:
  void updateDmaEngine(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
  static constexpr uint32_t MAX_SENTENCE_LENGTH = 100;
  static constexpr uint8_t DELIM_CHAR = 'z';

  static constexpr uint8_t PING_FLAG = 1 << 0;
  static constexpr uint8_t PONG_FLAG = 1 << 1;

  uint8_t ping_[MAX_SENTENCE_LENGTH];
  uint8_t pong_[MAX_SENTENCE_LENGTH];
  bool buffer_is_pong_;
  UART_HandleTypeDef* uart_instance_;
  osEventFlagsId_t uart_rx_event_;                 // Rx Data Ready for processing
  osThreadId_t rx_task_handle_;                    // Rx Task Handle
  uint32_t rx_task_buffer_[ 128 ];                 // Rx Task Buffer
  StaticTask_t rx_task_control_block_;             // Rx Task Control Block
  const osThreadAttr_t rx_task_attributes_ =       // Rx Task Attributes
  {
    .name = "UART Rx Handler",
    .cb_mem = &rx_task_control_block_,
    .cb_size = sizeof(rx_task_control_block_),
    .stack_mem = &rx_task_buffer_[0],
    .stack_size = sizeof(rx_task_buffer_),
    .priority = osPriorityRealtime,
  };
};

} /* namespace Drivers */
} /* namespace SolarGators */

#endif /* SOLARGATORSBSP_DRIVERS_INC_GPS_HPP_ */
