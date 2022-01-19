
/*
 * standio.h
 *
 * Created: 20/10/2016 08:32:27 م
 *  Author: LENOVO
 */ 

/*
 * standio.h
 *
 * Created: 14/10/2016 06:43:06 م
 *  Author: LENOVO
 */ 
#ifndef STANDIO_H_
#define STANDIO_H_
#include <avr/io.h>


typedef unsigned  char   u8;
typedef unsigned short int   u16;
typedef unsigned long int   u32;
typedef enum{false=0,true=1}boolean;

//gpio_registers:::
#define   DDRA        (*((volatile u32  *)0x3A))
#define   PORTA      (*((volatile u32  *)0x3B))
#define   PINA       (*((volatile u32  *)0x39))

#define   DDRB        (*((volatile u32  *)0x37))
#define   PORTB      (*((volatile u32  *)0x38))
#define   PINB       (*((volatile u32  *)0x36))

#define   DDRC        (*((volatile u32 *)0x34))
#define   PORTC       (*((volatile u32 *)0x35))
#define   PINC        (*((volatile u32 *)0x33))

#define  DDRD       (*((volatile u32  *)0x31))
#define  PORTD      (*((volatile u32  *)0x32))
#define  PIND       (*((volatile u32  *)0x30))
//::::::::::::::::::::::::::::::::::::
//:::::::::::::::I2C::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#define TWCR              (*((volatile u32 *)0x56))
#define TWINT      7
#define  TWEA      6
#define  TWSTA     5
#define TWSTO       4
#define  TWWC       3
#define   TWEN      2
#define   TWIE       0


#define   TWSR            (*((volatile u32 *)0x21))
#define    TWS4     7
#define    TWS3     6
#define    TWS2     5
#define    TWS1     4
#define    TWS0     3
#define    TWPS1    1
#define     TWPS0   0

#define    TWDR              (*((volatile u32 *)0x23))

#define  TWBR                (*((volatile u32 *)0x20))
#define   TWAR               (*((volatile u32 *)0x22))
#define  TWGCE      0         //enable call general 
//:::::::::::::::::::::::::::::::::uart_register::::::::::::::::::::::::::::::
#define  UBRRL   (*((volatile u32 *)0x29))
#define  UBRRH    (*((volatile u32 *)0x40))
#define  UCSRB  (*((volatile u32 *)0x2A))
#define  UCSRC  (*((volatile u32 *)0x40))
#define  UCSRA   (*((volatile u32 *)0x2B))  //
#define  UDR    (*((volatile u32 *)0x2C))


#define  RXEN    4
#define  TXEN    3
#define   UDRE    5
#define   RXC     7
#define   TXC     6
#define   UCSZ0   1
#define   UCSZ1   2
#define  USBS     3
//::::::::::::::::::::::::::::::INTERNAL_EEPROM_REGISTERS:::::::::::::::::::::::;;;
#define  EECR    (*((volatile u32 *)0x3C))
#define  EEARL    (*((volatile u32 *)0x3E))
#define  EEARH   (*((volatile u32 *)0x3F))
#define  EEDR    (*((volatile u32 *)0x3D))
#define  EERIE   3   // interupt enable
#define  EEMWE   2   //FLAg that cleared by hard and set by soft
#define  EEWE    1   //write enable
#define  EERE    0   //read enable




#endif