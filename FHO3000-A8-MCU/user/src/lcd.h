 #ifndef _LCD_H
 #define _LCD_H

#include"stm32f0xx_gpio.h"

#define SYS_DIS 0
#define SYS_EN 1
#define LCD_OFF 2
#define LCD_ON 3
#define LCD_CS_HIGH()  GPIO_SetBits(GPIOF,GPIO_Pin_5)
#define LCD_CS_LOW()   GPIO_ResetBits(GPIOF,GPIO_Pin_5)
#define LCD_WR_HIGH()  GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define LCD_WR_LOW()   GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define LCD_DATA_HIGH()  GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define LCD_DATA_LOW()  GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define LCD_BL_SHUTDOWN() GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LCD_BL_TURNON()  GPIO_SetBits(GPIOB,GPIO_Pin_5)

extern unsigned char LCD_EME[14];

void ht1621_initial(void);
void send_address_ht1621(uint8_t addr);
void send_head_ht1621(uint8_t head);
void send_command_ht1621(uint8_t cmd);
void send_data_ht1621(uint8_t rdata);
void lcd_update(void);
void LCD_Init(void);
void delay_us(int m);
void delay_ms(uint16_t time);
void lcd_light(void);
#endif
