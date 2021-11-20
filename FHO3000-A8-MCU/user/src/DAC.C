#include "Include.h"

unsigned char UART4_COM_RX_BUFF[40];
unsigned char g_r_uart4_rx_point = 0;
unsigned char g_r_uart4_rx_end = 0;


unsigned char g_r_uartx_tybe = 0;
void SEND_TEM_TO_A8(USART_TypeDef* USARTx);
void SEND_SN_TO_A8(USART_TypeDef* USARTx);
void SEND_HW_TO_A8(USART_TypeDef* USARTx);
void REND_OK(USART_TypeDef* USARTx);
void REND_erro(USART_TypeDef* USARTx);
void SEND_VBR(USART_TypeDef* USARTx);



void USART4_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, DISABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11);
}

void USART4_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);
    USART_Cmd(UART4, ENABLE);

    URAT4_NVIC_Configuration();
    /* 使能串口4接收中断 */
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

    USART_Cmd(UART4, ENABLE);
}
//******************************************************************
//函数名：  NVIC_UART1_Configuration
//功能：    配置USART1接收中断
//输入参数：无
//返回值：  无
//******************************************************************
void URAT4_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//强制优先级设定
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void UART4_IRQHandler(void)
{
    u8 UART4_RxBuffer;
    if(USART_GetFlagStatus(UART4, USART_IT_RXNE) == SET) //接收中断产生
    {
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);//清楚中断标志
        UART4_RxBuffer = USART_ReceiveData(UART4);//读取接收数据
        // g_r_buzz_alarm_flag=1;
        if(g_r_uart4_rx_end != 2)
        {
            if(UART4_RxBuffer == 0x0D)
            {
                g_r_uart4_rx_end = 1;
            }
            else
            {
                if(g_r_uart4_rx_end == 1)
                {
                    if(UART4_RxBuffer == 0x0A)
                    {
                        g_r_uart4_rx_end = 2;
                        g_r_uartx_tybe = 4;
                    }
                    else g_r_uart4_rx_end = 0;
                }
            }
            UART4_COM_RX_BUFF[g_r_uart4_rx_point++] = UART4_RxBuffer;
            if(g_r_uart4_rx_point >= 30)
            {
                g_r_uart4_rx_point = 0;
            }
        }
    }

}
//0x44 0x41 0x3a xxxx 0X0D 0X0A
unsigned char g_r_dac_channel = 0;
void COM4_RXD_FUNCTION(void)
{
    unsigned char i = 0;
    unsigned char j = 0, k = 0;
    unsigned char flag = 0;
    unsigned char flag_error = 0;
    unsigned char flag_right1 = 0;
    unsigned char flag_right2 = 0;
    unsigned char dac_channel = 0;
    unsigned int dac_value_rxd = 0;
    for(i = 0; i < g_r_uart4_rx_point - 2; i++)
    {
        if(g_r_A8_BOOT_Loading_flag && g_r_uart4_rx_point >= 7)
        {
            if(UART4_COM_RX_BUFF[i] == 'B' && UART4_COM_RX_BUFF[i + 1] == 'O' && UART4_COM_RX_BUFF[i + 2] == 'O' && UART4_COM_RX_BUFF[i + 3] == 'T'
                    && UART4_COM_RX_BUFF[i + 4] == 'O' && UART4_COM_RX_BUFF[i + 5] == 'K' && UART4_COM_RX_BUFF[i + 6] == '\r' && UART4_COM_RX_BUFF[i + 7] == '\n')
            {
                //升级完成指令：42 4F 4F 54 4F 4B 0D 0A
                g_r_A8_BOOT_Loading_flag = 0;
            }
        }
        else
        {
            if(UART4_COM_RX_BUFF[i] == 'D' && UART4_COM_RX_BUFF[i + 1] == 'A' && UART4_COM_RX_BUFF[i + 3] == ':' && UART4_COM_RX_BUFF[i + 6] == 0x0D && UART4_COM_RX_BUFF[i + 7] == 0X0A)
            {
                if(UART4_COM_RX_BUFF[i + 2] == '1')
                {
                    dac_channel = 1;
                }
                else if(UART4_COM_RX_BUFF[i + 2] == '2')
                {
                    dac_channel = 2;
                }
                g_r_dac_channel = dac_channel;
                if(g_r_dac_channel)
                {
                    dac_value_rxd = UART4_COM_RX_BUFF[i + 4];
                    dac_value_rxd = dac_value_rxd * 256 + UART4_COM_RX_BUFF[i + 5];
                    if(dac_value_rxd < 4096)
                    {
                        if(g_r_dac_channel == 1)
                            DAC_SetChannel1Data(DAC_Align_12b_R, dac_value_rxd);
                        else if(g_r_dac_channel == 2)
                            DAC_SetChannel2Data(DAC_Align_12b_R, dac_value_rxd);
                    }
                }
                break;

            }
            else if(UART4_COM_RX_BUFF[i] == 'V' && UART4_COM_RX_BUFF[i + 1] == 'B' && UART4_COM_RX_BUFF[i + 2] == 'R' && UART4_COM_RX_BUFF[i + 3] == ':')
            {
                if(UART4_COM_RX_BUFF[i + 4] == '?' && UART4_COM_RX_BUFF[i + 5] == '\r' && UART4_COM_RX_BUFF[i + 6] == '\n')
                {
                    if(g_r_uartx_tybe == 1)
                        SEND_VBR(USART1);
                    else if(g_r_uartx_tybe == 4)
                        SEND_VBR(UART4);
                }
                else if(i + 9 < 40 && UART4_COM_RX_BUFF[i + 7] == '\r' && UART4_COM_RX_BUFF[i + 8] == '\n')
                {
                    if(UART4_COM_RX_BUFF[i + 4] >= '2' && UART4_COM_RX_BUFF[i + 4] <= '8')
                    {
                        if(UART4_COM_RX_BUFF[i + 5] >= '0' && UART4_COM_RX_BUFF[i + 5] <= '9')
                            if(UART4_COM_RX_BUFF[i + 6] >= '0' && UART4_COM_RX_BUFF[i + 6] <= '9')
                            {
                                g_r_APD_VBR = UART4_COM_RX_BUFF[i + 4] - 0x30;
                                g_r_APD_VBR = g_r_APD_VBR * 10 + UART4_COM_RX_BUFF[i + 5] - 0x30;
                                g_r_APD_VBR = g_r_APD_VBR * 10 + UART4_COM_RX_BUFF[i + 6] - 0x30;
                                g_r_APD_VBR = g_r_APD_VBR / 10;
                                if(g_r_uartx_tybe == 1)
                                    REND_OK(USART1);
                                else if(g_r_uartx_tybe == 4) REND_OK(UART4);
                                Write_set_status(5);
                                break;
                            }
                    }
                    if(g_r_uartx_tybe == 1)
                        REND_erro(USART1);
                    else if(g_r_uartx_tybe == 4)
                        REND_erro(UART4);
                    break;
                }
            }
            else if(UART4_COM_RX_BUFF[i] == 'T' && UART4_COM_RX_BUFF[i + 1] == 'E' && UART4_COM_RX_BUFF[i + 2] == 'M' && UART4_COM_RX_BUFF[i + 3] == ':' && UART4_COM_RX_BUFF[i + 4] == '?' && UART4_COM_RX_BUFF[i + 5] == 0x0D && UART4_COM_RX_BUFF[i + 6] == 0X0A)
            {
                if(g_r_uartx_tybe == 1)
                    SEND_TEM_TO_A8(USART1);
                else if(g_r_uartx_tybe == 4)
                    SEND_TEM_TO_A8(UART4);
                break;
            }
            else if(UART4_COM_RX_BUFF[i] == 'S' && UART4_COM_RX_BUFF[i + 1] == 'N' && UART4_COM_RX_BUFF[i + 2] == ':')
            {
                if(UART4_COM_RX_BUFF[i + 3] == '?' && UART4_COM_RX_BUFF[i + 4] == '\r' && UART4_COM_RX_BUFF[i + 5] == '\n')
                {
                    if(g_r_uartx_tybe == 1)
                    {
                        SEND_SN_TO_A8(USART1);
                    }
                    else if(g_r_uartx_tybe == 4)
                    {
                        SEND_SN_TO_A8(UART4);
                    }
                    break;
                }
                else
                {
                    for(j = 0; j < 19; j++)
                    {
                        if(UART4_COM_RX_BUFF[j + 3] == '\r' && UART4_COM_RX_BUFF[j + 4] == '\n')
                        {
                            for(k = 0; k < j; k++)
                            {
                                if((UART4_COM_RX_BUFF[k + 3] <= 'Z' && UART4_COM_RX_BUFF[k + 3] >= 'A') ||
                                        (UART4_COM_RX_BUFF[k + 3] <= '9' && UART4_COM_RX_BUFF[k + 3] >= '0') ||
                                        (UART4_COM_RX_BUFF[k + 3] <= 'z' && UART4_COM_RX_BUFF[k + 3] >= 'a') ||
                                        (UART4_COM_RX_BUFF[k + 3] == 0x0d) || (UART4_COM_RX_BUFF[k + 3] == 0x0A))
                                {
                                    g_r_SN[k] = UART4_COM_RX_BUFF[k + 3];
                                    g_r_SN_temp[k] = UART4_COM_RX_BUFF[k + 3];
                                    flag_right1 = 1; //接收数据符合设定规则
                                }
                                else
                                {
                                    g_r_SN[k] = '*';
                                    flag_error = 1; //接收数据不符合设定规则
                                }
                            }
                            g_r_SN[k++] = '\r';
                            g_r_SN[k++] = '\n';
                            k = k - 2;
                            g_r_SN_temp[k++] = '\r';
                            g_r_SN_temp[k++] = '\n';
														if((flag_error!=1)&&(flag_right1==1))
                            {
															Write_set_status(2);//向EEROM写入序列号
														}
                            Load_set_status();//从EEROM中读取序列号
                            for(k = 0; k < 19; k++) //对比读取的序列号和串口接收到的数据是否一致
                            {
                                if(g_r_SN_temp[k] == g_r_SN[k])
                                {
                                    flag_right2 = 1; //数据一致
                                }
                                else if(g_r_SN_temp[k] != g_r_SN[k])
                                {
                                    flag_right2 = 0; //数据不一致
                                }
                            }
                            break;
                        }
                    }
                    if(flag_right2 == 0 || flag_error == 1) //读取数据和接收数据不一致或者接收数据不符合规则，发送错误ERROR
                    {
                        if(g_r_uartx_tybe == 1)
                        {
                            REND_erro(USART1);
                        }
                        else if(g_r_uartx_tybe == 4)
                        {
                            REND_erro(UART4);
                        }
                        flag_error = 0;
                        flag_right2 = 0;
                        break;
                    }

                    if(flag_right1 == 1 && flag_right2 == 1) //读取数据和接收数据一致且接收数据符合规则，发送正确OK
                    {
                        if(g_r_uartx_tybe == 1)
                        {
                            REND_OK(USART1);
                        }
                        else if(g_r_uartx_tybe == 4)
                        {
                            REND_OK(UART4);
                        }
                        flag_right1 = 0;
                        flag_right2 = 0;
                        break;
                    }
                }
            }
            else if(UART4_COM_RX_BUFF[i] == 'H' && UART4_COM_RX_BUFF[i + 1] == 'W' && UART4_COM_RX_BUFF[i + 2] == ':')
            {
                if(UART4_COM_RX_BUFF[i + 3] == '?' && UART4_COM_RX_BUFF[i + 4] == '\r' && UART4_COM_RX_BUFF[i + 5] == '\n')
                {
                    if(g_r_uartx_tybe == 1)
                        SEND_HW_TO_A8(USART1);
                    else if(g_r_uartx_tybe == 4) SEND_HW_TO_A8(UART4);
                    break;
                }
                else
                {
                    for(j = 0; j < 19; j++)
                    {
                        if(UART4_COM_RX_BUFF[j + 4] == '\r' && UART4_COM_RX_BUFF[j + 5] == '\n')
                        {
                            if(g_r_uartx_tybe == 1)
                                REND_OK(USART1);
                            else if(g_r_uartx_tybe == 4) REND_OK(UART4);
                            //	j=j+1;
                            for(k = 0; k <= j; k++)
                            {
                                g_r_HardWare_TYPE[k] = UART4_COM_RX_BUFF[k + 3];
                            }
                            g_r_HardWare_TYPE[k++] = '\r';
                            g_r_HardWare_TYPE[k++] = '\n';
                            Write_set_status(4);
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0)
                    {
                        if(g_r_uartx_tybe == 1)
                            REND_erro(USART1);
                        else if(g_r_uartx_tybe == 4)REND_erro(UART4);
                        break;
                    }
                }
            }
        }
    }
}
void SEND_SN_TO_A8(USART_TypeDef* USARTx)
{
    unsigned char i = 0;
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'S');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'N');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, ':');
    for(i = 0; i < 19; i++)
    {
        if(g_r_SN[i] == '\r' && g_r_SN[i + 1] == '\n')
        {

            break;
        }
        else
        {
            while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
            USART_SendData(USARTx, g_r_SN[i]);
        }
    }
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\r');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\n');
}

