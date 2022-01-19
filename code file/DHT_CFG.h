/*
 * DHT_CFG.h
 *
 * Created: 03--Aug--2021 3:06:07 PM
 *  Author: Pc
 */ 


#ifndef DHT_CFG_H_
#define DHT_CFG_H_

#include "GPIO.h"
#include "DHT.h"

#define DHT_TYPE            DHT_DHT22             //or DHT_DHT22  / DHT_DHT11  /DHT_DHT21
//
//#define DHT_PIN_NUMBER      PD2                   //0/1/2/3/4/5/6/7
////#define DHT_PORT            PD                   // PA/PB/PC/PD
///////////////////////////////////
#define DHT_DDR               DDRD    //( DDRA, DDRB, DDRC, DDRD)
#define DHT_PORT              PORTD  //( PORTA, PORTB, PORTC, PORTD)
#define DHT_PIN               PIND  //( PINA, PINB, PINC, PIND)
#define DHT_INPUTPIN          PD2   //PA,PC,PB,PD   +NUMBER FROM 0:7

#endif /* DHT_CFG_H_ */