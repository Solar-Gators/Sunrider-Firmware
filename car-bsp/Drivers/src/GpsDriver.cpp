/*
 * Gps.cpp
 *
 *  Created on: June 16, 2023
 *      Author: Raymond Bernardo, Noah Zinn, John Carr
 */

//Include Files
#include "GpsDriver.hpp"
#include "string.h"

namespace SolarGators {
namespace Drivers {

Gps::Gps(UART_HandleTypeDef* uart_instance, SolarGators::DataModules::GPS* gps_data)
{
  gps_data_ = gps_data;
  uart_instance_ = uart_instance;

  uart_rx_event_ = osEventFlagsNew(NULL);
  if (uart_rx_event_ == NULL)
  {
      Error_Handler();
  }

  rx_task_handle_ = osThreadNew((osThreadFunc_t)&Gps::HandleReceive, this, &rx_task_attributes_);
  if (rx_task_handle_ == NULL)
  {
      Error_Handler();
  }
}

Gps::~Gps()
{ }

void Gps::HandleReceive()
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
    osMutexAcquire(gps_data_->mutex_id_, osWaitForever);
    gps_data_->FromByteArray(reinterpret_cast<uint8_t*>(gps_sentence));
    osMutexRelease(gps_data_->mutex_id_);      
  }
}

void Gps::rxCpltCallback()
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

void Gps::startReception()
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

void Gps::updateDmaEngine(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
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

} /* namespace Drivers */
} /* namespace SolarGators */
