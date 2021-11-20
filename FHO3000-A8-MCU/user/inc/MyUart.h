#ifndef __MYUART_H
#define	__MYUART_H

extern unsigned char g_r_Com3_txd_flag;
extern unsigned char g_r_com3_txd_point;
extern unsigned char g_r_burst_send_time;
extern unsigned char g_r_com3_rx_end_flag;
extern unsigned char g_r_CT_command;
extern unsigned char g_r_com3_rxd_point;

void USART1_Config(void);
void USART2_Config(void);
void USART3_Config(void);
void NVIC_Configuration(void);
void UARTx_Send_Str(USART_TypeDef* UARTx,u8 *s);
void UARTx_Send_Data(USART_TypeDef* UARTx,u8 *Dat,u8 Len);
void URAT1_NVIC_Configuration(void);
void URAT2_NVIC_Configuration(void);
void URAT3_NVIC_Configuration(void);
void CT_ACK_OK(void);
void CT_ACK_ERRO(void);
void COM1_FUNCTION(void);
//unsigned char Fiber_alive_check(unsigned char flag);

void COM1_CMD_FUNCTION(void);
void USART3_OFF(void);
void UART3_TXD_BUFF(void);
void UART3_TXD_Burst_information(void);
void Burst_information(void);
void ACK_COMMAND(unsigned char flag);
void SEND_Opction(unsigned char command);
void COM3_RXD_FUNCTION(void);
void Loading_CH(unsigned char flag);
void OPCTION_VCC_SW_SET(unsigned char flag);
void UARTx_Send_Float_5Char(USART_TypeDef* UARTx,float data);
void UARTx_Send_Int_4Char(USART_TypeDef* UARTx,unsigned int data);
//void Sprintf(USART_TypeDef* USARTx, unsigned char *Data_point);

#endif

