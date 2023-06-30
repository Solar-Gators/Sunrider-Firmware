/*
 * PitComms.hpp
 *
 *  Created on: Feb 3, 2022
 *      Author: John Carr
 *  Description: This is the abstraction class for communicating with the pit via a serial device.
 */

#ifndef SOLARGATORSBSP_STM_DRIVERS_PITCOMMS_HPP_
#define SOLARGATORSBSP_STM_DRIVERS_PITCOMMS_HPP_

#include "etl/queue.h"
#include "etl/iterator.h"

#include "DataModule.hpp"
#include "Radio.hpp"

namespace SolarGators {
namespace Drivers {

class PitComms {
private:
  static constexpr uint8_t MAX_PACKETS = 10;
  static constexpr uint8_t START_CHAR = 0xFF;
  static constexpr uint8_t ESC_CHAR = 0x2F;
  static constexpr uint8_t END_CHAR = 0x3F;
  SolarGators::Drivers::Radio* radio_;
public:
  PitComms(SolarGators::Drivers::Radio* radio);
  virtual ~PitComms();
  void Init();
  void SendDataModule(SolarGators::DataModules::DataModule& data_module);
  void rxCpltCallback();
  void startReception();
  void HandleReceive();
private:
  void updateDmaEngine(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
  static constexpr uint32_t MAX_SENTENCE_LENGTH = 50;
  static constexpr uint8_t DELIM_CHAR = '\n';

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
    .name = "Pit Rx Handler",
    .cb_mem = &rx_task_control_block_,
    .cb_size = sizeof(rx_task_control_block_),
    .stack_mem = &rx_task_buffer_[0],
    .stack_size = sizeof(rx_task_buffer_),
    // This thread is deferred ISR work and since we are using DMA this task needs to be run
    // really quickly to clear the DMA buffer before it switches back.
    .priority = osPriorityISR,
  };

  uint8_t EscapeData(uint8_t data);
};

} /* namespace Drivers */
} /* namespace SolarGators */

#endif /* SOLARGATORSBSP_STM_DRIVERS_PITCOMMS_HPP_ */
