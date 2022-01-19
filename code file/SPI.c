/*
 * SPI.c
 *
 * Created: 03--Aug--2021 2:41:15 PM
 *  Author: Pc
 */ 


#include <avr/io.h>
#include "SPI.h"
#include "GPIO.h"

//SPI initialize for SD card
//clock rate: 125Khz
void spi_init(void)
{

	GPIO_InitParamType spi_temp;
	INIT_GPIO_PIN(spi_temp,PB,(1<<4),0xff,0x00);
	INIT_GPIO_PIN(spi_temp,PB,(1<<5),0xff,0x00);
	INIT_GPIO_PIN(spi_temp,PB,(1<<6),0x00,0xff); //miso pull up and dirction input
	INIT_GPIO_PIN(spi_temp,PB,(1<<7),0xff,0x00);
	SPCR = 0x52; //setup SPI: Master mode, MSB first, SCK phase low, SCK idle low
	SPSR = 0x00;
	
}

unsigned char SPI_transmit(unsigned char data)
{
	// Start transmission
	SPDR = data;

	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	data = SPDR;

	return(data);
}
unsigned char SPI_transfer(unsigned char data){
	unsigned char datax;
	// Wait for reception complete

	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	datax = SPDR;

	// Return data register
	return datax;
	
}
unsigned char SPI_receive(void)
{
	unsigned char data;
	// Wait for reception complete

	SPDR = 0xff;
	while(!(SPSR & (1<<SPIF)));
	data = SPDR;

	// Return data register
	return data;
}


