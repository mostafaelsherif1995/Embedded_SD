
///*
//* LED.c
//*
//* Created: 8/30/2016 7:30:14 PM
//*  Author: mostafa
//*/
#include "LED.h"
#include "GPIO.h"
#include "LED_Cfg.h"
extern const LED_ConfigType LED_ConfigParam[LED_MAX_NUM];
void LED_Init(void)
{
	unsigned long int LoopIndex;
	GPIO_InitParamType LedParam;
	GPIO_WriteParamType LedWrtParam;
	LedParam.PortDirection = 0xff;
	LedParam.InternPullUpUsed = 0x00;
	for(LoopIndex = 0; LoopIndex < LED_MAX_NUM; LoopIndex ++)
	{
		/*Init LEDs Direcion*/
		LedParam.PortMask = 1 << LED_ConfigParam[LoopIndex].LedPin;
		LedParam.PortName = LED_ConfigParam[LoopIndex].LedPort;
		GPIO_Init(&LedParam);
		/*Turn off all leds*/
		LedWrtParam.PortData = LED_ConfigParam[LoopIndex].LedConnection;
		LedWrtParam.PortMask = 	1 << LED_ConfigParam[LoopIndex].LedPin;
		LedWrtParam.PortName = 	LED_ConfigParam[LoopIndex].LedPort;
		GPIO_Write(&LedWrtParam);
	}
	
}
unsigned char LED_SetData(unsigned long int LedID,LED_DataType LedData)
{
	unsigned char FunState ;
	GPIO_WriteParamType LedParam;
	if(LedID < LED_MAX_NUM)
	{
		LedParam.PortName = LED_ConfigParam[LedID].LedPort;
		LedParam.PortMask = 1 << LED_ConfigParam[LedID].LedPin;
		LedParam.PortData = LedData ^ LED_ConfigParam[LedID].LedConnection;

		GPIO_Write(&LedParam);
		FunState = 1;
	}
	else
	{
		FunState = 0;
	}
	return FunState;
}