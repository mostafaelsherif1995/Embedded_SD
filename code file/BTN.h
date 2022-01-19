/*
 * BTN.h
 *
 * Created: 8/30/2016 9:23:52 PM
 *  Author: mostafa
 */ 


#ifndef BTN_H_
#define BTN_H_
#include "GPIO.h"
typedef enum {BTN_ACTIVE_LOW = 0xff, BTN_ACTIVE_HIGH = 0x00} BTN_ConnectionType;
typedef struct
{
	GPIO_PortType BtnPort;
	unsigned char BtnPin;
	BTN_ConnectionType BtnConnection;
	}BTN_ConfigType;
typedef enum {BTN_OFF=0, BTN_ON, BTN_JUST_PRESSED, BTN_JUST_RELEASED} BTN_StateType;
void BTN_Init(void);
BTN_StateType BTN_GetState(unsigned long int BtnId);
BTN_StateType SwitchBTN_GetState(unsigned long int BtnId);



unsigned char BTN_State(unsigned long int BtnId,BTN_StateType* BtnStatePtr);
unsigned char BTN_Get(unsigned long int BtnId);
#endif /* BTN_H_ */