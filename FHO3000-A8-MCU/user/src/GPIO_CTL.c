#include "include.h"

//初始化所有A口的输出端口
void GPIO_PORTA_OUT_INIT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//开启IO时钟 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void GPIO_PORTA_IN_INIT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//开启IO时钟 
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//初始化所有B口的输出端口
void GPIO_PORTB_OUT_INIT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//初始化所有C口的输出端口
void GPIO_PORTC_OUT_INIT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);//开启IO时钟 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void GPIO_PORTC_IN_INIT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);//开启IO时钟 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


//初始化所有D口的输出端口
void GPIO_PORTD_OUT_INIT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);//开启IO时钟 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}
void GPIO_OUT_INIT(void)
{
	GPIO_PORTA_OUT_INIT();
	GPIO_PORTB_OUT_INIT();
	GPIO_PORTC_OUT_INIT();
	GPIO_PORTD_OUT_INIT();
	
	GPIO_PORTA_IN_INIT();
	GPIO_PORTC_IN_INIT();
	
	OPM_Channel_config();
	OPM_Discharge_config();
}
void BUZZ_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOC, GPIO_Pin_12);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}

void BUZZ_CTL_TIME(unsigned long time)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_12);
	delay_ms(time);
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}

void CPU_POWER_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOA, GPIO_Pin_7);
	else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
}

void ALL_POWER_LOCK(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOC, GPIO_Pin_0);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_0);
}
void LCD_BL_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOC, GPIO_Pin_1);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_1);
}

void USB_POWER_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOC, GPIO_Pin_2);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_2);
}

void Loading_CH(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOC, GPIO_Pin_3);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_3);
}

void CHARGE_EANBLE(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOB, GPIO_Pin_9);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_9);
}
void VFL_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOD, GPIO_Pin_2);
	else GPIO_ResetBits(GPIOD, GPIO_Pin_2);
}
void LED_VFL_RED_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOB, GPIO_Pin_12);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}
void LED_VFL_GREED_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOB, GPIO_Pin_13);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}
void LED_POWER_RED_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOB, GPIO_Pin_14);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}
void LED_POWER_GREED_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOB, GPIO_Pin_15);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void OPM_VCC_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOC, GPIO_Pin_6);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_6);
}
void LD_POWER_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOC, GPIO_Pin_13);
	else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}
void LD_Channel_CTL(unsigned char wavelength)
{
	switch(wavelength)
	{
		case 0: 
			    GPIO_ResetBits(GPIOB, GPIO_Pin_7|GPIO_Pin_8);
		      break;
		case 1:
			    GPIO_SetBits(GPIOB, GPIO_Pin_7); 
		      GPIO_ResetBits(GPIOB,GPIO_Pin_8);
		      break;
		case 2:
			    GPIO_SetBits(GPIOB, GPIO_Pin_8); 
		      GPIO_ResetBits(GPIOB,GPIO_Pin_7);
		      break;
		case 3:
			    GPIO_SetBits(GPIOB, GPIO_Pin_7|GPIO_Pin_8); 
		      break;
		default:
			    break;
	}
}
void BATT_STATUS_IO_INIT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//开启IO时钟 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void BATT_CHARGE_STATUS(void)
{
	unsigned char status=0;
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)//有外电插入
	{
		if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0)
		{
			if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0)
			{
				
			}
		}
	}
	else //没有外电插入
	{
		
	}
}

