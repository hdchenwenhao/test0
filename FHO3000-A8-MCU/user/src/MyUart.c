#include "Include.h"
//******************************************************************
//函数名：  USART1_Config
//功能：    串口配置
//输入参数：无
//返回值：  无
//******************************************************************



#define COM3_RXD_BUFF_SIZE 50
#define COM3_TXD_BUFF_SIZE 50
#define COM1_RXD_BUFF_SIZE 50

unsigned char dBug_information_print_flag=1;

unsigned char COM3_RXD_BUFF[COM3_RXD_BUFF_SIZE];
unsigned char g_r_com3_rxd_point=0;
unsigned char g_r_com3_rx_end_flag=0;

unsigned char COM3_TXD_BUFF[COM3_TXD_BUFF_SIZE];
unsigned char g_r_com3_txd_point=0;
unsigned char g_r_com3_tx_end_flag=0;
unsigned char g_r_Com3_txd_flag=0;
unsigned char uart1_rx_buff[5];


unsigned char g_r_Brust_point=0;
unsigned char Brust_BUFF[20];
unsigned char g_r_burst_send_time=0;

void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 
    USART_Cmd(USART1, ENABLE);
		
		URAT1_NVIC_Configuration();//rzg20160514
        
        /* 使能串口1接收中断 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
}
//******************************************************************
//函数名：  NVIC_UART1_Configuration
//功能：    配置USART1接收中断
//输入参数：无
//返回值：  无
//******************************************************************
void URAT1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//强制优先级设定
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
//******************************************************************
//函数名：  fputc
//功能：    重定向c库函数printf到USART1
//输入参数：ch 发送的数据
//返回值：  无
//******************************************************************
int fputc(int ch, FILE *f)
{
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		// 发送一个字节数据到USART1 
		USART_SendData(USART1, (uint8_t) ch);
		
		// 等待发送完毕 
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	
		return (ch);
}
//******************************************************************
//函数名：  fgetc
//功能：    重定向c库函数scanf到USART1
//输入参数：*f 获取文件流
//返回值：  无
//******************************************************************
int fgetc(FILE *f)
{
		// 等待串口1输入数据 
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
        
}
//******************************************************************
//函数名：  UARTx_Send_Str
//功能：    任意串口发送数据
//输入参数：UARTx 串口号 *s 发送字符串
//返回值：  无
//******************************************************************
void UARTx_Send_Str(USART_TypeDef* UARTx,u8 *s)
{
        u8 i=0;  

        while(s[i]!='\0')  
        {
            while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
            USART_SendData(UARTx,s[i]);       
            while( USART_GetFlagStatus(UARTx,USART_FLAG_TC)!= SET);  
            i++;               
        }
}
void UARTx_Send_Float_5Char(USART_TypeDef* UARTx,float data)
{
	unsigned long data_int=0;
	unsigned char Char_data=0;
	if(data<0)
	{
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,'-');  
	}
	data=fabs(data*1000);
	data_int=data;
	data_int=data_int/10;
	if(data_int>=1000)
	{
		Char_data=data_int/1000;
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,Char_data+0x30);  
	}
//	if(data_int>=100)
	{
		Char_data=data_int/100%10;
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,Char_data+0x30);  
	}
	while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
	USART_SendData(UARTx,'.');  
	Char_data=data_int/10%10;
	while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
	USART_SendData(UARTx,Char_data+0x30);  
	Char_data=data_int%10;
	while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
	USART_SendData(UARTx,Char_data+0x30);  
}

void UARTx_Send_Int_4Char(USART_TypeDef* UARTx,unsigned int data)
{
	unsigned char Char_data=0;
	if(data>=1000)
	{
		Char_data=data/1000;
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,Char_data+0x30);  
	}
	if(data>=100)
	{
		Char_data=data/100%10;
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,Char_data+0x30);  
	}
	if(data>=10)
	{
		Char_data=data/10%10;
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,Char_data+0x30);  
	}
	  Char_data=data%10;
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,Char_data+0x30);
}
//******************************************************************
//函数名：  UARTx_Send_Data
//功能：    串口发送数据
//输入参数：USART_TypeDef* UARTx：需要使用的串口号
//          Dat：待发送的数据
//返回值：  无
//******************************************************************
void UARTx_Send_Data(USART_TypeDef* UARTx,u8 *Dat,u8 Len)
{
	u8 i = 0;
	for(i = 0;i < Len;i++)
	{
		while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET);
		USART_SendData(UARTx,Dat[i]);       
		while( USART_GetFlagStatus(UARTx,USART_FLAG_TC)!= SET);  
	}
}

unsigned char uart_tx_point=0;
unsigned char g_r_CT_command=0;
unsigned char Uart1_rxd_buff[COM1_RXD_BUFF_SIZE];
unsigned char g_r_uart1_rxd_buff_point=0;
/*
void USART1_IRQHandler(void)
{	
	u8 UART1_RxBuffer;
    if(USART_GetFlagStatus(USART1,USART_IT_RXNE)==SET)//接收中断产生 
    {        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清楚中断标志
        UART1_RxBuffer = USART_ReceiveData(USART1);//读取接收数据 
			  uart1_rx_buff[2]=uart1_rx_buff[1];
		  	uart1_rx_buff[1]=uart1_rx_buff[0];
			  uart1_rx_buff[0]=UART1_RxBuffer;
			  if(uart1_rx_buff[2]=='C'&&uart1_rx_buff[1]=='T'&&uart1_rx_buff[0]=='+')
				{
					g_r_CT_command=1;
					g_r_uart1_rxd_buff_point=0;
					Uart1_rxd_buff[g_r_uart1_rxd_buff_point++]='C';
					Uart1_rxd_buff[g_r_uart1_rxd_buff_point++]='T';
					Uart1_rxd_buff[g_r_uart1_rxd_buff_point++]='+';
					return;
				}
				if(uart1_rx_buff[1]=='\r'&&uart1_rx_buff[0]=='\n'&&g_r_CT_command==1)
				{
						Uart1_rxd_buff[g_r_uart1_rxd_buff_point++]='\n';
						g_r_CT_command=2;
				}
				else
				{
					if(g_r_CT_command)
					{
					  Uart1_rxd_buff[g_r_uart1_rxd_buff_point++]=uart1_rx_buff[0];
					}
					else  //系列号设置
					{
						if(g_r_uart4_rx_end!=2)
						{
							if(UART1_RxBuffer==0X0D)
							{
								g_r_uart4_rx_end=1;
							}
							else
							{
								if(g_r_uart4_rx_end==1)
								{
									if(UART1_RxBuffer==0X0A)
									{
										g_r_uart4_rx_end=2;
										g_r_uartx_tybe=1;
									}
									else g_r_uart4_rx_end=0;
								}
							}
							 UART4_COM_RX_BUFF[g_r_uart4_rx_point++]=UART1_RxBuffer;
							if(g_r_uart4_rx_point>=30)
							{
								g_r_uart4_rx_point=0;
							}
						} 
					}
					
				}
    }

}
*/
void CT_ACK_OK(void)
{
	printf("CT+OK\r\n");
}
void CT_ACK_ERRO(void)
{
	printf("CT+ERRO\r\n");
}

