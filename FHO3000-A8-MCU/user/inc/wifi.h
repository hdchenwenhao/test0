#ifndef __WIFI_H
#define	__WIFI_H

void USART2_Config(void);
void NVIC_Configuration_WIFI_UART(void);
void CleanBuf(void);

extern u8 TxBuf[10];//发送缓冲
extern u8 RxBuffer[10];//接收缓冲
extern u8 ReceiveCount;//接收计数变量
#endif
