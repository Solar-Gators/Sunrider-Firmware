//File Name: gps-driver.h
//Description: This contains the implementation of the gps driver for receiving gps data through uart and parsing it

//Header Guards
#ifndef GPS_DRIVER_H_
#define GPS_DRIVER_H_

//Include Files
#include "main.h"
//Public Constants
//Public Structs
//Public Variables

//Public Function Prototypes
void GPS_init(USART_TypeDef* uart_instance);

void GPS_startReception(void);

char* GPS_RxCpltCallback(bool* success);

#endif //End Header Guard