void CT_ACK_OK_COM3(void)
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'C');   
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'T');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'+');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'O');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'K');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'\r');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'\n');  
}
void CT_ACK_ERRO_COM3(void)
{
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'C');   
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'T');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'+');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'E');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'R');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'R');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'O');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'\r');  
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3,'\n');
}
//void Sprintf(USART_TypeDef* USARTx, unsigned char *Data_point)
//{
//	unsigned char i=0;
//	unsigned char data=0;
//	while(1)
//	{
//		while(*Data_point!='\0')
//		{
//			while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
//			USART_SendData(USARTx,*Data_point);
//		}
//		i++;
//		if(i>50)break;
//	}
//	
//}

void COM1_FUNCTION(void)
{
	unsigned char flag=0;
	unsigned long set_value_INT=0;
	float set_value_FLOAT=0;
	if(g_r_CT_command==2)
	{
	  if(Uart1_rxd_buff[3]=='O'&&Uart1_rxd_buff[4]=='P'&&Uart1_rxd_buff[5]=='M')//功率计相关操作
		{
			if(Uart1_rxd_buff[6]=='S'&&Uart1_rxd_buff[7]=='E'&&Uart1_rxd_buff[8]=='T')
			{
				if(Uart1_rxd_buff[9]=='C')
				{
					OPM_VCC_CTL(1);
					g_r_opm_set_flag=1;//进入档位校准模式
					g_r_dBm_dB_wM_flag=dBm;
					g_r_power_meter_wave_lenght=4;//校准波长为1550
					CT_ACK_OK();
					if(dBug_information_print_flag)
					{
						printf("进入功率计档位校准模式：波长：1550nm\r\n");
					}
			  }
				else if(Uart1_rxd_buff[9]=='W')
				{
					OPM_VCC_CTL(1);
					g_r_opm_set_flag=2;//进入校准模式
					g_r_dBm_dB_wM_flag=dBm;
					g_r_power_meter_wave_lenght=0;//校准波长为1310
					CT_ACK_OK();
					if(dBug_information_print_flag)
					{
						printf("进入功率计波长校准模式：波长：850nm\r\n");
					}
				}
				else if(Uart1_rxd_buff[9]=='M')
				{
					OPM_VCC_CTL(0);
					g_r_opm_set_flag=0;//固化校准值,并推出校准模式
					CT_ACK_OK();
					
					if(dBug_information_print_flag)
					{
						printf("固化校准值，退出校准模式\r\n");
					}
						BUZZ_CTL(1);
						save_cal_constant();
						Save_wavelength_coeff();
						delay_ms(500);
						BUZZ_CTL(0);
						
					
				}
				else if(Uart1_rxd_buff[9]=='R')
				{
					g_r_opm_set_flag=0;
					g_r_dBm_dB_wM_flag=dBm;
					g_r_power_meter_wave_lenght=4;//
					CT_ACK_OK();
					reset_cal_constant();
					if(dBug_information_print_flag)
					{
						printf("功率校准值复位!!!\r\n");
					}
				}
				else CT_ACK_ERRO();
				return;
			}
			else if(Uart1_rxd_buff[6]=='O'&&Uart1_rxd_buff[7]=='U'&&Uart1_rxd_buff[8]=='T')
			{
				OPM_VCC_CTL(0);
				g_r_opm_set_flag=0;//退出校准模式
				CT_ACK_OK();
				if(dBug_information_print_flag)
				{
					printf("退出功率计校准模式\r\n");
				}
				return;
			}
			if(g_r_opm_set_flag)
			{
				flag=0;
				if(g_r_opm_set_flag==1)//档位校准
				{
					if(Uart1_rxd_buff[6]=='S'&&Uart1_rxd_buff[7]=='4')
			    {
						 if(Uart1_rxd_buff[8]>='0'&&Uart1_rxd_buff[8]<='6')
						 {
							 g_r_opm_set_channel_no=Uart1_rxd_buff[8]-0x30;
							 if((Uart1_rxd_buff[9]=='0')||(Uart1_rxd_buff[9]=='1'))
							 {
								 if(Uart1_rxd_buff[10]=='+')
								 {
									 set_value_INT=(Uart1_rxd_buff[11]-0x30);
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[12]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[13]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[14]-0x30;
									 set_value_FLOAT=set_value_INT;
									 set_value_FLOAT=set_value_FLOAT/100;
									 flag=1;
								 }
								 else if(Uart1_rxd_buff[10]=='-')
								 {
									 set_value_INT=(Uart1_rxd_buff[11]-0x30);
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[12]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[13]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[14]-0x30;
									 set_value_FLOAT=set_value_INT;
									 set_value_FLOAT=0-set_value_FLOAT/100;
									 flag=1;
								 }
							 }
						 }
					}
					if(flag==1)
						{
							cal_power[g_r_opm_set_channel_no*2+Uart1_rxd_buff[9]-0x30]=pow(10,set_value_FLOAT/10);
							cal_adc[g_r_opm_set_channel_no*2+Uart1_rxd_buff[9]-0x30]=g_r_opm_adc_value;
							CT_ACK_OK();
						}
						else CT_ACK_ERRO();
				}
				else if(g_r_opm_set_flag==2)//波长校准
				{
					if(Uart1_rxd_buff[6]=='S')
					{
						if(Uart1_rxd_buff[7]>='0'&&Uart1_rxd_buff[7]<='6')
						{
							  if(Uart1_rxd_buff[8]=='+')
								 {
									 set_value_INT=(Uart1_rxd_buff[9]-0x30);
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[10]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[11]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[12]-0x30;
									 set_value_FLOAT=set_value_INT;
									 set_value_FLOAT=set_value_FLOAT/100;
									 flag=1;
								 }
								 else if(Uart1_rxd_buff[8]=='-')
								 {
									 set_value_INT=(Uart1_rxd_buff[9]-0x30);
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[10]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[11]-0x30;
									 set_value_INT=set_value_INT*10+Uart1_rxd_buff[12]-0x30;
									 set_value_FLOAT=set_value_INT;
									 set_value_FLOAT=0-set_value_FLOAT/100;
									 flag=1;
								 }
								 g_r_power_meter_wave_lenght=Uart1_rxd_buff[7]-0x30;
								 OPM_TEST();
								 set_value_FLOAT=set_value_FLOAT-g_r_dB_Power_meter;
								 set_value_FLOAT=pow(10,set_value_FLOAT/10);
								 set_value_FLOAT=set_value_FLOAT*wavelength_coeff[g_r_power_meter_wave_lenght];
								 wavelength_coeff[g_r_power_meter_wave_lenght]=set_value_FLOAT;
						}
						else  if(Uart1_rxd_buff[7]>='N'||Uart1_rxd_buff[7]=='L')
						{
							if(Uart1_rxd_buff[7]>='N')
							{
								g_r_power_meter_wave_lenght++;
								if(g_r_power_meter_wave_lenght>6)g_r_power_meter_wave_lenght=0;
							}
							else 
							{
								if(g_r_power_meter_wave_lenght>0)g_r_power_meter_wave_lenght--;
								else g_r_power_meter_wave_lenght=6;
							}
						}
						else CT_ACK_ERRO();
					}
				}
				
				
			}
			
		}
	}
	g_r_CT_command=0;
}

