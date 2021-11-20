 #ifndef _LCD_H
 #define _LCD_H


#define SYS_DIS 0
#define SYS_EN 1
#define LCD_OFF 2
#define LCD_ON 3
#define LCD_CS_HIGH()  GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define LCD_CS_LOW()   GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define LCD_WR_HIGH()  GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define LCD_WR_LOW()   GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define LCD_DATA_HIGH()  GPIO_SetBits(GPIOG,GPIO_Pin_0)
#define LCD_DATA_LOW()  GPIO_ResetBits(GPIOG,GPIO_Pin_0)
#define LCD_BL_SHUTDOWN() GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define LCD_BL_TURNON()  GPIO_SetBits(GPIOB,GCIO_Pin_6)

extern unsigned char LCD_EME[14];

void ht1621_initial(void);
void send_address_ht1621(unsigned char addr);
void send_head_ht1621(unsigned char head);
void send_command_ht1621(unsigned char cmd);
void send_data_ht1621(unsigned char rdata);
void lcd_update(void);
void LCD_Init(void);
void lcd_light(void);
#endif

