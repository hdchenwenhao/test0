#ifndef __DAC_H
#define	__DAC_H


extern unsigned char UART4_COM_RX_BUFF[40];
extern unsigned char g_r_uart4_rx_point;
extern unsigned char g_r_uart4_rx_end;
extern unsigned char g_r_uartx_tybe;
void Dac1_Init(void);
void Dac2_Init(void);
void Dac1_Set_Vol(unsigned int vol);
void Dac2_Set_Vol(unsigned int vol);
void Apd_voltage_set(float apd_voltage,float k);
void USART4_Config(void);
void URAT4_NVIC_Configuration(void);
void COM4_RXD_FUNCTION(void);
void USART4_OFF(void);
void Laser_souce_lamda_set(unsigned char lamda);

#endif 

