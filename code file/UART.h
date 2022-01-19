/*
 * UART.h
 *
 * Created: 03--Aug--2021 2:48:36 PM
 *  Author: Pc
 */ 


#ifndef UART_H_
#define UART_H_

#define CHAR 0
#define INT  1
#define LONG 2


#define TX_NEWLINE {transmitByte(0x0d); transmitByte(0x0a);}

void uart0_init(void);  //1Mhz for 8Mhz for clock
void Serial_init(unsigned long int baud_rate,unsigned long int clock );
unsigned char receiveByte(void);
void transmitByte(unsigned char);
void transmitString_F(char*);
void transmitString(unsigned char*);
void transmitNum(double number);
void transmitHex( unsigned char dataType, unsigned long data );
void double_to_str(double num,char str[]);


#endif /* UART_H_ */