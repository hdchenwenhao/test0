#ifndef __RTC_H
#define	__RTC_H

//typedef struct rtc_time
//	{
//	int tm_sec;
//	int tm_min;
//	int tm_hour;
//	int tm_mday;
//	int tm_mon;
//	int tm_year;
//	int tm_wday;
//}systmtime;
typedef struct rtc_time
	{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
}sys_time;    

void GregorianDay(struct rtc_time * tm);
unsigned long mktimev(struct rtc_time *tm);
void to_tm(unsigned long tim, struct rtc_time * tm);

/* Private function prototypes -----------------------------------------------*/
void RTC_Configuration(void);
void RTC_NVIC_Configuration(void);
void Time_Regulate(struct rtc_time *tm);
//uint32_t Time_Regulate(void);
void Time_Adjust(void);
void Time_Show(void);
void Time_Display(unsigned int TimeVar);
void InterruptConfig(void);
void RTC_EXTI_Config(void);
extern unsigned int g_r_power_auto_off_time;
extern unsigned char g_r_WIFI_LIVE_time;
extern unsigned char TimeDisplay;

#endif
