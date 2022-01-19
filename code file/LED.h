///*
 //* led.h
 //*
 //* Created: 31/08/2016 08:24:44 م
 //*  Author: mostafa
 //*/ 
//
#ifndef LED_H_
#define LED_H_
#include "GPIO.h"
typedef enum {LED_ACTIVE_LOW = 0xff, LED_ACTIVE_HIGH = 0x00} LED_ConnectionType;
typedef struct
{
	GPIO_PortType LedPort;
	unsigned char LedPin;
	LED_ConnectionType LedConnection;
}LED_ConfigType;
typedef enum {LED_OFF=0x00,LED_ON=0xff} LED_DataType;
void LED_Init(void);
unsigned char LED_SetData(unsigned long int LedID,LED_DataType LedData);

#endif /* LED_H_ */