void USART2_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* config USART1 clock */
   // RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB2Periph_GPIOA, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB2Periph_GPIOA, ENABLE);
    
    /* USART2 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure); 
    USART_Cmd(USART2, ENABLE);
        
        /* 使能串口1接收中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
}
//******************************************************************
//函数名：  NVIC_UART1_Configuration
//功能：    配置USART1接收中断
//输入参数：无
//返回值：  无
//******************************************************************
void URAT2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//强制优先级设定
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
//串口中断函数
void USART2_IRQHandler(void)
{
	unsigned char UART2_RxBuffer=0;
	 if(USART_GetFlagStatus(USART2,USART_IT_RXNE)==SET)//接收中断产生 
    {        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);//清楚中断标志
        UART2_RxBuffer = USART_ReceiveData(USART2);//读取接收数据 
        //USART_SendData(USART2,UART2_RxBuffer);//发送接收到的数据
    }
		
}

void USART3_OFF(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10|GPIO_Pin_11);
}

void USART3_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* config USART1 clock */
   // RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB2Periph_GPIOB, ENABLE);
	  
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
        
    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 19200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure); 
    USART_Cmd(USART3, ENABLE);
		
		URAT3_NVIC_Configuration();//rzg20160514
        
        /* 使能串口1接收中断 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
	USART_Cmd(USART3, ENABLE);
}
//******************************************************************
//函数名：  NVIC_UART1_Configuration
//功能：    配置USART1接收中断
//输入参数：无
//返回值：  无
//******************************************************************
void URAT3_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//强制优先级设定
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
//串口中断函数

void USART3_IRQHandler(void)
{
	unsigned char UART3_RxBuffer=0;
	 if(USART_GetFlagStatus(USART3,USART_IT_RXNE)==SET)//接收中断产生 
    {        
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);//清楚中断标志
        UART3_RxBuffer = USART_ReceiveData(USART3);//读取接收数据 
       // USART_SendData(USART1,UART3_RxBuffer);//发送接收到的数据
			if(g_r_com3_rx_end_flag!=ON&&g_r_Com3_txd_flag==0)
			{
				COM3_RXD_BUFF[g_r_com3_rxd_point++]=UART3_RxBuffer;
				if(g_r_com3_rxd_point>=COM3_RXD_BUFF_SIZE)
					g_r_com3_rxd_point=0;
				if(UART3_RxBuffer==0x0D)
					g_r_com3_rx_end_flag=1;
				else if(UART3_RxBuffer==0x0A&&g_r_com3_rx_end_flag==1)
					g_r_com3_rx_end_flag=ON;//接收到结束符
				else 
					g_r_com3_rx_end_flag=0;
			}
		}
}

void COM3_RXD_FUNCTION(void)
{
	unsigned char i=0;
	unsigned char COMMAND_TYPE=0;
	unsigned char COMMAND=0;
	unsigned char Frame_Longer=0;
	unsigned char point_start=0;
	unsigned char send_check_sum=0;
	
	unsigned char tx_flag=0;
  unsigned long SET_BUFF=0;
  unsigned char SET_X1,SET_X2;
	
	unsigned char flag=0;
	unsigned long set_value_INT=0;
	float set_value_FLOAT=0;
	
	for(i=0;i<g_r_com3_rxd_point-2;i++)
	{
		if(COM3_RXD_BUFF[i]=='B'&&COM3_RXD_BUFF[i+1]=='O'&&COM3_RXD_BUFF[i+2]=='O'&&COM3_RXD_BUFF[i+3]=='T'
			&&COM3_RXD_BUFF[i+4]=='S'&&COM3_RXD_BUFF[i+5]=='E'&&COM3_RXD_BUFF[i+6]=='T')
		{
			//升级指令 42 4F 4F 54 53 45 54 0D 0A
			g_r_A8_BOOT_Loading_flag=ON;
			break;
		}
		if(COM3_RXD_BUFF[i]==0X21&&COM3_RXD_BUFF[i+1]==0x02)//电池电量等指令
		{
			COMMAND_TYPE=1;
			point_start=i+3;
			break;
		}
		if(COM3_RXD_BUFF[i]==0X64&&COM3_RXD_BUFF[i+1]==0x10)//光功率计等指令
		{
			COMMAND_TYPE=2;
			point_start=i+3;
			break;
		}
		//CT+指令 
		if(COM3_RXD_BUFF[i]=='C'&&COM3_RXD_BUFF[i+1]=='T'&&COM3_RXD_BUFF[i+2]=='+')
		{
			COMMAND_TYPE=3;
			g_r_CT_command=3;
			point_start=i;
		}
		if(COM3_RXD_BUFF[i]=='L'&&COM3_RXD_BUFF[i+1]=='S'&&COM3_RXD_BUFF[i+2]==':')
		{
			COMMAND_TYPE=4;
			point_start=i+2;
		}
	}
	if(COMMAND_TYPE==1)//电池管理等指令
	{
		if(g_r_com3_rxd_point>=point_start+3)
		{
			COMMAND=COM3_RXD_BUFF[point_start];
			switch(COMMAND)
			{
				case 0XBF:
					break;
				case 0XFE:
				{
					g_r_com3_txd_point=0;
					Frame_Longer=22;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0x64;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0x10;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=Frame_Longer;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=COMMAND;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_dc_in_flag%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_dc_in_flag/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_charge_status%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_charge_status/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Battery_status%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Battery_status/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Battery_capacity%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Battery_capacity/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_voltage%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_voltage/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Battery_Tem%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Battery_Tem/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_AverageTimeToFull%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_AverageTimeToFull/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_AverageTimeToEmpty%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_AverageTimeToEmpty/256;
					
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Percentage%256;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=g_r_Percentage/256;
					
					for(i=3;i<g_r_com3_txd_point;i++)
					{
						send_check_sum=send_check_sum+COM3_TXD_BUFF[i];
					}
					COM3_TXD_BUFF[g_r_com3_txd_point++]=send_check_sum;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0x0D;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0X0A;
					g_r_Com3_txd_flag=1;
					break;
				}
				case 0xA0://关机应答
				{
					delay_ms(2000);
					g_r_power_flag=1; // 进入关机倒计时
					BUZZ_CTL_TIME(1000);
					
					break;
				}
				case 0xA1://外接电源插入应答
				{
					delay_ms(2000);
				
					BUZZ_CTL_TIME(1000);
					
					break;
				}
				case 0xA2://外接电源拔出应答
				{
					delay_ms(2000);
				
					BUZZ_CTL_TIME(1000);
					
					break;
				}
				case 0XEE://读取MCU程序版本号
				{ 					
					g_r_com3_txd_point=0;
					Frame_Longer=14;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0x64;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0x10;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=Frame_Longer;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=COMMAND;
					for(i=0;i<10;i++)
					{
						COM3_TXD_BUFF[g_r_com3_txd_point++]=Version_BUFF[i];
					}
					for(i=3;i<g_r_com3_txd_point;i++)
					{
							send_check_sum=send_check_sum+COM3_TXD_BUFF[i];
					}
					COM3_TXD_BUFF[g_r_com3_txd_point++]=send_check_sum;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0x0D;
					COM3_TXD_BUFF[g_r_com3_txd_point++]=0X0A;
					g_r_Com3_txd_flag=1;
					break;
				}
				case 0XC8://机器复位
				{
					delay_ms(500);
					BUZZ_CTL_TIME(1000);
					CPU_POWER_CTL(OFF);
					delay_ms(500);
					CPU_POWER_CTL(ON);
				}
			}
		}
	}
		else if(COMMAND_TYPE==2)//功率计操作相关指令
		{
			  COMMAND=COM3_RXD_BUFF[2];
				switch(COMMAND)
				{
				case 0XA1://读取功率值
					{
						if((g_r_power_meter_work_on&0x01)==0)
						{
							g_r_power_meter_work_on|=0x01;
							OPCTION_VCC_SW_SET(g_r_power_meter_work_on);
						}
						OPM_VCC_CTL(1);//打开光功率计电源
						tx_flag=1;
						break;
					}
				case 0XA2://设置指令
					{
						if((g_r_power_meter_work_on&0x01)==0)
						{
							g_r_power_meter_work_on|=0x01;
							OPCTION_VCC_SW_SET(g_r_power_meter_work_on);
						}
						SET_X1=COM3_RXD_BUFF[3];
						SET_X2=COM3_RXD_BUFF[4];
						if(SET_X1<=L1650)
						{
							g_r_power_meter_wave_lenght=SET_X1;
							g_r_X6&=~(0x0f);
							g_r_X6|=(SET_X1&0X0F);
						}
						if(SET_X2<=TWIN_MODE)
						{
							g_r_opction_work_mode=SET_X2;
							if(g_r_opction_work_mode==TWIN_MODE)
							{
								g_r_X6&=~(0xf0);
								g_r_X6|=0x10;
							}
							else  g_r_X6&=~(0xf0);
						}
						tx_flag=1;
						break;
					}
				 case 0XA3://设置指令
						{
							SET_X1=COM3_RXD_BUFF[3];
							g_r_power_meter_work_on=SET_X1;
							OPCTION_VCC_SW_SET(g_r_power_meter_work_on);//????,???,?PD???
					//		P2OUT|=BIT1;
							tx_flag=1;
							break;
						}
				case 0XA8://??????,??PD,?????????
					{
//						g_r_Special_opction_flag=ON;
//						g_r_Special_Time_count=0;
//						g_r_Special_opction_status=0;
//						for(i=0;i<2;i++)
//						{
//							for(j=0;j<5;j++)Special_Opction_Buff[i][j]=0.0;
//						}
//						
//						TB0R=0;//????,????
//						tx_flag=1;
						break;
					}
				case 0XA4:
					{
//						SET_X1=COM0_RX_BUF[3];
//						if(SET_X1==0)tx_flag=2;
//						else
//						{
//							SET_BUFF=COM0_RX_BUF[4];
//							SET_BUFF=SET_BUFF*256+COM0_RX_BUF[5];
//							SET_BUFF=SET_BUFF*256+COM0_RX_BUF[6];
//							SET_BUFF=SET_BUFF*256+COM0_RX_BUF[7];
//							if(SET_BUFF>=100000)tx_flag=4;
//							else 
//							{
//								g_r_opsw_change_count=SET_BUFF;
//								tx_flag=3;
//								
//							}
//						}
//						
						break;
					}
				case 0XB0:
					{
						SET_X1=COM3_RXD_BUFF[3];
						if(SET_X1>='1')
						{
							SET_X1=SET_X1-0x30;
							if(SET_X1<=10)
							{
								SET_BUFF=SET_X1;
								SET_BUFF=SET_BUFF*50;
								BUZZ_CTL_TIME(SET_BUFF);
							}
								
						}
						break;
					}
				case 0XB2:
					{
						SET_X1=COM3_RXD_BUFF[3];
						if(SET_X1=='1')
						{
							LCD_BL_CTL(ON);
						}
						else if(SET_X1=='0')
						{
							LCD_BL_CTL(OFF);
						}
						break;
					}
				case 0XB4:
					{
						SET_X1=COM3_RXD_BUFF[3];
						if(SET_X1=='2')
						{
							g_r_VFL_status=2;
						}
						else if(SET_X1=='1')
						{
							g_r_VFL_status=1;
						}
						else if(SET_X1=='0')
						{
							g_r_VFL_status=0;
						}
						
						if(g_r_VFL_status==1)
						 {
							 LED_VFL_RED_CTL(1);
							 VFL_CTL(1);
						 }
						 else if(g_r_VFL_status==0)
						 {
							 LED_VFL_RED_CTL(0);
							 VFL_CTL(0);
						 }
						 if(g_r_VFL_status==2)
							{
								Timer_start(1);
							}
							else
							{
								Timer_start(0);
							}
						break;
					}
				default:break;
				}
				if(tx_flag==1)//??????,??????
				{
					//if(g_r_Special_opction_flag==ON)SEND_Opction(0XA8);
					
					SEND_Opction(0XAA);
				}
				else if(tx_flag==2||tx_flag==3||tx_flag==4)
				{
					ACK_COMMAND(tx_flag);
				}
	}
		else if(COMMAND_TYPE==3)
		{

			if(g_r_CT_command==3)
			{
					if(COM3_RXD_BUFF[point_start+3]=='O'&&COM3_RXD_BUFF[point_start+4]=='P'&&COM3_RXD_BUFF[point_start+5]=='M')//功率计相关操作
					{
						if(COM3_RXD_BUFF[point_start+6]=='S'&&COM3_RXD_BUFF[point_start+7]=='E'&&COM3_RXD_BUFF[point_start+8]=='T')
						{
							if(COM3_RXD_BUFF[point_start+9]=='C')
							{
								OPM_VCC_CTL(1);
								g_r_opm_set_flag=1;//进入档位校准模式
								g_r_dBm_dB_wM_flag=dBm;
								g_r_power_meter_wave_lenght=4;//校准波长为1550
								CT_ACK_OK_COM3();
								if(dBug_information_print_flag)
								{
									UARTx_Send_Str(USART3,"进入功率计档位校准模式：波长：1550nm\r\n");
									//printf("进入功率计档位校准模式：波长：1550nm\r\n");
								}
							}
							else if(COM3_RXD_BUFF[point_start+9]=='W')
							{
								OPM_VCC_CTL(1);
								g_r_opm_set_flag=2;//进入校准模式
								g_r_dBm_dB_wM_flag=dBm;
								g_r_power_meter_wave_lenght=0;//校准波长为1310
								CT_ACK_OK_COM3();
								if(dBug_information_print_flag)
								{
									UARTx_Send_Str(USART3,"进入功率计波长校准模式：波长：850nm\r\n");
								//	printf("进入功率计波长校准模式：波长：850nm\r\n");
								}
							}
							else if(COM3_RXD_BUFF[point_start+9]=='M')
							{
								OPM_VCC_CTL(0);
								g_r_opm_set_flag=0;//固化校准值,并推出校准模式
								CT_ACK_OK_COM3();
								
								if(dBug_information_print_flag)
								{
									UARTx_Send_Str(USART3,"固化校准值，退出校准模式\r\n");
									//printf("固化校准值，退出校准模式\r\n");
								}
									BUZZ_CTL(1);
									save_cal_constant();
									Save_wavelength_coeff();
									delay_ms(500);
									BUZZ_CTL(0);
									
								
							}
							else if(COM3_RXD_BUFF[point_start+9]=='R')
							{
								g_r_opm_set_flag=0;
								g_r_dBm_dB_wM_flag=dBm;
								g_r_power_meter_wave_lenght=4;//
								CT_ACK_OK_COM3();
								reset_cal_constant();
								if(dBug_information_print_flag)
								{
									UARTx_Send_Str(USART3,"功率校准值复位!!!\r\n");
								//	printf("功率校准值复位!!!\r\n");
								}
							}
							else CT_ACK_ERRO_COM3();
							return;
						}
						else if(COM3_RXD_BUFF[point_start+6]=='O'&&COM3_RXD_BUFF[point_start+7]=='U'&&COM3_RXD_BUFF[point_start+8]=='T')
						{
							OPM_VCC_CTL(0);
							g_r_opm_set_flag=0;//退出校准模式
							CT_ACK_OK_COM3();
							if(dBug_information_print_flag)
							{
								UARTx_Send_Str(USART3,"退出功率计校准模式\r\n");
								//printf("退出功率计校准模式\r\n");
							}
							return;
						}
						if(g_r_opm_set_flag)
						{
							flag=0;
							if(g_r_opm_set_flag==1)//档位校准
							{
								if(COM3_RXD_BUFF[point_start+6]=='S'&&COM3_RXD_BUFF[point_start+7]=='4')
								{
									 if(COM3_RXD_BUFF[point_start+8]>='0'&&COM3_RXD_BUFF[point_start+8]<='6')
									 {
										 g_r_opm_set_channel_no=COM3_RXD_BUFF[point_start+8]-0x30;
										 if((COM3_RXD_BUFF[point_start+9]=='0')||(COM3_RXD_BUFF[point_start+9]=='1'))
										 {
											 if(COM3_RXD_BUFF[point_start+10]=='+')
											 {
												 set_value_INT=(Uart1_rxd_buff[11]-0x30);
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+12]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+13]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+14]-0x30;
												 set_value_FLOAT=set_value_INT;
												 set_value_FLOAT=set_value_FLOAT/100;
												 flag=1;
											 }
											 else if(COM3_RXD_BUFF[point_start+10]=='-')
											 {
												 set_value_INT=(Uart1_rxd_buff[point_start+11]-0x30);
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+12]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+13]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+14]-0x30;
												 set_value_FLOAT=set_value_INT;
												 set_value_FLOAT=0-set_value_FLOAT/100;
												 flag=1;
											 }
										 }
									 }
								}
								if(flag==1)
									{
										cal_power[g_r_opm_set_channel_no*2+COM3_RXD_BUFF[point_start+9]-0x30]=pow(10,set_value_FLOAT/10);
										cal_adc[g_r_opm_set_channel_no*2+COM3_RXD_BUFF[point_start+9]-0x30]=g_r_opm_adc_value;
										CT_ACK_OK_COM3();
									}
									else CT_ACK_ERRO_COM3();
							}
							else if(g_r_opm_set_flag==2)//波长校准
							{
								if(COM3_RXD_BUFF[point_start+6]=='S')
								{
									if(COM3_RXD_BUFF[point_start+7]>='0'&&COM3_RXD_BUFF[point_start+7]<='6')
									{
											if(COM3_RXD_BUFF[point_start+8]=='+')
											 {
												 set_value_INT=(Uart1_rxd_buff[point_start+9]-0x30);
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+10]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+11]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+12]-0x30;
												 set_value_FLOAT=set_value_INT;
												 set_value_FLOAT=set_value_FLOAT/100;
												 flag=1;
											 }
											 else if(Uart1_rxd_buff[point_start+8]=='-')
											 {
												 set_value_INT=(Uart1_rxd_buff[point_start+9]-0x30);
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+10]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+11]-0x30;
												 set_value_INT=set_value_INT*10+COM3_RXD_BUFF[point_start+12]-0x30;
												 set_value_FLOAT=set_value_INT;
												 set_value_FLOAT=0-set_value_FLOAT/100;
												 flag=1;
											 }
											 g_r_power_meter_wave_lenght=COM3_RXD_BUFF[point_start+7]-0x30;
											 OPM_TEST();
											 set_value_FLOAT=set_value_FLOAT-g_r_dB_Power_meter;
											 set_value_FLOAT=pow(10,set_value_FLOAT/10);
											 set_value_FLOAT=set_value_FLOAT*wavelength_coeff[g_r_power_meter_wave_lenght];
											 wavelength_coeff[g_r_power_meter_wave_lenght]=set_value_FLOAT;
									}
									else  if(COM3_RXD_BUFF[point_start+7]>='N'||COM3_RXD_BUFF[point_start+7]=='L')
									{
										if(COM3_RXD_BUFF[point_start+7]>='N')
										{
											g_r_power_meter_wave_lenght++;
											if(g_r_power_meter_wave_lenght>6)g_r_power_meter_wave_lenght=0;
										}
										else 
										{
											if(g_r_power_meter_wave_lenght>0)g_r_power_meter_wave_lenght--;
											else g_r_power_meter_wave_lenght=6;
										}
									}
									else CT_ACK_ERRO_COM3();
								}
							}
							
							
						}
						
					}
				}
	  g_r_CT_command=0;
		}
		else if(COMMAND_TYPE==4)//光源操作
		{
			 if(COM3_RXD_BUFF[point_start]==0)
			 {
				 g_r_laser_souce_enable=0;
				 GPIO_ResetBits(GPIOC,GPIO_Pin_13)	;//关闭电源
				 Laser_souce_lamda_set(0);
				 Dac2_Set_Vol(0);
				 g_r_laser_souce_status=0;
			 }
			 else
			 {
				 flag=0;
				 if(COM3_RXD_BUFF[point_start+1]>0&&COM3_RXD_BUFF[point_start+1]<4)
				 {
					 set_value_INT=COM3_RXD_BUFF[point_start+3]*256+COM3_RXD_BUFF[point_start+4];
					 if(set_value_INT<4096)
					 {
						 flag=1;
						 g_r_laser_souce_enable=1;
				     GPIO_SetBits(GPIOC,GPIO_Pin_13)	;//打开电源
						 Laser_souce_lamda_set(COM3_RXD_BUFF[point_start+1]);
						 Dac2_Set_Vol(set_value_INT);
					   g_r_laser_souce_status=1;
						 CT_ACK_OK_COM3();
					 }
				 }
				 if(flag==0)
				 {
					 CT_ACK_ERRO_COM3();
				 }
			 }
		}
	g_r_com3_rx_end_flag=0;
}
/**************************************************************/
 //??????????????,??:??????????
 //?????????,???????,????????
 //???????????
 /**************************************************************/

