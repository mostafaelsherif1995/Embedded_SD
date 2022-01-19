///*
 //* led_cf.c
 //*
 //* Created: 31/08/2016 08:24:30 م
 //*  Author: mostafa
 //*/ 
#include "LED.h"
#include "LED_Cfg.h"
///////////port name(PORTA--PA      PORTB--PB        PORTC---PC          PORTD--PD       ,NUMBER OF PIN     //CONNECT METHOD IF ACTIVE HIGH OR LOW
const LED_ConfigType LED_ConfigParam[LED_MAX_NUM] =
{
	
		{
			PD,5,LED_ACTIVE_HIGH
		}
		,
		{
			PD,6,LED_ACTIVE_HIGH
		}
		,
		{
			PD,7,LED_ACTIVE_HIGH
		}
		
		
		
		
};