void SEND_HW_TO_A8(USART_TypeDef* USARTx)
{
    unsigned char i = 0;
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'H');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'W');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, ':');
    for(i = 0; i < 19; i++)
    {
        if(g_r_HardWare_TYPE[i] == '\r' && g_r_HardWare_TYPE[i + 1] == '\n')
        {
            break;
        }
        else
        {
            while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
            USART_SendData(USARTx, g_r_HardWare_TYPE[i]);
        }
    }
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\r');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\n');
}


void SEND_TEM_TO_A8(USART_TypeDef* USARTx)
{
    unsigned char *Float_p;
    unsigned char i = 0;
    Float_p = (unsigned char *)&g_r_temp_pcb;
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'T');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'E');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'M');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, ':');
    for(i = 0; i < 4; i++)
    {
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
        USART_SendData(USARTx, *Float_p++);
    }
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\r');
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\n');
}

void REND_OK(USART_TypeDef* USARTx)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'O');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'K');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\r');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\n');
}
void REND_erro(USART_TypeDef* USARTx)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'E');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'R');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'R');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'O');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\r');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\n');
}

void SEND_VBR(USART_TypeDef* USARTx)
{
    float F_VBR = g_r_APD_VBR * 100;
    unsigned int VBR = F_VBR;
    VBR = VBR / 10;
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'V');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'B');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, 'R');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, ':');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, VBR / 256);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, VBR % 256);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\r');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    USART_SendData(USARTx, '\n');
}