#define Message_resend_time 1

 void Burst_information(void)
 {
 		 switch(g_r_burst_information_tybe)
		 {
		 	case 0XA1: //外电插入
			{
				 g_r_Brust_point=0;
				 Brust_BUFF[g_r_Brust_point++]=0X64;
				 Brust_BUFF[g_r_Brust_point++]=0X10;
				 Brust_BUFF[g_r_Brust_point++]=0X04;
				 Brust_BUFF[g_r_Brust_point++]=0XA1;
				 Brust_BUFF[g_r_Brust_point++]=0XA1;
				 Brust_BUFF[g_r_Brust_point++]=0X0D;
				 Brust_BUFF[g_r_Brust_point++]=0X0A;
         g_r_burst_send_time=Message_resend_time	;
				break;
			}
			case 0XA2:	//外电拔出
		  {
					g_r_Brust_point=0;
					Brust_BUFF[g_r_Brust_point++]=0X64;
					Brust_BUFF[g_r_Brust_point++]=0X10;
					Brust_BUFF[g_r_Brust_point++]=0X04;
					Brust_BUFF[g_r_Brust_point++]=0XA2;
					Brust_BUFF[g_r_Brust_point++]=0XA2;
					Brust_BUFF[g_r_Brust_point++]=0X0D;
					Brust_BUFF[g_r_Brust_point++]=0X0A; 
					g_r_burst_send_time=Message_resend_time;
					break;
			}
			case 0XA0:					// 电源关机请求
			{
					g_r_Brust_point=0;
					Brust_BUFF[g_r_Brust_point++]=0X64;
					Brust_BUFF[g_r_Brust_point++]=0X10;
					Brust_BUFF[g_r_Brust_point++]=0X04;
					Brust_BUFF[g_r_Brust_point++]=0XA0;
					Brust_BUFF[g_r_Brust_point++]=0XA0;
					Brust_BUFF[g_r_Brust_point++]=0X0D;
					Brust_BUFF[g_r_Brust_point++]=0X0A;
					g_r_burst_send_time=Message_resend_time	;
					break;
			}
			default:
			{
				 g_r_Brust_point=0;
				 g_r_burst_send_time=0	;
			}
		 }
 }
 
