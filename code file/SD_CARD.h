/*
 * SD_CARD.h
 *
 * Created: 03--Aug--2021 2:57:17 PM
 *  Author: Pc
 */ 


#ifndef SD_CARD_H_
#define SD_CARD_H_


//Use following macro if you don't want to activate the multiple block access functions
//those functions are not required for FAT32

#define FAT_TESTING_ONLY


#define SD_CS_ASSERT     PORTB &= ~0x10
#define SD_CS_DEASSERT   PORTB |= 0x10

//SD commands, many of these are not used here
#define GO_IDLE_STATE            0
#define SEND_OP_COND             1
#define SEND_IF_COND			 8
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define SD_SEND_OP_COND			 41   //ACMD
#define APP_CMD					 55
#define READ_OCR				 58
#define CRC_ON_OFF               59
//////////////   for writing data


#define ON     1
#define OFF    0

volatile unsigned long startBlock, totalBlocks;
volatile unsigned char SDHC_flag, cardType, buffer[512];




void  SdCard_SendCommand(unsigned char cmd, unsigned long arg);
uint8_t SdCard_RespCommand(unsigned char cmd, unsigned long arg);
uint8_t SdCard_Init(void);
uint8_t SdCard_ReadSingleBlock(uint32_t address, uint8_t *arrydata, uint8_t *token);
uint8_t SdCard_WriteSingleBlock(uint32_t address, uint8_t *arraydata, uint8_t *token);
unsigned char SdCard_Erase (unsigned long startBlock, unsigned long totalBlocks);


unsigned char SdCard_ReadMultipleBlock (unsigned long startBlock, unsigned long totalBlocks);

unsigned char SdCard_WriteMultipleBlock(unsigned long startBlock, unsigned long totalBlocks);





#endif /* SD_CARD_H_ */