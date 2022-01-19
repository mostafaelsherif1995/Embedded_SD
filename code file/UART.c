


#include <avr/io.h>
#include <avr/pgmspace.h>
#include "UART.h"

void uart0_init(void)
{

 uint16_t UBRR_Value= lrint ( (8000000 / (8L * 1000000) ) -1);
 UCSRA|=(1<<1);
 UBRRL= (uint8_t) UBRR_Value;
 UBRRH= (uint8_t) (UBRR_Value >> 8);
 UCSRB= (1<<RXEN) | (1<<TXEN);
 UCSRB|=(1<<RXCIE);
 UCSRC|=(1<<7)|(3<<UCSZ0);
}

void Serial_init(unsigned long int baud_rate,unsigned long int clock )
{
	uint16_t UBRR_Value= lrint ( (clock / (8L * baud_rate) ) -1);
	UCSRA|=(1<<1);
	UBRRL= (uint8_t) UBRR_Value;
	UBRRH= (uint8_t) (UBRR_Value >> 8);
	UCSRB= (1<<RXEN) | (1<<TXEN);
	UCSRB|=(1<<RXCIE);
	UCSRC|=(1<<7)|(3<<UCSZ0);

	

}
unsigned char receiveByte( void )
{
	unsigned char data, status;
	
	while(!(UCSRA & (1<<RXC))); 	// Wait for incomming data
	
	status = UCSRA;
	data = UDR;
	
	return(data);
}

//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	while ( !(UCSRA & (1<<UDRE)) )
		; 			                /* Wait for empty transmit buffer */
	UDR = data; 			        /* Start transmition */
}



void transmitHex( unsigned char dataType, unsigned long data )
{
unsigned char count, i, temp;
unsigned char dataString[] = "0x        ";

if (dataType == CHAR) count = 2;
if (dataType == INT) count = 4;
if (dataType == LONG) count = 8;

for(i=count; i>0; i--)
{
  temp = data % 16;
  if((temp>=0) && (temp<10)) dataString [i+1] = temp + 0x30;
  else dataString [i+1] = (temp - 10) + 0x41;

  data = data/16;
}

transmitString (dataString);
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(unsigned char* string)
{
  while (*string)
   transmitByte(*string++);
}


void double_to_str(double num,char str[]){
	long long int_num;
	unsigned int length=0,lenght_f=0,dot_pos=0,sign=0,dot=0;
	int index=0;
	double f_num;

	if(num < 0){
		sign='-';
		num *=-1;
	}
	else{
		
	}
	f_num=num;
	int_num= num;
	while((f_num-(double)int_num)!=0.0 && !((f_num - (double)int_num) < 0.0) ){
		lenght_f++;
		f_num *=10;
		int_num=f_num;
		
	}
	
	for (length = (num > 1) ? 0 : 1; num > 1; length++){
		if(num==10){
			length++;
		}
	else	{}
	
	num/= 10;
}

if(lenght_f){
	dot=1;
	dot_pos=length;
}
else{
	dot=0;
}

length=length + dot + lenght_f;
if(sign=='-'&&dot==1){
	length++;
	dot_pos++;
}
else if(sign=='-'){
	length++;
}
 else{}////dont make any thing

 for (index= length; index >= 0; index--)
 {
	 if (index == length)
	 str[index] = '\0';
	 else if (index == (dot_pos)&&dot==1)
	 str[index] = '.';
	 else if (sign == '-' && index == 0)
	 str[index] = '-';
	 else
	 {
		 str[index] = (int_num%10) + '0';
		 
			
			
		int_num /= 10;
		
	 }
 }

}
void transmitNum(double number){
	 char str[12];
	 
	 double_to_str(number, str);
	 transmitString(str);

}