void UART3_TXD_BUFF(void)
{
	unsigned char i=0;
	unsigned char FLAG=0;
	for(i=0;i<COM3_TXD_BUFF_SIZE;i++)
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		USART_SendData(USART3,COM3_TXD_BUFF[i]);   
		while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET); 		
		if(COM3_TXD_BUFF[i]==0X0D)FLAG=1;
		else if(COM3_TXD_BUFF[i]==0X0A&&FLAG==1)
		break;
	}
}

void UART3_TXD_Burst_information(void)
{
	unsigned char i=0;
	unsigned char FLAG=0;
	for(i=0;i<20;i++)
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		USART_SendData(USART3,Brust_BUFF[i]);   
		while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET); 		
		if(Brust_BUFF[i]==0X0D)FLAG=1;
		else if(Brust_BUFF[i]==0X0A&&FLAG==1)break;
	}
}



/******************************************************/
//光功率计的通信
/******************************************************/

void ACK_COMMAND(unsigned char flag)
{
  unsigned char i=0;
  unsigned int CRC_buff=0;
    
    CRC_buff=0;
    g_r_POWER_METER_TX_BUFF[0]=0X50;
    g_r_POWER_METER_TX_BUFF[1]=0X4D;
    g_r_POWER_METER_TX_BUFF[2]=0xA4;  
    
  switch(flag)
  {
  case 2:
    {
      g_r_X5=0X00;
      break;
    }
  case 3:
    {
      g_r_X5=0X01;
      break;
    }
  case 4:
    {
       g_r_X5=0X11;
       break;
    }
  default:break;
  }
  if(flag==2||flag==3||flag==4)
  {
    g_r_X6=0;
   
      g_r_POWER_METER_TX_BUFF[3]=0x00;//g_r_opsw_change_count/256/256/256;
      g_r_POWER_METER_TX_BUFF[4]=0x00;//(g_r_opsw_change_count/256/256)%256;
      g_r_POWER_METER_TX_BUFF[5]=0x00;//(g_r_opsw_change_count/256)%256;
      g_r_POWER_METER_TX_BUFF[5]=0x00;//(g_r_opsw_change_count%256);
      g_r_POWER_METER_TX_BUFF[7]=g_r_X5;
      g_r_POWER_METER_TX_BUFF[8]=g_r_X6;
      g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-2]=0X0D;
      g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-1]=0X0A;
      
      for(i=0;i<POWER_METER_TX_BUFF_SIZE-4;i++)
      {
        CRC_buff=CRC_buff+g_r_POWER_METER_TX_BUFF[i];
      }
      g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-4]=CRC_buff/256;//?????
      g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-3]=CRC_buff%256;
      for(i=0;i<POWER_METER_TX_BUFF_SIZE;i++)//??????BUFF??
      {
				while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		    USART_SendData(USART3,g_r_POWER_METER_TX_BUFF[i]);   
		    while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET); 	
      }
  }
}

