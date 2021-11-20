//*************************头文件包含文件
#ifndef __INCLUDE_H__
#define __INCLUDE_H__

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef signed int INT;

#define OPM_A  1


#define MAX_WIFI_DATA_POINT  128
#define WIFI_LIVE_TIME 5
#define Resend_ERRO_TIME 200

#define REAL_TIME_MODE 2
#define AVG_TEST_MODE 1
#define STOP_MODE 0

#define English  0
#define Chinese  1
#define Language  Chinese
#define DeBug_Enable  0

#define TEM_HIGHT_ERRO_CODE    0X0001
#define VCC_LOW_ERRO_CODE      0X0002
#define FIBER_ALIVE_ERRO_CODE  0x0003
#define VBR_SET_ERRO           0X0004

#define on 0xff
#define ON on
#define off 0X00
#define OFF off
#define F_2HZ 1

//#define SamplePoint_ALL   22000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_conf.h"
#include "delay.h"
#include "GPIO_CTL.h"
#include "MyUart.h"

#include "ADC.h"
#include "HT1621B.h"
#include "KEY.h"
#include "GPIO.h"
#include "DAC.h"
#include "RTC.h"
#include "IIC_BUS.h"
#include "Timer.h"
#include "OPM.h"

#define POWER_OFF  0x00
#define POWER_OFF_CHARGE 0X01
#define POWER_OFF_STEP   0X02
#define POWER_ON   0XFF
#define LONG_KEY_POWER_OFF_TIME 5

#define TEM_BUZZ_ALARM  55
#define TEM_POWER_OFF   65

extern unsigned char Version_BUFF[10];
extern unsigned char g_r_power_flag;
extern unsigned int g_r_dc_in_flag;
extern unsigned int g_r_charge_status;
extern unsigned int g_r_Battery_capacity;
extern unsigned int g_r_Battery_Tem;
extern unsigned int g_r_AverageTimeToFull;
extern unsigned int g_r_AverageTimeToEmpty;
extern unsigned int g_r_Percentage;
extern unsigned int g_r_voltage;
extern unsigned int g_r_Battery_status;
extern unsigned char g_r_burst_information_tybe;
extern unsigned char g_r_buzz_alarm_flag;

extern unsigned char g_r_VFL_status;
extern unsigned char g_r_A8_BOOT_Loading_flag;
extern float g_r_temp_pcb;

extern unsigned char g_r_LD_wavelength;
extern unsigned char g_r_ld_power_status;
extern unsigned int LD_Voltage;
extern unsigned key_sys_loading_value;

extern unsigned char g_r_laser_souce_enable;
extern unsigned char g_r_laser_souce_status;

void Second_function(void);
void VFL_KEY_FUNNCTION(void);
void POWER_OFF_CTL_FUNCTION(void);
void POWER_ON_CTL_FUNCTION(void);
void LD_FUNCTION(void);

#endif
