//************AD9959.C***************/
#include "Include.h"


#define CLR_SCL()   GPIO_ResetBits(GPIOB,GPIO_Pin_10)
#define SET_SCL()   GPIO_SetBits(GPIOB,GPIO_Pin_10)

#define SET_CS()   GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define CLR_CS()   GPIO_ResetBits(GPIOB,GPIO_Pin_0)

#define SDA_OUT()  AD9959_SDA_DIR(1)
#define SDA_IN()   AD9959_SDA_DIR(0)

#define SET_SDIO()  GPIO_SetBits(GPIOB,GPIO_Pin_11)
#define CLR_SDIO()  GPIO_ResetBits(GPIOB,GPIO_Pin_11)



void WriteToAD9959ViaSpi(unsigned char RegisterAddress, unsigned char NumberofRegisters, unsigned char *RegisterData);
void ReadFromAD9959ViaSpi(unsigned char RegisterAddress, unsigned char NumberofRegisters, unsigned char *RegisterData);
void AD9959_IO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10|GPIO_Pin_11;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void AD9959_SDA_DIR(unsigned char flag)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;	
	if(flag)GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  else 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
	   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void AD9959_IO_UPDATA_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//开启IO时钟 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void AD9959_Reset_IO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//开启IO时钟 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}



unsigned char RegisterData[4] = {0,0,0,0};
void delay (unsigned int length)
{
	while (length >0)
	{
		length--;
		;
		;
	}
	
}

void IO_Update(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	delay_ms(500);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	delay_ms(500);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}

void AD9959_Reset(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	delay_ms(500);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	delay_ms(500);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
}

void AD9959_INIT(void)
{
	unsigned int i=0;
  AD9959_IO_INIT();
	AD9959_IO_UPDATA_INIT();
	AD9959_Reset_IO_INIT();
	
	delay(1000);
	RegisterData[0] = 0xF0;		//Channel 0,1,2,3 enable
	WriteToAD9959ViaSpi(0x00,1,RegisterData);
	RegisterData[2] = 0xCF;//AB;		//REF CLK = 50MHz, Multiplication factor = 10
	RegisterData[1] = 0x00;
	RegisterData[0] = 0x00;
	WriteToAD9959ViaSpi(0x01,3,RegisterData);//VCO gain control = 1, system clock = 500MHz

	RegisterData[3] = 0x05;
	RegisterData[2] = 0x1E;
	RegisterData[1] = 0xB8;
	RegisterData[0] = 0x51;
	WriteToAD9959ViaSpi(0x04,4,RegisterData);//Output frequency = 10MHz

	IO_Update();
    for(i=0;i<50;i++)delay_ms(2000);
	ReadFromAD9959ViaSpi(0x80,1,RegisterData);
	for(i=0;i<5;i++)delay_ms(2000);
   RegisterData[0]=0;
	 RegisterData[1]=0;
	 RegisterData[2]=0;
	 RegisterData[3]=0;
	ReadFromAD9959ViaSpi(0x81,3,RegisterData);

	
	//if(RegisterData[2]==0XFF)LED_CTL1(1);
	//else LED_CTL1(0);
	//while(1)
	//{;}
}




//---------------------------------
//WriteToAD9959ViaSpi();
//---------------------------------
//Function that writes to the AD9959 via the SPI port. It sends first the control
//word that includes the start address and then the data to write.
//--------------------------------------------------------------------------------
void WriteToAD9959ViaSpi(unsigned char RegisterAddress, unsigned char NumberofRegisters, unsigned char *RegisterData)
{
	unsigned	char	ControlValue = 0;
	unsigned	char	ValueToWrite = 0;
	signed		char	RegisterIndex = 0;
	unsigned	char	i = 0;

	//Create the 8-bit header
	//ControlValue = RegisterAddress+NumberofRegisters<<;
    ControlValue = RegisterAddress;
	CLR_SCL();
	delay_ms(10);
	SET_CS();
	delay_ms(10);
	CLR_CS();	 //bring CS low
	delay_ms(10);

	SDA_OUT();	//Make SDIO an output

	//Write out the control word
	for(i=0; i<8; i++)
	{
		CLR_SCL();
		if((ControlValue & 0x80))
		{
			SET_SDIO();	  //Send one to SDIO pin
		}
		else
		{
			CLR_SDIO();	  //Send zero to SDIO pin
		}
		delay_ms(10);
		SET_SCL();
		delay_ms(10);
		ControlValue <<= 1;	//Rotate data
	}
	CLR_SCL();
	delay_ms(20);
	//And then the data
	for (RegisterIndex=NumberofRegisters; RegisterIndex>0; RegisterIndex--)
	{
		ValueToWrite = *(RegisterData + RegisterIndex - 1);
		for (i=0; i<8; i++)
		{
			CLR_SCL();
			if((ValueToWrite & 0x80))
			{
				SET_SDIO();	  //Send one to SDIO pin
			}
			else
			{
				CLR_SDIO();	  //Send zero to SDIO pin
			}
			delay_ms(10);
			SET_SCL();
			delay_ms(10);
			ValueToWrite <<= 1;	//Rotate data
		}
	}
	CLR_SCL();
	delay_ms(10);
	SET_CS();	//bring CS high again
}

//---------------------------------
//ReadFromAD9959ViaSpi();
//---------------------------------
//Function that reads from the AD9959 via the SPI port. It first send the control word
//that includes the start address and then 32 clocks for each register to read.
//--------------------------------------------------------------------------------
void ReadFromAD9959ViaSpi(unsigned char RegisterAddress, unsigned char NumberofRegisters, unsigned char *RegisterData)
{
	unsigned	char	ControlValue = 0;
	signed		char	RegisterIndex = 0;
	unsigned	char	ReceiveData = 0;
	unsigned	char	i = 0;
	unsigned	long		iTemp = 0;

	//Create the 8-bit header
	ControlValue = RegisterAddress;

	SET_SCL();
	delay_ms(10);
	SET_CS();
	delay_ms(10);
	CLR_CS();	 //bring CS low
	delay_ms(10);

	SDA_OUT();	//Make SDIO an output

	//Write out the control word
	for(i=0; i<8; i++)
	{
		CLR_SCL();
		if((ControlValue & 0x80))
		{
			SET_SDIO();	  //Send one to SDIO pin
		}
		else
		{
			CLR_SDIO();	  //Send zero to SDIO pin
		}
		delay_ms(10);
		SET_SCL();
		delay_ms(10);
		ControlValue <<= 1;	//Rotate data
	}

	SDA_IN();	//Make SDA an input

	//Read data in
	for (RegisterIndex=NumberofRegisters; RegisterIndex>0; RegisterIndex--)
	{
		for(i=0; i<8; i++)
		{
			CLR_SCL();
			delay_ms(1);
			ReceiveData <<= 1;		//Rotate data
			iTemp =GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);// (GPIO&0X80);//&=~0X80			//Read SDIO of AD9959
			if((iTemp & 0x80))
			{
				ReceiveData |= 1;
			}
			delay_ms(10);
			SET_SCL();
			delay_ms(10);
		}
		*(RegisterData + RegisterIndex - 1) = ReceiveData;
	}
	delay(10);
	SET_CS();	//bring CS high again
}
