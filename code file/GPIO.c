/*
 * GPIO.c
 *
 * Created: 04/07/2017 01:22:17 ص
 *  Author: LENOVO
 */ 

 
#include "GPIO.h"
#include <avr/io.h>
/*Is A macro to Set data into a register selected by a mask*/
/*Clear Bits Selected by mask*/
/*Should mask the data before ORing with the register*/
/*#define SET_REG_DATA(REG,REG_MASK,DATA)  REG &= ~(REG_MASK); \
REG |= (REG_MASK & DATA)*/

#define SET_REG_DATA(REG,REG_MASK,DATA) REG = ((REG & ~(REG_MASK)) | (DATA & REG_MASK))
/*Initialization function*/
void GPIO_Init(const GPIO_InitParamType *InitParamPtr)
{
	switch(InitParamPtr->PortName)
	{
		case PA:
		{
			/*Initialize Direction*/
			SET_REG_DATA(DDRA,InitParamPtr->PortMask,InitParamPtr->PortDirection);
			/*Init Pullup Resistor*/
			SET_REG_DATA(PORTA,InitParamPtr->PortMask,InitParamPtr->InternPullUpUsed);


		}
		break;
		case PB:
		{
			/*Initialize Direction*/
			SET_REG_DATA(DDRB,InitParamPtr->PortMask,InitParamPtr->PortDirection);
			/*Init Pullup Resistor*/
			SET_REG_DATA(PORTB,InitParamPtr->PortMask,InitParamPtr->InternPullUpUsed);
			
		}
		break;
		case PC:
		{
			/*Initialize Direction*/
			SET_REG_DATA(DDRC,InitParamPtr->PortMask,InitParamPtr->PortDirection);
			/*Init Pullup Resistor*/
			SET_REG_DATA(PORTC,InitParamPtr->PortMask,InitParamPtr->InternPullUpUsed);
		}
		break;
		case PD:
		{
			/*Initialize Direction*/
			SET_REG_DATA(DDRD,InitParamPtr->PortMask,InitParamPtr->PortDirection);
			/*Init Pullup Resistor*/
			SET_REG_DATA(PORTD,InitParamPtr->PortMask,InitParamPtr->InternPullUpUsed);
		}
		break;
		default:
		{
			/*No thing to be done, no register will be effected*/
		}
		break;
	}
	
}
void GPIO_Write(const GPIO_WriteParamType* WriteParamPtr)
{
	switch(WriteParamPtr->PortName)
	{
		case PA:
		{
			SET_REG_DATA(PORTA,WriteParamPtr->PortMask,WriteParamPtr->PortData);
		}
		break;
		case PB:
		{
			SET_REG_DATA(PORTB,WriteParamPtr->PortMask,WriteParamPtr->PortData);
		}
		break;
		case PC:
		{
			SET_REG_DATA(PORTC,WriteParamPtr->PortMask,WriteParamPtr->PortData);
		}
		break;
		case PD:
		{
			SET_REG_DATA(PORTD,WriteParamPtr->PortMask,WriteParamPtr->PortData);
		}
		break;
		default:
		{
			/*No thing to be done, no register will be effected*/
		}
		break;
	}
}
/*Read function*/
unsigned char GPIO_Read(const GPIO_ReadParamType *ReadParamPtr)
{
	unsigned char Dataread = 0;
	switch(ReadParamPtr->PortName)
	{
		case PA:
		{
			Dataread = PINA & (ReadParamPtr->PortMask);
		}
		break;
		case PB:
		{
			Dataread = PINB & (ReadParamPtr->PortMask);
		}
		break;
		case PC:
		{
			Dataread = PINC & (ReadParamPtr->PortMask);
		}
		break;
		case PD:
		{
			Dataread = PIND & (ReadParamPtr->PortMask);
		}
		break;
		default:
		{
			/*No thing to be done, no register will be effected*/
		}
		break;
	}
	return Dataread;
}