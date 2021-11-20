#ifndef __GPIO_CTL_H__
#define __GPIO_CTL_H__

void GPIO_OUT_INIT(void);
void BUZZ_CTL_TIME(unsigned long time);
void CPU_POWER_CTL(unsigned char flag);
void CHARGE_EANBLE(unsigned char flag);
void LED_POWER_RED_CTL(unsigned char flag);
void LED_POWER_GREED_CTL(unsigned char flag);
void ALL_POWER_LOCK(unsigned char flag);
void LCD_BL_CTL(unsigned char flag);
void LED_VFL_RED_CTL(unsigned char flag);
void LED_VFL_GREED_CTL(unsigned char flag);
void USB_POWER_CTL(unsigned char flag);
void LD_POWER_CTL(unsigned char flag);
void LD_Channel_CTL(unsigned char wavelength);

#endif 
