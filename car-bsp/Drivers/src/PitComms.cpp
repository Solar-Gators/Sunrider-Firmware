/*
 * PitComms.cpp
 *
 *  Created on: Feb 3, 2022
 *      Author: John Carr
 */

#include "PitComms.hpp"
#include "crc16.hpp"

namespace SolarGators {
namespace Drivers {

PitComms::PitComms(SolarGators::Drivers::Radio* radio):radio_(radio)
{
  radio_->Init();
  uart_rx_event_ = osEventFlagsNew(NULL);
  if (uart_rx_event_ == NULL)
  {
      Error_Handler();
  }

  rx_task_handle_ = osThreadNew((osThreadFunc_t)&PitComms::HandleReceive, this, &rx_task_attributes_);
  if (rx_task_handle_ == NULL)
  {
      Error_Handler();
  }
}

void PitComms::HandleReceive()
{
  while(1)
  {
    char gps_sentence[MAX_SENTENCE_LENGTH];
    uint32_t flags = osEventFlagsWait(uart_rx_event_, (PING_FLAG | PONG_FLAG), osFlagsWaitAny, osWaitForever);
    // These string copies need to happen fast
    if(flags & PING_FLAG)
    {
      // Read from the ping buffer
      strcpy(gps_sentence, reinterpret_cast<char*>(ping_));
    }
    else
    {
      // Read from the pong buffer
      strcpy(gps_sentence, reinterpret_cast<char*>(pong_));
    }
  }
}

PitComms::~PitComms()
{
  // TODO Auto-generated destructor stub
}

void PitComms::startReception()
{
  // Temporarily disable uart
  ATOMIC_CLEAR_BIT(uart_instance_->Instance->CR1, USART_CR1_UE);
  // Add character to match
  ATOMIC_SET_BIT(uart_instance_->Instance->CR2, (DELIM_CHAR << USART_CR2_ADD_Pos));
  // Enable the interrupt for character match
  __HAL_UART_ENABLE_IT(uart_instance_, UART_IT_CM);
  // Enable UART
  ATOMIC_SET_BIT(uart_instance_->Instance->CR1, USART_CR1_UE);
  // Start the first reception
  HAL_UART_Receive_DMA(uart_instance_, ping_, sizeof(ping_));
}

void PitComms::rxCpltCallback()
{
  DMA_HandleTypeDef *hdma = uart_instance_->hdmarx;
  uint32_t data_transferred = MAX_SENTENCE_LENGTH;
  // If this IRQ is caused by a character match
  if(READ_BIT(uart_instance_->Instance->ISR, USART_ISR_CMF))
  {
    // Change the DMA reg
    // Stop DMA
    hdma->Instance->CCR &= ~DMA_CCR_EN;
    // Get the amount of bytes transferred
    data_transferred = data_transferred - hdma->Instance->CNDTR;
    // Set the Init direction so that we a sure we are DMAing to memory
    hdma->Init.Direction = DMA_PERIPH_TO_MEMORY;
    if(buffer_is_pong_)
    {
      // End the sentence with null termination so we can strcpy
      pong_[data_transferred] = '\0';
      updateDmaEngine(hdma, (uint32_t)&(uart_instance_->Instance->RDR), (uint32_t)ping_, sizeof(ping_));
      buffer_is_pong_ = false;
      osEventFlagsSet(uart_rx_event_, PONG_FLAG);
    }
    else
    {
      // End the sentence with null termination so we can strcpy
      ping_[data_transferred] = '\0';
      updateDmaEngine(hdma, (uint32_t)&(uart_instance_->Instance->RDR), (uint32_t)pong_, sizeof(pong_));
      buffer_is_pong_ = true;
      osEventFlagsSet(uart_rx_event_, PING_FLAG);
    }
    /* Enable the TC complete interrupt and error interrupt */
    hdma->Instance->CCR |= (DMA_IT_TC | DMA_IT_TE);
    /* Enable the DMA Peripheral */
    hdma->Instance->CCR |= DMA_CCR_EN;
  }

  // Clear Interrupts
  uint32_t toClear = uart_instance_->Instance->ISR;
  uart_instance_->Instance->ICR |= toClear;
}

void PitComms::updateDmaEngine(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  /* Clear all flags */
  hdma->DmaBaseAddress->IFCR  = (DMA_FLAG_GL1 << hdma->ChannelIndex);

  /* Configure DMA Channel data length */
  hdma->Instance->CNDTR = DataLength;

  /* Memory to Peripheral */
  if ((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    /* Configure DMA Channel destination address */
    hdma->Instance->CPAR = DstAddress;

    /* Configure DMA Channel source address */
    hdma->Instance->CMAR = SrcAddress;
  }
  /* Peripheral to Memory */
  else
  {
    /* Configure DMA Channel source address */
    hdma->Instance->CPAR = SrcAddress;

    /* Configure DMA Channel destination address */
    hdma->Instance->CMAR = DstAddress;
  }
}

void PitComms::SendDataModule(SolarGators::DataModules::DataModule& data_module)
{
  // Start Condition
  osMutexAcquire(data_module.mutex_id_, osWaitForever);
  radio_->SendByte(START_CHAR);
  // Only Sending one Datamodule
  radio_->SendByte(EscapeData((data_module.can_id_ & 0xFF000000) >> 24));
  radio_->SendByte(EscapeData((data_module.can_id_ & 0x00FF0000) >> 16));
  radio_->SendByte(EscapeData((data_module.can_id_ & 0x0000FF00) >> 8));
  radio_->SendByte(EscapeData(data_module.can_id_ & 0x000000FF));
  radio_->SendByte(EscapeData(data_module.instance_id_));
  radio_->SendByte(EscapeData(data_module.size_));
  // Temporary buffer
  uint8_t buff[100];
  data_module.ToByteArray(buff);
  // Calculate and send CRC
  uint16_t crc = SolarGators::Helpers::crc16(buff, data_module.size_);
  uint8_t lo_crc = crc & 0xFF;
  uint8_t hi_crc = (crc >> 8) & 0xFF;
  EscapeData(hi_crc);
  radio_->SendByte(hi_crc);
  EscapeData(lo_crc);
  radio_->SendByte(lo_crc);
  // Send Buffer
  for (uint16_t i = 0; i < data_module.size_; ++i) {
    EscapeData(buff[i]);
    radio_->SendByte(buff[i]);
  }
  // End condition
  radio_->SendByte(END_CHAR);
  osMutexRelease(data_module.mutex_id_);
}

inline uint8_t PitComms::EscapeData(uint8_t data)
{
  if(data == START_CHAR || data == END_CHAR || data == ESC_CHAR)
  {
    radio_->SendByte(ESC_CHAR);
  }
  return data;
}

} /* namespace Drivers */
} /* namespace SolarGators */
