//*******************************************************************
//        **** Interview task ****
//*******************************************************************
//Controller		: ATmega32 (Clock: 8 Mhz-internal)
//Compiler			: AVR-GCC (winAVR with AVRStudio-4)
//Author			: mostafa khalil ahmed
//	email:		  	: mostafa.elsherif1995@gamail.com
//Date				: 1 Aug 2021
//*******************************************************************


#define F_CPU 8000000UL	
#define UART_BaudRate   1000000   //1Mhz	
#define I2C_Seed         100000   //100khz

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SPI.h"
#include "SD_CARD.h"
#include "UART.h"
#include "RTC.h"
#include "I2C.h"
#include "FAT32.h"
#include "DHT.h"
#include "BTN.h"
#include "LED.h"


#define Error_led             0    //red led
#define InterfaceMode_led     1  //green led
#define  workingMode_led      2   //blue led
#define Mode_BTN              0

#define  Interface_Mode	1				//interval defined here is approximate only, as the overhead delays
#define  Working_Mode	0					//are not added, error is more for smaller values (i.e. <100ms)


//call this routine to initialize all peripherals
void init_devices(void);
void Blink_Error(void);
unsigned char Mode_working(void);
unsigned char Mode_Waitworking(void);
extern volatile int delay_num;


//*************************** MAIN *******************************//
int main(void)
{
	
	//all verialble
	unsigned char option, error, buf_count=0,index=0,i=0,j=0, data, channel,temperature_str[8],humidity_str[8];
	unsigned char fileName[13],mode;

	float temperature=0,humidity=0;
	//Variables

	_delay_ms(100);  //delay for VCC stabilization

	init_devices();
	
	transmitString_F (PSTR("\n\r\n********Interface Mode***"));
	transmitString_F (PSTR("\n\r         Interview Task SD Card      "));
	transmitString_F (PSTR("\n\r****************************************************\n\r"));
	cardType = 0;
	
	for (i=0; i<10; i++)
	{
		error = SdCard_Init();
		if(!error) break;
	}

	if(error)
	{
		if(error == 1) transmitString_F(PSTR("SD card not detected.."));
		if(error == 2) transmitString_F(PSTR("Card Initialization failed.."));
		Blink_Error();

	}

	switch (cardType)
	{
		case 1:transmitString_F(PSTR("Standard Capacity Card (Ver 1.x) Detected!"));
		break;
		case 2:transmitString_F(PSTR("High Capacity Card Detected!"));
		break;
		case 3:transmitString_F(PSTR("Standard Capacity Card (Ver 2.x) Detected!"));
		break;
		default:transmitString_F(PSTR("Unknown SD Card Detected!"));
		break;
	}

	error = getBootSectorData (); //read boot sector and keep necessary data in global variables
	if(error)
	{
		transmitString_F (PSTR("\n\rFAT32 not found!"));  //FAT32 incompatible drive
		Blink_Error();
	}


	_delay_ms(1);   	//some delay for settling new spi speed



	while(1){
		mode=Mode_working();
		if(mode==Interface_Mode){
			
			transmitString_F(PSTR("\n\r\n\r> 0 : Exit the Menu"));
			transmitString_F(PSTR("\n\r> 1 : Display current Date/Time"));
			transmitString_F(PSTR("\n\r> 2 : Update Date"));
			transmitString_F(PSTR("\n\r> 3 : Update Time"));
			transmitString_F(PSTR("\n\r> 4 : Get file list"));
			transmitString_F(PSTR("\n\r> 5 : Read File"));
			transmitString_F(PSTR("\n\r> 6 : Delete File"));
			transmitString_F(PSTR("\n\r> 7 : Create File"));
			transmitString_F(PSTR("\n\r> 8 : Get Sensor Data"));

			transmitString_F(PSTR("\n\r\n\r> Enter the option:"));
			option = receiveByte();
			transmitByte(option);


			switch (option)
			{
				case '0':
				transmitString_F(PSTR("\n\rNormal operation started.."));
				TX_NEWLINE;TX_NEWLINE;TX_NEWLINE;TX_NEWLINE;TX_NEWLINE;TX_NEWLINE;
				
				transmitString_F(PSTR("\n\r click Button mode please and wait to click .."));
				while(1){
					if(Mode_Waitworking()!=Interface_Mode) break;
				}
				break;

				case '1':RTC_displayDate();
				RTC_displayTime();
				break;

				case '2':RTC_updateDate();
				break;

				case '3':RTC_updateTime();
				break;

				case '4':TX_NEWLINE;
				findFiles(GET_LIST,0);
				break;

				case '5':
				
				case '6':
				case '7': transmitString_F(PSTR("\n\rEnter file name(with .txt or other): "));
				for(i=0; i<13; i++)
				fileName[i] = 0x00;   //clearing any previously stored file name
				i=0;
				while(1)
				{
					data = receiveByte();
					if(data == 0x0d) break;  //'ENTER' key pressed
					if(data == 0x08)	//'Back Space' key pressed
					{
						if(i != 0)
						{
							transmitByte(data);
							transmitByte(' ');
							transmitByte(data);
							i--;
						}
						continue;
					}
					if(data <0x20 || data > 0x7e) continue;  //check for valid English text character
					transmitByte(data);
					fileName[i++] = data;
				if(i==13){transmitString_F(PSTR(" file name too long..")); break;}
			}
			if(i>12) break;
			
			TX_NEWLINE;
			if(option == '5')
			{
				error = readFile( READ, fileName);
				if(error == 1) transmitString_F(PSTR("File does not exist.."));
			}
			if(option == '6') deleteFile(fileName);
			if(option == '7'){
				writeFile(fileName);
				
			}
			break;
			case '8':
			TX_NEWLINE;
			transmitString_F(PSTR("\n\r\n\r data of dht22!:"));
			
		
			dht_gettemperaturehumidity(&temperature,&humidity);
				
				transmitString_F(PSTR("temperature:"));
				transmitNum(temperature);
				TX_NEWLINE;
				transmitString_F(PSTR("humidity:"));
				transmitNum(humidity);
				_delay_ms(100);
		
				
			break;

			default:transmitString_F(PSTR("\n\r\n\r Invalid option!\n\r"));
		}
		
	}
	
	else {
		_delay_ms(40);	   		//key debounce delay
		error = RTC_getDate();
		j=0;i=0;
		fileName[i++]='M';
		fileName[i++]='O';
		fileName[i++]='S';
		fileName[i++]='T';
		for(i=0; i<4; i++)
		{
			fileName[i+4] = date[j++];
			if(j==2 ) j++;	//excluding the '/' character from date in the fileName
		}

		fileName[8] = '.';
		fileName[9] = 'C';
		fileName[10] = 'S';
		fileName[11] = 'V';

		error = RTC_getTime();
		buf_count=0;

		for(index=0; index<10; index++) dataString[buf_count++] = date[index];
		dataString[buf_count++] = ',';

		for(index=0;index <8; index++)  dataString[buf_count++] = time[index];
		dataString[buf_count++]= ',';
		
		
		dht_gettemperaturehumidity(&temperature,&humidity);
		
		double_to_str(temperature,temperature_str);
		
		for(index=0;index<5; index++) dataString[buf_count++] = temperature_str[index];
		
		dataString[buf_count++] = ',';
		double_to_str(humidity,humidity_str);
		for(index=0; index<5; index++) dataString[buf_count++] = humidity_str[index];
		dataString[buf_count++] = ',';
		dataString[buf_count++] = '\r';	dataString[buf_count] = '\n';    //new line and new row of data
		
		error = writeFile(fileName);
		

		_delay_ms(3000);
		transmitString(" data written good");
		TX_NEWLINE;
	}
	
}//end of while(1)



return 0;

}//end of main

  
void init_devices(void)
{
	//all interrupts disabled
	LED_Init();

	BTN_Init();
	spi_init();
	//twi_init();
	i2c_init(I2C_Seed,F_CPU);
	//uart0_init();
	Serial_init(UART_BaudRate,F_CPU);
	cli();
}

//function to blink LED in case of any error
void Blink_Error(void)
{
	LED_SetData(workingMode_led,LED_OFF);   LED_SetData(InterfaceMode_led,LED_OFF);
	while(1){
		LED_SetData(Error_led,LED_ON);
		_delay_ms(1000);
		LED_SetData(Error_led,LED_OFF);
		_delay_ms(1000);
	}
	
}

unsigned char Mode_working(void)	{
	unsigned char  _mode;
	_mode= (SwitchBTN_GetState(Mode_BTN)== BTN_ON)?(Interface_Mode):(Working_Mode) ;
	if(_mode==Interface_Mode){
		LED_SetData(InterfaceMode_led,LED_ON);	 LED_SetData(workingMode_led,LED_OFF);
	}
	else{
		LED_SetData(InterfaceMode_led,LED_OFF);	 LED_SetData(workingMode_led,LED_ON);
	}
	return _mode;
}
unsigned char Mode_Waitworking(void)	{
	unsigned char  _mode;
	_mode= (SwitchBTN_GetState(Mode_BTN)== BTN_ON)?(Interface_Mode):(Working_Mode) ;
	
	return _mode;
}