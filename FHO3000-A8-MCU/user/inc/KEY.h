#ifndef __KEY_H__
#define __KEY_H__


#define POWER_KEY  0x01
#define VFL_KEY    0X02


extern unsigned char g_r_key_value;
extern unsigned char g_r_long_Key_time;
void KEY_EXTI_Config(void);
void KEY_FUNCTION(void);
#endif
