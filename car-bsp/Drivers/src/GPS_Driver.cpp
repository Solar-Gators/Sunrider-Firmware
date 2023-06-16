//File Name: gps-driver.cpp
//Description: This implements the definition of the gps driver

//Include Files
#include "GPS_Driver.hpp"
#include <string.h>

//Private Constants
#define RX_MESSAGE_MAX_SIZE 100
//Private Variables
//static UART_HandleTypeDef* uartInstance;
static char rxMessage[RX_MESSAGE_MAX_SIZE];
static uint8_t messageStart = 0;
static uint16_t rxMessageIndex = 0;
//Public Constants

//Public Variables

//Private Function Prototypes
//Public Function Prototypes

//Private Function Definitions

//Public Function Definitions
void GPS_init(UART_HandleTypeDef* uart_instance)
{
	//uartInstance = uart_instance;
	//GPRMC only
	uint8_t options[] = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
	HAL_UART_Transmit(uart_instance, options, sizeof(options), 0);
	/*
	for(uint16_t i = 0; i < sizeof(options); i++)
	{
		while(!(uartInstance->ISR & USART_ISR_TXE));
		uartInstance->TDR = options[i];
	}
	*/
	memset(rxMessage,0, RX_MESSAGE_MAX_SIZE);
}

void GPS_startReception(UART_HandleTypeDef* uart_instance)
{
	//Enable interrupts
	//uartInstance->CR1 |= USART_CR1_RXNEIE;
}

char* GPS_RxCpltCallback(bool* success, char rxChar)
{
	*success = false;
	if(rxChar == '$')
	{
		messageStart = 1;
		rxMessageIndex = 0;
	}
	if(messageStart)
	{
		rxMessage[rxMessageIndex++] = rxChar;
	}
	if(rxChar == 0x0A)
	{
		rxMessage[rxMessageIndex] = '\0';
		messageStart = 0;
		*success = true;
	}
	return rxMessage;
}