void Dac1_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //??PORTA????
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //??DAC????

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // ????
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4)	; //PA.4 ???

    DAC_InitType.DAC_Trigger = DAC_Trigger_None;	//??????? TEN1=0
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None; //???????
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //???????
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Enable ;	//SJ  DAC_OutputBuffer_Disable
    DAC_Init(DAC_Channel_1, &DAC_InitType);	 //???DAC??1

    DAC_Cmd(DAC_Channel_1, ENABLE);  //??DAC1
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12??????????DAC?

}

//????1????
//vol:0~3300,??0~3.3V
void Dac1_Set_Vol(unsigned int  vol)
{
    float temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;
    DAC_SetChannel1Data(DAC_Align_12b_R, temp); //12??????????DAC?
}



void Dac2_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //??PORTA????
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //??DAC????

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 // ????
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_5)	; //PA.4 ???

    DAC_InitType.DAC_Trigger = DAC_Trigger_None;	//??????? TEN1=0
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None; //???????
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //???????
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;	//DAC1?????? BOFF1=1
    DAC_Init(DAC_Channel_2, &DAC_InitType);	 //???DAC??1

    DAC_Cmd(DAC_Channel_2, ENABLE);  //??DAC1

    DAC_SetChannel2Data(DAC_Align_12b_R, 0);  //12??????????DAC?

    DAC_DualSoftwareTriggerCmd(ENABLE);

}

