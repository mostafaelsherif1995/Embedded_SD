/*
 * GPIO.h
 *
 * Created: 04/07/2017 01:23:14 ص
 *  Author: LENOVO
 */ 


#ifndef GPIO_H_
#define GPIO_H_

/*Type for the Port name*/
typedef enum {PA=0,PB,PC,PD} GPIO_PortType;

/*Initialization parameters struct type*/
typedef struct  
{
	GPIO_PortType PortName;
	unsigned char PortMask;
	unsigned char PortDirection;
	unsigned char InternPullUpUsed;
}GPIO_InitParamType;

/*Write parameters struct type*/
typedef struct  
{
	GPIO_PortType PortName;
	unsigned char PortMask;
	unsigned char PortData;
}GPIO_WriteParamType;

typedef struct  
{
	GPIO_PortType PortName;
	unsigned char PortMask;
}GPIO_ReadParamType;
/*Initialization function*/
void GPIO_Init(const GPIO_InitParamType *InitParamPtr);
/*Write function*/
void GPIO_Write(const GPIO_WriteParamType* WriteParamPtr);
/*Read function*/
unsigned char GPIO_Read(const GPIO_ReadParamType *ReadParamPtr);


#define INIT_GPIO_PIN(TEMP_VAR,PORT_NAME,PORT_MASK,Direct,Pull) TEMP_VAR.PortName = PORT_NAME;\
TEMP_VAR.PortMask = PORT_MASK;\
TEMP_VAR.PortDirection = Direct;\
TEMP_VAR.InternPullUpUsed = Pull;\
GPIO_Init(&TEMP_VAR)

#define WRITE_GPIO_DATA(TEMP_VAR,PORT_NAME,PORT_MASK,DATA) TEMP_VAR.PortName = PORT_NAME;\
TEMP_VAR.PortMask = PORT_MASK;\
TEMP_VAR.PortData = DATA;\
GPIO_Write(&TEMP_VAR)

#define READ_GPIO_DATA(TEMP_VAR,PORT_NAME,PORT_MASK,temp_data) TEMP_VAR.PortName = PORT_NAME;\
TEMP_VAR.PortMask= PORT_MASK;\
temp_data = GPIO_Read(&TEMP_VAR)







#endif /* GPIO_H_ */