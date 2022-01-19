/*
 * SPI.h
 *
 * Created: 03--Aug--2021 2:41:34 PM
 *  Author: Pc
 */ 


#ifndef SPI_H_
#define SPI_H_


#define SPI_SD             SPCR = 0x52
#define SPI_HIGH_SPEED     SPCR = 0x50; SPSR |= (1<<SPI2X)


void spi_init(void);
unsigned char SPI_transmit(unsigned char);
unsigned char SPI_receive(void);
unsigned char SPI_transfer(unsigned char);


#endif /* SPI_H_ */