//????1????
//vol:0~3300,??0~3.3V
void Dac2_Set_Vol(unsigned int vol)
{
//	float temp=vol;
//	temp/=1000;
//	temp=temp*4096/3.3;
    DAC_SetChannel2Data(DAC_Align_12b_R, vol); //12??????????DAC?
}

void Apd_voltage_set(float apd_voltage, float k)
{
    //0---->60.82    3298--->27.48
    float dac_vol = 0;
    int temp = 0;
    float adp_voltage_hight = 60.82;
    float adp_voltage_low = 27.48;
    float dac_hight = 3298;
    float dac_low = 0;
    if(apd_voltage >= 60.82)apd_voltage = 60.82;
    if(apd_voltage <= 27.48)apd_voltage = 27.48;

    apd_voltage = apd_voltage * k;

    dac_vol = (apd_voltage - adp_voltage_hight) / ((adp_voltage_low - adp_voltage_hight) / (dac_hight - dac_low));
    temp = dac_vol;
    Dac1_Set_Vol(temp);
}

void Laser_souce_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);//开启IO时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);//开启IO时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Laser_souce_lamda_set(unsigned char lamda)
{
    switch(lamda)
    {
    case 0:
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_7)	;
        GPIO_ResetBits(GPIOB, GPIO_Pin_8)	;
        break;
    }
    case 1:
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_7)	;
        GPIO_ResetBits(GPIOB, GPIO_Pin_8)	;
        break;
    }
    case 2:
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_7)	;
        GPIO_SetBits(GPIOB, GPIO_Pin_8)	;
        break;
    }
    case 3:
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_7)	;
        GPIO_SetBits(GPIOB, GPIO_Pin_8)	;
        break;
    }
    default:
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_7)	;
        GPIO_ResetBits(GPIOB, GPIO_Pin_8)	;
        break;
    }
    }
}

void Laser_souce_Set(unsigned char status, unsigned char lamda, unsigned int DAC_VALUE)
{
    if(status == 0) //光源关闭
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13)	; //关闭电源
        Laser_souce_lamda_set(0);
        Dac2_Set_Vol(0);
    }
    else
    {
        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)) //OTDR控制
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13)	; //关闭电源
            Laser_souce_lamda_set(0);
            Dac2_Set_Vol(0);
        }
        else
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_13)	; //打开电源
            Laser_souce_lamda_set(lamda);
            Dac2_Set_Vol(DAC_VALUE);
        }
    }
}

