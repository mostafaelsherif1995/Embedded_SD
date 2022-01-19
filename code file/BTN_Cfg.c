/*
 * BTN_Cfg.c
 *
 * Created: 8/30/2016 9:24:35 PM
 *  Author: mostafa
 */ 
#include "BTN.h"
#include "BTN_Cfg.h"
///////////port name(PORTA--PA      PORTB--PB        PORTC---PC          PORTD--PD       ,NUMBER OF PIN     //CONNECT METHOD IF ACTIVE HIGH OR LOW
const BTN_ConfigType BTN_ConfigParam [BTN_MAX_NUM] = 
{
	{
	PA,4,BTN_ACTIVE_HIGH
	}	
	
	
};
