#ifndef __WIFI_H
#define	__WIFI_H

void USART2_Config(void);
void NVIC_Configuration_WIFI_UART(void);
void CleanBuf(void);

extern u8 TxBuf[10];//���ͻ���
extern u8 RxBuffer[10];//���ջ���
extern u8 ReceiveCount;//���ռ�������
#endif
