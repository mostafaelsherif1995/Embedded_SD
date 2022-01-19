/*
 * BTN.c
 *
 * Created: 8/30/2016 9:24:00 PM
 *  Author: mostafa
 */
#include "GPIO.h"
#include "BTN.h"
#include "BTN_Cfg.h" 
extern const BTN_ConfigType BTN_ConfigParam[BTN_MAX_NUM];
static BTN_StateType BtnState[BTN_MAX_NUM];
void BTN_Init(void)
{
	unsigned long int LoopIndex;
	GPIO_InitParamType BtnParam;
		  BtnParam.PortDirection = 0x00;
	for(LoopIndex=0; LoopIndex < BTN_MAX_NUM; LoopIndex++){
	 
		BtnParam.InternPullUpUsed = BTN_ConfigParam[LoopIndex].BtnConnection;
		BtnParam.PortMask = 1 << BTN_ConfigParam[LoopIndex].BtnPin;
		BtnParam.PortName = BTN_ConfigParam[LoopIndex].BtnPort;
		GPIO_Init(&BtnParam);
		BtnState[LoopIndex] = BTN_OFF;
	}
}
BTN_StateType BTN_GetState(unsigned long int BtnId)
{
	BTN_StateType FunStat;
	GPIO_ReadParamType  BtnParam;
	unsigned char BtnRead=0;
	
	if(BtnId < BTN_MAX_NUM){
		BtnParam.PortName=BTN_ConfigParam[BtnId].BtnPort;
		BtnParam.PortMask= 1  << BTN_ConfigParam[BtnId].BtnPin;
          BtnRead= GPIO_Read(&BtnParam);
   if((BtnRead!=0)&&(BtnState[BtnId])== BTN_OFF){
						FunStat=BTN_JUST_PRESSED;
						             }
	 if((BtnRead!=0)&& BtnState[BtnId]==BTN_JUST_PRESSED){
						FunStat=BTN_ON;
						
	}		
 if(BtnRead==0&& BtnState[BtnId]==BTN_ON){  
				  
                         FunStat=BTN_JUST_RELEASED;
	
	                         }                           
					
 if(BtnRead==0&&BtnState[BtnId]== BTN_JUST_RELEASED){
			FunStat=BTN_OFF;
						
					  }
					  		
	  else{}//else to make program good
		
	
	}	
	else{
		FunStat=255;
	}
	
	BtnState[BtnId]=FunStat;
	return FunStat;
}
BTN_StateType SwitchBTN_GetState(unsigned long int BtnId)
{
	GPIO_ReadParamType  BtnParam;
	unsigned char temp;
	BTN_StateType SwitchBtn_Stat=BTN_OFF;
	
	BtnParam.PortName=BTN_ConfigParam[BtnId].BtnPort;
	BtnParam.PortMask= 1  << BTN_ConfigParam[BtnId].BtnPin;
	 temp= GPIO_Read(&BtnParam);
	
	if(temp)
		SwitchBtn_Stat= BTN_ON;
	
	
	
	return SwitchBtn_Stat;

	
	


}
unsigned char BTN_Get(unsigned long int BtnId)
{

	GPIO_ReadParamType  BtnParam;
	
	
		BtnParam.PortName=BTN_ConfigParam[BtnId].BtnPort;
		BtnParam.PortMask= 1  << BTN_ConfigParam[BtnId].BtnPin;
		return  GPIO_Read(&BtnParam);
	
}
unsigned char BTN_State(unsigned long int BtnId,BTN_StateType* BtnStatePtr)
{
	unsigned char Temp_State;
	unsigned char FunState=1;
	GPIO_ReadParamType ReadParam;
	
	ReadParam.PortName = BTN_ConfigParam[BtnId].BtnPort;
	ReadParam.PortMask = (1<<BTN_ConfigParam[BtnId].BtnPin);
	
	Temp_State = GPIO_Read(&ReadParam);
	
	switch (Temp_State>>BtnId)
	{
		case 0:
		{
			switch(BtnState[BtnId])
			{
				case BTN_OFF:
				{
					*BtnStatePtr=BTN_OFF;
					BtnState[BtnId]=BTN_OFF;
					FunState=0;
				}
				break;
				
				case BTN_ON:
				{
					*BtnStatePtr=BTN_JUST_RELEASED;
					BtnState[BtnId]=BTN_JUST_RELEASED;
					FunState=0;
				}
				break;
				
				case BTN_JUST_PRESSED:
				{
					*BtnStatePtr=BTN_JUST_RELEASED;
					BtnState[BtnId]=BTN_JUST_RELEASED;
					FunState=0;
				}
				break;
				
				case BTN_JUST_RELEASED:
				{
					*BtnStatePtr=BTN_OFF;
					BtnState[BtnId]=BTN_OFF;
					FunState=0;
				}
				break;
				
				default:
				//do no thing
				break;
			}
		}
		break;

		case 1:
		{
			switch(BtnState[BtnId])
			{
				case BTN_OFF:
				{
					*BtnStatePtr=BTN_JUST_PRESSED;
					BtnState[BtnId]=BTN_JUST_PRESSED;
					FunState=0;
				}
				break;
				
				case BTN_ON:
				{
					*BtnStatePtr=BTN_ON;
					BtnState[BtnId]=BTN_ON;
					FunState=0;
				}
				break;
				
				case BTN_JUST_PRESSED:
				{
					*BtnStatePtr=BTN_ON;
					BtnState[BtnId]=BTN_ON;
					FunState=0;
				}
				break;
				
				case BTN_JUST_RELEASED:
				{
					*BtnStatePtr=BTN_JUST_PRESSED;
					BtnState[BtnId]=BTN_JUST_PRESSED;
					FunState=0;
				}
				break;
				
				default:
				//do no thing
				break;
			}
		}
		break;

		default:
		//do no thing
		break;
	}
	
	return FunState;
}