void SEND_Opction(unsigned char command)
{
    unsigned char i=0;
    unsigned int CRC_buff=0;
    
    CRC_buff=0;
    g_r_X6=(g_r_X6&0XF0);
    g_r_X6|=(g_r_power_meter_wave_lenght);
    g_r_POWER_METER_TX_BUFF[0]=0X50;
    g_r_POWER_METER_TX_BUFF[1]=0X4D;
    g_r_POWER_METER_TX_BUFF[2]=command;
    g_r_POWER_METER_TX_BUFF[7]=g_r_X5;
    g_r_POWER_METER_TX_BUFF[8]=g_r_X6;
    g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-2]=0X0D;
    g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-1]=0X0A;
    
    for(i=0;i<POWER_METER_TX_BUFF_SIZE-4;i++)
    {
      CRC_buff=CRC_buff+g_r_POWER_METER_TX_BUFF[i];
    }
    g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-4]=CRC_buff/256;//?????
    g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE-3]=CRC_buff%256;
    for(i=0;i<POWER_METER_TX_BUFF_SIZE;i++)//??????BUFF??
    {
			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			USART_SendData(USART3,g_r_POWER_METER_TX_BUFF[i]);   
		  while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET); 
    }
}

void OPCTION_VCC_SW_SET(unsigned char flag)
{
  unsigned char power_bit=0;
//  unsigned char pd_bit=0;
//  unsigned char opsw_bit=0;
  power_bit=flag&0x01;
 // pd_bit   =flag&0x02;
//  opsw_bit =flag&0x04;
  
  if(power_bit==0)//关闭光功率计电源
  {
    OPM_VCC_CTL(0);
  } 
  else 
	{
    OPM_VCC_CTL(1);//打开光功率计电源
		delay_ms(50);
		OPM_TEST();
	}
 
} 

