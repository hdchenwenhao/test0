#ifndef __GPIO_H
#define	__GPIO_H
void POWER_LOCK_CTL(unsigned char flag);
void OPM_VCC_CTL_Config(void);
void GPIO_CTL_INIT(void);
void OPM_VCC_CTL(unsigned char flag);
void LED_WIFI_STATUS_CTL(unsigned char led_status);
void VFL_CTL(unsigned char flag);
void BUZZ_CTL(unsigned char flag);
void BUZZ_CTL_time(unsigned long time);
void BL_CTL(unsigned char flag);
void AMP_VCC_CTL(unsigned char flag);
void APD_VCC_ENABLE(unsigned char flag);
void OPM_VCC_CTL(unsigned char flag);
void LED_ERRO_ALAM(unsigned int flag);
void LED_TEST_STATUS_CTL(unsigned char led_status);
void WIFI_VCC_CTL(unsigned char flag);
#endif

