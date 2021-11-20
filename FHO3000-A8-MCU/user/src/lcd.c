#include"lcd.h"
#include"stm32f0xx_gpio.h"
#include"stm32f0xx_rcc.h"
extern unsigned char LCD_MEM[14];
void delay_us(int m)
{
	int i,j;
	for(i=0;i<m;i++)
	{ 
		for(j=0;j<12;j++);
	}
}
  
void delay_ms(uint16_t time)  //ÑÓÊ±º¯Êý
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=12000;j>0;j--);
}
void LCD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOF, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5 ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}

/****************************************************************************
*						Name:	ht1621_initial(void)
*						Return:	void
*						Description:For HT1621B initialization
****************************************************************************/
void ht1621_initial(void)
{
	unsigned char i;
	LCD_CS_LOW();
	send_head_ht1621(4);
	send_command_ht1621(SYS_EN);
	send_command_ht1621(LCD_ON);
	send_command_ht1621(0x29);					//Set 4 commons
	LCD_CS_HIGH();

	for(i=0;i<14;i++) 
	{
		LCD_CS_LOW();		
		delay_us(20);
		send_head_ht1621(5);
		send_address_ht1621(2*i);
		send_data_ht1621(0);	
		LCD_CS_HIGH();
		delay_us(20);
		LCD_CS_LOW();		
		delay_us(20);   
		send_head_ht1621(5);
		send_address_ht1621(2*i+1);
		send_data_ht1621(0);	
		LCD_CS_HIGH();
		delay_us(20);		 	
	}
}



/****************************************************************************
*						Name:	send_head_ht1621(uint8_t)
*						Return:	void
*						Description:Send  head of command to HT1621B
****************************************************************************/
void send_head_ht1621(uint8_t head)
{
	unsigned char i;
	for(i=0;i<3;i++)							
	{
		LCD_WR_LOW();
		delay_us(20);
		if(head&4) LCD_DATA_HIGH();
		else LCD_DATA_LOW();
		delay_us(20);
		head<<=1;
		LCD_WR_HIGH();
		delay_us(20);
	}
}


/****************************************************************************
*						Name:	send_command_ht1621(uint8_t)
*						Return:	void
*						Description:Send command  to HT1621B
****************************************************************************/
void send_command_ht1621(uint8_t cmd)
{
	unsigned char i;
	for(i=0;i<9;i++)						
	{
		LCD_WR_LOW();
		delay_us(20);
		if(cmd&0x80) LCD_DATA_HIGH();
		else LCD_DATA_LOW();
		delay_us(20);
		cmd<<=1;
		LCD_WR_HIGH();
		delay_us(20);
	}
}


/****************************************************************************
*						Name:	send_address_ht1621(uint8_t)
*						Return:	void
*						Description:Send address  to HT1621B
****************************************************************************/
void send_address_ht1621(uint8_t addr)
{
	unsigned char i;
	for(i=0;i<6;i++)						
	{
		LCD_WR_LOW();
		delay_us(20);
		if(addr&0x20) LCD_DATA_HIGH();
		else LCD_DATA_LOW();
		delay_us(20);
		addr<<=1;
		LCD_WR_HIGH();
		delay_us(20);
	}
}


/****************************************************************************
*						Name:	send_data_ht1621(uint8_t)
*						Return:	void
*						Description:Send data to HT1621B
****************************************************************************/
void send_data_ht1621(uint8_t rdata)
{
	unsigned char i;
	for(i=0;i<4;i++)						
	{
		LCD_WR_LOW();
		delay_us(20);
		if(rdata&1) LCD_DATA_HIGH();
		else LCD_DATA_LOW();
		delay_us(20);
		rdata>>=1;
		LCD_WR_HIGH();
		delay_us(20);
	}
}



/****************************************************************************
*						Name:	lcd_update(void)
*						Return:	void
*						Description:Update display data in HT1621B
****************************************************************************/
void lcd_update(void)
{
	unsigned char i;
	static unsigned char  former_bit_map[14];
	LCD_CS_LOW();		
	delay_us(20);
	send_head_ht1621(5);
	send_address_ht1621(0);
	send_data_ht1621(LCD_MEM[0]&0xf);
	LCD_CS_HIGH();
	delay_us(20);
	for(i=0;i<8;i++) 					
	{
		if((LCD_MEM[i]&0xf)!=(former_bit_map[i]&0xf)) 
		{
			LCD_CS_LOW();		
			delay_us(20);
			send_head_ht1621(5);
			send_address_ht1621(2*i-1);
			send_data_ht1621(LCD_MEM[i]&0xf);
			LCD_CS_HIGH();
			delay_us(20);
		}
		if((LCD_MEM[i]&0xf0)!=(former_bit_map[i]&0xf0)) 
		{
			LCD_CS_LOW();		
			delay_us(20);   
			send_head_ht1621(5);
			send_address_ht1621(2*i);
			send_data_ht1621((LCD_MEM[i]&0xf0)>>4);	
			LCD_CS_HIGH();
			delay_us(20);
		}		
		former_bit_map[i]=LCD_MEM[i];
	}
}





