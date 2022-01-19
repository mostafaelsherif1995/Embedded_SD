/*
 * SD_CARD.c
 *
 * Created: 03--Aug--2021 2:57:37 PM
 *  Author: Pc
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "SPI.h"
#include "SD_CARD.h"
#include "UART.h"

//******************************************************************
#define SD_MAX_READ_ATTEMPTS    1563
#define SD_READ_START_TOKEN     0xFE
#define SD_INIT_CYCLES          80

#define SD_START_TOKEN          0xFE
#define SD_ERROR_TOKEN          0x00

#define SD_DATA_ACCEPTED        0x05
#define SD_DATA_REJECTED_CRC    0x0B
#define SD_DATA_REJECTED_WRITE  0x0D
#define SD_MAX_WRITE_ATTEMPTS   3907
#define SD_R1_NO_ERROR(X)   X < 0x02
#define SD_BLOCK_LEN            512

#define SD_TOKEN_OOR(X)     X & 0b00001000
#define SD_TOKEN_CECC(X)    X & 0b00000100
#define SD_TOKEN_CC(X)      X & 0b00000010
#define SD_TOKEN_ERROR(X)   X & 0b00000001

#define PARAM_ERROR(X)      X & 0b01000000
#define ADDR_ERROR(X)       X & 0b00100000
#define ERASE_SEQ_ERROR(X)  X & 0b00010000
#define CRC_ERROR(X)        X & 0b00001000
#define ILLEGAL_CMD(X)      X & 0b00000100
#define ERASE_RESET(X)      X & 0b00000010
#define IN_IDLE(X)          X & 0b00000001
#define CMD_VER(X)          ((X >> 4) & 0xF0)
#define VOL_ACC(X)          (X & 0x1F)

#define VOLTAGE_ACC_27_33   0b00000001
#define VOLTAGE_ACC_LOW     0b00000010
#define VOLTAGE_ACC_RES1    0b00000100
#define VOLTAGE_ACC_RES2    0b00001000
#define POWER_UP_STATUS(X)  X & 0x40
#define CCS_VAL(X)          X & 0x40
#define VDD_2728(X)         X & 0b10000000
#define VDD_2829(X)         X & 0b00000001
#define VDD_2930(X)         X & 0b00000010
#define VDD_3031(X)         X & 0b00000100
#define VDD_3132(X)         X & 0b00001000
#define VDD_3233(X)         X & 0b00010000
#define VDD_3334(X)         X & 0b00100000
#define VDD_3435(X)         X & 0b01000000
#define VDD_3536(X)         X & 0b10000000

static void  SdCard_WalkupSD(void);
static uint8_t SdCard_ReadReg1(void);
static uint8_t SdCard_SendCMD0(void);
static void SdCard_ReadReg7(uint8_t *res);
static void SdCard_GetSD_Vers(uint8_t *res);
static void SdCard_PrintReg1(uint8_t res);
static void SdCard_PrintReg7(uint8_t *res);
static void SdCard_ReadReg3(uint8_t *res);
static void SdCard_ReadOCR(uint8_t *res);
static void SdCard_PrintR3(uint8_t *res);
static uint8_t SdCard_SendLedACMD(void)  ;
static uint8_t  SdCard_SendOpCondver2(void) ;
static uint8_t  SdCard_SendOpCondver1(void) ;
static void SdCard_PrintDataErrToken(uint8_t token);



//******************************************************************


uint8_t SdCard_Init(void)
{	unsigned char  SD_version,res[5];
	unsigned int retry=0 ;

	SdCard_WalkupSD();
	do{
		res[0] =SdCard_SendCMD0();
		SdCard_PrintReg1(res[0]);
		retry++;
		if(retry > 0XFF) return 1;
		
	}while(res[0] != 0x01);
	retry = 0;
	SD_version = 2; //default set to SD  with ver2.x and check output of sd cm8 if illigal bit it means version 1 other version 2;

	do
	{ SdCard_GetSD_Vers(res);
		SdCard_PrintReg1(res);
		retry++;
		if((retry>20) &&(ILLEGAL_CMD(res[0])))
		{
			TX_NEWLINE;
			transmitString("SD Card Version is:Version1 and card Type is :type1");
			TX_NEWLINE;
			SD_version = 1;
			cardType = 1;
			break;
		} //time out

	}while(res[0] != 0x01);  //not ideal stata

	retry = 0;
	SDHC_flag = 0;
	if(SD_version ==1){
		do{
			
			res[0]=SdCard_SendOpCondver1();
			SdCard_PrintReg1(res[0]);
			if(retry > 40 ) return 1;
			
		}while(res[0] !=0x00);        //not card ready dont out from while except after time count
		

	}
	else{
		do
		{
			SdCard_SendLedACMD();
			res[0]= SdCard_SendOpCondver2() ;

			retry++;
			if(retry>0xfe)
			{
				TX_NEWLINE;
				return 2;  //time out, card initialization failed
			}

		}while(res[0] != 0x00);   //not ready
		
		retry=0;
		do
		{
			SdCard_ReadOCR(res);
			if(res[0] == 0x00 )  //checking response of CMD58
			{
				
				if((res[1] & 0x40) == 0x40) SDHC_flag = 1;  //we need it to verify SDHC card
				else SDHC_flag = 0;
			}
			
			retry++;
			if(retry>0xfe)
			{
				TX_NEWLINE;
				cardType = 0;
				break;
			} //time out

		}while(res[0] != 0x00);

		if(SDHC_flag == 1) cardType = 2;
		else cardType = 3;
		
	}

	//SD_sendCommand(CRC_ON_OFF, OFF); //disable CRC; deafault - CRC disabled in SPI mode
	//SD_sendCommand(SET_BLOCK_LEN, 512); //set block size to 512; default size is 512

  SPI_HIGH_SPEED;	//SCK - 4 MHz
	return 0; //successful return
}



uint8_t SdCard_ReadSingleBlock(uint32_t address, uint8_t *arrydata, uint8_t *token)
{
	uint8_t res1, read;
	uint16_t readAttempts;

	// set token to none
	*token = 0xFe;

	
	SPI_transfer(0xFF);
	SD_CS_ASSERT;
	SPI_transfer(0xFF);

	// send CMD17
	SdCard_SendCommand(READ_SINGLE_BLOCK,address );
	

	// read R1
	res1 = SdCard_ReadReg1();

	// if response received from card
	if(res1 != 0xFF)
	{
		// wait for a response token (timeout = 100ms)
		readAttempts = 0;
		while(++readAttempts != SD_MAX_READ_ATTEMPTS){
		if((read = SPI_transfer(0xFF)) != 0xFF) break;
		}

		// if response token is 0xFE
		if(read == 0xFE)
		{
			// read 512 byte block
			for(uint16_t i = 0; i < 512; i++) *arrydata++ = SPI_transfer(0xFF);

			// read 16-bit CRC
			SPI_transfer(0xFF);
			SPI_transfer(0xFF);
		}

		// set token to card response
		*token = read;
	}

	// deassert chip select
	SPI_transfer(0xFF);
	SD_CS_DEASSERT;
	SPI_transfer(0xFF);

	return res1;
}

uint8_t SdCard_WriteSingleBlock(uint32_t address, uint8_t *arraydata, uint8_t *token)
{
	uint8_t readAttempts, read;
	unsigned char res;
	// set token to none
	*token = 0xFE;

	// assert chip select
	SPI_transfer(0xFF);
	SD_CS_ASSERT;
	SPI_transfer(0xFF);

	// send CMD24
	SdCard_SendCommand( WRITE_SINGLE_BLOCK ,address );
	
	// read response
	res =  SdCard_ReadReg1();

	// if no error
	if(res == 0)
	{
		// send start token
		SPI_transfer(SD_START_TOKEN);

		// write buffer to card
		for(uint16_t i = 0; i < SD_BLOCK_LEN; i++) SPI_transfer(arraydata[i]);

		// wait for a response (timeout = 250ms)
		readAttempts = 0;
		while(++readAttempts != SD_MAX_WRITE_ATTEMPTS)
	if((read = SPI_transfer(0xFF)) != 0xFF) { *token = 0xFF; break; }

	// if data accepted
	if((read & 0x1F) == 0x05)
	{
		// set token to data accepted
		*token = 0x05;

		// wait for write to finish (timeout = 250ms)
		readAttempts = 0;
		while(SPI_transfer(0xFF) == 0x00)
	if(++readAttempts == SD_MAX_WRITE_ATTEMPTS) { *token = 0x00; break; }
}
    }

    // deassert chip select
    SPI_transfer(0xFF);
   SD_CS_DEASSERT;
    SPI_transfer(0xFF);

    return res;
}





unsigned char SdCard_Erase (unsigned long startBlock, unsigned long totalBlocks)
{
unsigned char response;

 response = SdCard_RespCommand(ERASE_BLOCK_START_ADDR, startBlock); //send starting block address

if(response != 0x00) //check for SD status: 0x00 - OK (No flags set)
  return response;

 response = SdCard_RespCommand(ERASE_BLOCK_END_ADDR,(startBlock + totalBlocks - 1)); //send end block address

if(response != 0x00)
  return response;

 response = SdCard_RespCommand(ERASE_SELECTED_BLOCKS, 0); //erase all selected blocks
if(response != 0x00)
  return response;

return 0; //normal return
}




#ifndef FAT_TESTING_ONLY


unsigned char SdCard_ReadMultipleBlock (unsigned long startBlock, unsigned long totalBlocks)
{
unsigned char response;
unsigned int i, retry=0;
retry = 0;

response = SdCard_RespCommand(READ_MULTIPLE_BLOCKS, startBlock); 
if(response != 0x00) return response; 
SD_CS_ASSERT;

while( totalBlocks )
{
  retry = 0;
  while(SPI_receive() != 0xfe) //wait for start block token 0xfe (0x11111110)
  if(retry++ > 0xfffe){SD_CS_DEASSERT; return 1;} //return if time-out

  for(i=0; i<512; i++) //read 512 bytes
    buffer[i] = SPI_receive();

  SPI_receive(); //receive incoming CRC (16-bit), CRC is ignored here
  SPI_receive();

  SPI_receive(); //extra 8 cycles
  TX_NEWLINE;
  transmitString_F(PSTR(" --------- "));
  TX_NEWLINE;

  for(i=0; i<512; i++) //send the block to UART
  {
    if(buffer[i] == '~') break;
    transmitByte ( buffer[i] );
  }

  TX_NEWLINE;
  transmitString_F(PSTR(" --------- "));
  TX_NEWLINE;
  totalBlocks--;
}

SdCard_RespCommand(STOP_TRANSMISSION, 0); //command to stop transmission
SD_CS_DEASSERT;
SPI_receive(); //extra 8 clock pulses

return 0;
}


unsigned char SdCard_WriteMultipleBlock(unsigned long startBlock, unsigned long totalBlocks)
{
unsigned char response, data;
unsigned int i, retry=0;
unsigned long blockCounter=0, size;

response = SdCard_RespCommand(WRITE_MULTIPLE_BLOCKS, startBlock); //write a Block command

if(response != 0x00) return response; //check for SD status: 0x00 - OK (No flags set)

SD_CS_ASSERT;

TX_NEWLINE;
transmitString_F(PSTR(" Enter text (End with ~): "));
TX_NEWLINE;

while( blockCounter < totalBlocks )
{
   i=0;
   do
   {
     data = receiveByte();
     if(data == 0x08)	//'Back Space' key pressed
	 { 
	   if(i != 0)
	   { 
	     transmitByte(data);
	     transmitByte(' '); 
	     transmitByte(data); 
	     i--; 
		 size--;
	   } 
	   continue;     
	 }
     transmitByte(data);
     buffer[i++] = data;
     if(data == 0x0d)
     {
        transmitByte(0x0a);
        buffer[i++] = 0x0a;
     }
	 if(i == 512) break;
   }while (data != '~');

   TX_NEWLINE;
   transmitString_F(PSTR(" ---- "));
   TX_NEWLINE;

   SPI_transmit(0xfc); //Send start block token 0xfc (0x11111100)

   for(i=0; i<512; i++) //send 512 bytes data
     SPI_transmit( buffer[i] );

   SPI_transmit(0xff); //transmit dummy CRC (16-bit), CRC is ignored here
   SPI_transmit(0xff);

   response = SPI_receive();
   if( (response & 0x1f) != 0x05) //response= 0xXXX0AAA1 ; AAA='010' - data accepted
   {                              //AAA='101'-data rejected due to CRC error
      SD_CS_DEASSERT;             //AAA='110'-data rejected due to write error
      return response;
   }

   while(!SPI_receive()) //wait for SD card to complete writing and get idle
     if(retry++ > 0xfffe){SD_CS_DEASSERT; return 1;}

   SPI_receive(); //extra 8 bits
   blockCounter++;
}

SPI_transmit(0xfd); //send 'stop transmission token'

retry = 0;

while(!SPI_receive()) //wait for SD card to complete writing and get idle
   if(retry++ > 0xfffe){SD_CS_DEASSERT; return 1;}

SD_CS_DEASSERT;
SPI_transmit(0xff); //just spend 8 clock cycle delay before reasserting the CS signal
SD_CS_ASSERT; //re assertion of the CS signal is required to verify if card is still busy

while(!SPI_receive()) //wait for SD card to complete writing and get idle
   if(retry++ > 0xfffe){SD_CS_DEASSERT; return 1;}
SD_CS_DEASSERT;

return 0;
}
//*********************************************

#endif
//******************************************************************
//******init function
void  SdCard_SendCommand(unsigned char cmd, unsigned long arg)
{	
if(cmd == READ_SINGLE_BLOCK     ||   cmd == READ_MULTIPLE_BLOCKS  ||   cmd == WRITE_SINGLE_BLOCK    ||   cmd == WRITE_MULTIPLE_BLOCKS ||   cmd == ERASE_BLOCK_START_ADDR|| cmd == ERASE_BLOCK_END_ADDR ) 
   {
     arg = arg << 9;
   }	   

SPI_transmit(cmd | 0x40); //send command, first two bits always '01'
SPI_transmit(arg>>24);
SPI_transmit(arg>>16);
SPI_transmit(arg>>8);
SPI_transmit(arg);

if(cmd == SEND_IF_COND)	 //it is compulsory to send correct CRC for CMD8 (CRC=0x87) & CMD0 (CRC=0x95)
  SPI_transmit(0x87);    //for remaining commands, CRC is ignored in SPI mode
else 
  SPI_transmit(0x95); 


}
void  SdCard_WalkupSD()
{
	SD_CS_DEASSERT;
	_delay_ms(1);
	for(uint8_t i = 0; i < 10; i++)
	SPI_transfer(0xFF);
	SD_CS_DEASSERT;
	SPI_transfer(0xFF);
}
uint8_t SdCard_ReadReg1()
{
	uint8_t i = 0, res1;

	
	while((res1 = SPI_transfer(0xFF)) == 0xFF)
	{
		i++;

		
		if(i > 8) break; //error
	}

	return res1;
}
uint8_t SdCard_SendCMD0()
{
	// assert chip select
	SPI_transfer(0xFF);
	SD_CS_ASSERT;
	SPI_transfer(0xFF);

	
		SdCard_SendCommand( GO_IDLE_STATE, 0x00);


	uint8_t res1 = SdCard_ReadReg1();


	SPI_transfer(0xFF);
	SD_CS_DEASSERT;
	SPI_transfer(0xFF);

	return res1;
}
void SdCard_ReadReg7(uint8_t *res)
{
	// read response 1 in R7
	res[0] = SdCard_ReadReg1();

	// if error reading R1, return
	if(res[0] > 1) return;

	// read remaining bytes
	res[1] = SPI_transfer(0xFF);
	res[2] = SPI_transfer(0xFF);
	res[3] = SPI_transfer(0xFF);
	res[4] = SPI_transfer(0xFF);
}
void SdCard_GetSD_Vers(uint8_t *res)
{
	// assert chip select
	SPI_transfer(0xFF);
	SD_CS_ASSERT;
	SPI_transfer(0xFF);
	SdCard_SendCommand( SEND_IF_COND, 0x0000001AA);
    SdCard_ReadReg7(res);
	SPI_transfer(0xFF);
	SD_CS_DEASSERT;
	SPI_transfer(0xFF);
}
void SdCard_PrintReg1(uint8_t res)
{
	if(res & 0b10000000)
{ transmitString("\tError: MSB = 1\r\n"); return; }
	if(res == 0)
{ transmitString("\tCard Ready\r\n"); return; }
	if(PARAM_ERROR(res))
	transmitString("\tParameter Error\r\n");
	if(ADDR_ERROR(res))
	transmitString("\tAddress Error\r\n");
	if(ERASE_SEQ_ERROR(res))
	transmitString("\tErase Sequence Error\r\n");
	if(CRC_ERROR(res))
	transmitString("\tCRC Error\r\n");
	if(ILLEGAL_CMD(res))
	transmitString("\tIllegal Command\r\n");
	if(ERASE_RESET(res))
	transmitString("\tErase Reset Error\r\n");
	if(IN_IDLE(res))
	transmitString("\tIn Idle State\r\n");
}
void SdCard_PrintReg7(uint8_t *res)
{
	 SdCard_PrintReg1(res[0]);

	if(res[0] > 1) return;

	transmitString("\tCommand Version: ");
	transmitHex( INT, CMD_VER(res[1]));
	transmitString("\r\n");

	transmitString("\tVoltage Accepted: ");
	if(VOL_ACC(res[3]) == VOLTAGE_ACC_27_33)
	transmitString("2.7-3.6V\r\n");
	else if(VOL_ACC(res[3]) == VOLTAGE_ACC_LOW)
	transmitString("LOW VOLTAGE\r\n");
	else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES1)
	transmitString("RESERVED\r\n");
	else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES2)
	transmitString("RESERVED\r\n");
	else
	transmitString("NOT DEFINED\r\n");

	transmitString("\tEcho: ");
	transmitHex( INT, res[4]);
	transmitString("\r\n");
}
void SdCard_ReadReg3(uint8_t *res)
{
	// read R1
	res[0] = SdCard_ReadReg1();

	// if error reading R1, return
	if(res[0] > 1) return;

	// read remaining bytes
	
	res[1] = SPI_transfer(0xff);
	res[2] = SPI_transfer(0xff);
	res[3] = SPI_transfer(0xff);
	res[4] = SPI_transfer(0xff);
}
void SdCard_ReadOCR(uint8_t *res)  //to show voltage level
{
	// assert chip select
	SPI_transfer(0xFF);
	SD_CS_ASSERT;
	SPI_transfer(0xFF);

	// send CMD58
	
	SdCard_SendCommand(READ_OCR, 0x00);

	// read response
	SdCard_ReadReg3(res);

	// deassert chip select
	SPI_transfer(0xFF);
	SD_CS_DEASSERT;
	SPI_transfer(0xFF);
}
void SdCard_PrintR3(uint8_t *res)
{
	SdCard_PrintReg1(res[0]);

	if(res[0] > 1) return;

	transmitString("\tCard Power Up Status: ");
	if(POWER_UP_STATUS(res[1]))
	{
		transmitString("READY\r\n");
		transmitString("\tCCS Status: ");
	if(CCS_VAL(res[1])){ transmitString("1\r\n"); }
	else transmitString("0\r\n");
}
else
{
	transmitString("BUSY\r\n");
}

transmitString("\tVDD Window: ");
if(VDD_2728(res[3])) transmitString("2.7-2.8, ");
if(VDD_2829(res[2])) transmitString("2.8-2.9, ");
if(VDD_2930(res[2])) transmitString("2.9-3.0, ");
if(VDD_3031(res[2])) transmitString("3.0-3.1, ");
if(VDD_3132(res[2])) transmitString("3.1-3.2, ");
if(VDD_3233(res[2])) transmitString("3.2-3.3, ");
if(VDD_3334(res[2])) transmitString("3.3-3.4, ");
if(VDD_3435(res[2])) transmitString("3.4-3.5, ");
if(VDD_3536(res[2])) transmitString("3.5-3.6");
transmitString("\r\n");
}
uint8_t SdCard_SendLedACMD()
{
	// assert chip select
	SPI_transfer(0xFF);
		SD_CS_ASSERT;
	SPI_transfer(0xFF);

SdCard_SendCommand(APP_CMD, 0x00);
	
	uint8_t res1 = SdCard_ReadReg1();

	// deassert chip select
	SPI_transfer(0xFF);
		SD_CS_DEASSERT;
	SPI_transfer(0xFF);

	return res1;
}
uint8_t  SdCard_SendOpCondver2()
{
	// assert chip select
	SPI_transfer(0xFF);
	SD_CS_ASSERT;
	SPI_transfer(0xFF);

	// send CMD0
SdCard_SendCommand(SD_SEND_OP_COND, 0x00);

	// read response
	uint8_t res1 = SdCard_ReadReg1();

	// deassert chip select
	SPI_transfer(0xFF);
	SD_CS_DEASSERT;
	SPI_transfer(0xFF);

	return res1;
}
uint8_t  SdCard_SendOpCondver1(){
	SPI_transfer(0xFF);
	SD_CS_ASSERT;
	SPI_transfer(0xFF);

SdCard_SendCommand(SEND_OP_COND , 0x00);
	uint8_t res1 = SdCard_ReadReg1();

	// deassert chip select
	SPI_transfer(0xFF);
	SD_CS_DEASSERT;
	SPI_transfer(0xFF);

	return res1;
	
	
}
uint8_t SdCard_RespCommand(unsigned char cmd, unsigned long arg){
		SPI_transfer(0xFF);
		SD_CS_ASSERT;
		SPI_transfer(0xFF);

		SdCard_SendCommand(cmd , arg);
		uint8_t res1 = SdCard_ReadReg1();

		// deassert chip select
		SPI_transfer(0xFF);
		SD_CS_DEASSERT;
		SPI_transfer(0xFF);

		return res1;
}

 void SdCard_PrintDataErrToken(uint8_t token)
{
	if(SD_TOKEN_OOR(token))
	transmitString("\tData out of range\r\n");
	if(SD_TOKEN_CECC(token))
	transmitString("\tCard ECC failed\r\n");
	if(SD_TOKEN_CC(token))
	transmitString("\tCC Error\r\n");
	if(SD_TOKEN_ERROR(token))
	transmitString("\tError\r\n");
}


