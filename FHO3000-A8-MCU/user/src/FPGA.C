#include "Include.h"
//******************************************************************
//函数名：  FPGA
//功能：    和FPGA通信接口
//输入参数：无
//返回值：  无
//******************************************************************
extern unsigned char g_r_FPGA_TEST_OVER_flag;
extern unsigned char dBug_information_print_flag;
extern unsigned int  g_r_SampleFreq;
extern unsigned int delete_point_buff[2];
extern unsigned long AVG_SAMPLE_NOISE_END;

unsigned  int g_r_read_page_data[1024];


void Delay_ns(unsigned long time)
{
	while(time)time--;
}
void FPGA_CTL_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);//开启IO时钟 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;	
  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,0xf000);
	
	Realy_GPIO_Config();
	

}
void FPGA_IRQ_IN_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);//开启IO时钟 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void FPGA_DATA_Config(unsigned char RorW)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	if(RorW==write)//写模式下，数据接口为输出
	{
		RCC_APB2PeriphClockCmd( FPGA_DATA_CLK, ENABLE);//开启IO时钟 
		GPIO_InitStructure.GPIO_Pin = 0xFFFF;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(FPGA_DATA_PORT, &GPIO_InitStructure);

		//GPIO_SetBits(FPGA_DATA_PORT, 0XFFFF);
	}
	else//读模式下，数据接口为输入
	{
		RCC_APB2PeriphClockCmd( FPGA_DATA_CLK, ENABLE);//开启IO时钟 
    GPIO_InitStructure.GPIO_Pin = 0xFFFF;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FPGA_DATA_PORT, &GPIO_InitStructure);

    //GPIO_SetBits(FPGA_DATA_PORT, 0XFFFF);
	}
}

void FPGA_VCC_CTL_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//开启IO时钟 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	  GPIO_ResetBits(GPIOA,GPIO_Pin_1);// IO输出低电平 
}
void FPGA_VCC_CTL(unsigned char flag)
{
	if(flag)GPIO_SetBits(GPIOA, GPIO_Pin_1);
	else GPIO_ResetBits(GPIOA,GPIO_Pin_1);// IO输出低电平 
}

unsigned int FPGA_DATA_IN(unsigned int ADDRESS_L)
{
	unsigned int data_read=0;
	FPGA_DATA_Config(read);
	Delay_ns(1);
	FPGA_CE_L;
	Delay_ns(1);
	FPGA_RD_L;
	Delay_ns(1);
	data_read=GPIO_ReadInputData(FPGA_DATA_PORT);
	Delay_ns(1);
	FPGA_RD_H;
	Delay_ns(1);
	FPGA_CE_H;
	return data_read;
}
void FPGA_DATA_OUT(unsigned int data)
{
	GPIO_ResetBits(FPGA_DATA_PORT,0xffff);
	GPIO_SetBits(FPGA_DATA_PORT, data);
}
void FPGA_ADDRESS_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(FPGA_ADDRESS_CLK, ENABLE);//开启IO时钟 
  GPIO_InitStructure.GPIO_Pin = FPGA_ADDRESS_PIN;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(FPGA_ADDRESS_PORT, &GPIO_InitStructure);

  GPIO_SetBits(FPGA_ADDRESS_PORT, 0X0000);
}
void FPGA_Write_address(unsigned int address)
{
	GPIO_ResetBits(FPGA_ADDRESS_PORT,FPGA_ADDRESS_PIN);
	GPIO_SetBits(FPGA_ADDRESS_PORT, address&0X0FFF);
}

//******************************************************************
//函数名：  FPGA_IRQ_NVIC_Configuration
//功能：    配置嵌套向量中断控制器NVIC
//输入参数：无
//返回值：  无
//******************************************************************
void FPGA_IRQ_NVIC_Configuration(void)
{

  NVIC_InitTypeDef NVIC_InitSructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);          //???????2
 
	NVIC_InitSructure.NVIC_IRQChannel = EXTI15_10_IRQn;        //???????15????
	NVIC_InitSructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitSructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitSructure.NVIC_IRQChannelCmd = ENABLE;                               //??????
	NVIC_Init(&NVIC_InitSructure);
}
//******************************************************************
//函数名：  FPGA_IRQ_EXTI_Config
//功能：    配置线中断口，并设置中断优先级
//输入参数：无
//返回值：  无
//******************************************************************
void FPGA_IRQ_EXTI_Config(unsigned char flag)
{

  GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //     
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;// GPIO_Mode_IN_FLOATING;//;//GPIO_Mode_IPU;	 // GPIO_Mode_IN_FLOATING 
  GPIO_Init(GPIOE, &GPIO_InitStructure);

	                                                      //?????

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource15);

  EXTI_ClearITPendingBit(EXTI_Line15);

  EXTI_InitStructure.EXTI_Mode =EXTI_Mode_Interrupt;                            //????????
  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;// EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising;                    //?????
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;  // ???????????
  if(flag)EXTI_InitStructure.EXTI_LineCmd = ENABLE;                                                        // ?????????  ??
  else EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);                     //?EXIT_InitStructure?????????????

 }

void FPGA_Init_Config(void)
{
	
	FPGA_DATA_Config(write);
	FPGA_VCC_CTL_Config();
	FPGA_ADDRESS_Config();
	FPGA_CTL_Config();
	//FPGA_IRQ_NVIC_Configuration();
	//FPGA_IRQ_EXTI_Config(0);
	FPGA_IRQ_IN_Config();
}



void FPGA_W_COMMAND(unsigned int address,unsigned int data)
{
	FPGA_DATA_Config(write);//将数据位配置成输出模式
	Delay_ns(1);
	FPGA_Write_address(address);
	FPGA_DATA_OUT(data);
	Delay_ns(1);
	FPGA_CE_L;
	Delay_ns(1);
	FPGA_WR_L;
	Delay_ns(1);
	FPGA_WR_H;
	Delay_ns(1);
	FPGA_CE_H;
	Delay_ns(1);
}

void FPGA_TEST_START_FUNCTION(unsigned int relay,unsigned int wavelenght,unsigned int Ls_type,unsigned int FreaqShitNo, unsigned int pulse_width,
	                            unsigned int sample_point,unsigned int avg_number)
{
	unsigned int CRC_value=0;
	unsigned int pulse_number=0;
	unsigned int basic_information=0;
	unsigned long Sample_point_number=0;
	Sample_point_number=sample_point;
	if(Sample_point_number>=65000)Sample_point_number=65000;
	basic_information|=0x8000;
	if(wavelenght==1310)
	{
		basic_information&=0xFFFC;
	}
	else if(wavelenght==1550)
	{
		basic_information&=0xFFFC;
		basic_information|=0X0001;
	}
	else if(wavelenght==1490)
	{
		basic_information&=0xFFFC;
		basic_information|=0X0002;
	}
	else if(wavelenght==1625)
	{
		basic_information|=0X0003;
	}
	if(Ls_type==0)//脉冲光源
		basic_information&=0xfffb;
	else basic_information|=0x0004;
	
	if(FreaqShitNo<=63)
	{
		basic_information&=0x8007;
		basic_information=basic_information+(FreaqShitNo<<3);
	}
	
	
	if(pulse_width==3)pulse_number=1;
	else if(pulse_width==5)pulse_number=2;
	else pulse_number=pulse_width*1000/3252;
	
	CRC_value=basic_information+pulse_number+Sample_point_number+avg_number;
	CRC_value=CRC_value%0X10000;
	FPGA_W_COMMAND(0X63f<<1,0);
	FPGA_W_COMMAND(0X640<<1,0);
	FPGA_W_COMMAND(0X641<<1,basic_information);
	FPGA_W_COMMAND(0X642<<1,pulse_number);
	FPGA_W_COMMAND(0X643<<1,Sample_point_number);
	FPGA_W_COMMAND(0X644<<1,avg_number);
	FPGA_W_COMMAND(0X645<<1,0);
	FPGA_W_COMMAND(0X646<<1,0);
	FPGA_W_COMMAND(0X647<<1,0);
	FPGA_W_COMMAND(0X648<<1,0);
	FPGA_W_COMMAND(0X649<<1,0);
	FPGA_W_COMMAND(0X64A<<1,0);
	FPGA_W_COMMAND(0X64B<<1,0);
	FPGA_W_COMMAND(0X64C<<1,0);
	FPGA_W_COMMAND(0X64D<<1,CRC_value);
	if(dBug_information_print_flag)
	{
		printf("relay:%d\r\n",relay);
		printf("basic_information:%d\r\n",basic_information);
		printf("pulse_number:%d\r\n",pulse_number);
		printf("sample_point:%d\r\n",sample_point);
		printf("avg_number:%d\r\n",avg_number);
		printf("CRC_value:%d\r\n",CRC_value);
	}
	
}
//读取1024个数据


int FPGA_READ_PAGE_DATA(unsigned int address_H,unsigned int sample_max_point)
{
	unsigned int i=0;
	unsigned int read_data=0;
	unsigned int start_point=0;
	unsigned int point_delete_buff=0;
	if(g_r_SampleFreq==25)
		point_delete_buff=delete_point_buff[0];
		else point_delete_buff=delete_point_buff[1];
	if(address_H==0)
	{
		if(g_r_SampleFreq==25)
		start_point=delete_point_buff[0];
		else start_point=delete_point_buff[1];
	}
	else start_point=0;
	//if(address_H>14)return 0;//最多15000点
	FPGA_W_COMMAND(0x545<<1,address_H);
	
	FPGA_DATA_Config(read);
	for(i=start_point;i<1024;i++)
	{
		//read_data=FPGA_DATA_IN(i);
			if((address_H)*1024+i>=sample_max_point+point_delete_buff)
			{
				g_r_read_page_data[i-start_point-1]=AVG_SAMPLE_NOISE_END;
				break;
			}
		  FPGA_Write_address(i*2);
			Delay_ns(1);
			FPGA_CE_L;
			Delay_ns(1);
			FPGA_RD_L;
			Delay_ns(1);
			g_r_read_page_data[i-start_point]=GPIO_ReadInputData(FPGA_DATA_PORT);
			Delay_ns(1);
			FPGA_RD_H;
			Delay_ns(1);
			FPGA_CE_H;
		  Delay_ns(1);
		if(i==1||g_r_read_page_data[i-start_point]<=0X7FF0)
		{
			
		  FPGA_Write_address(i*2);
			Delay_ns(1);
			FPGA_CE_L;
			Delay_ns(1);
			FPGA_RD_L;
			Delay_ns(1);
			g_r_read_page_data[i-start_point]=GPIO_ReadInputData(FPGA_DATA_PORT);
			Delay_ns(1);
			FPGA_RD_H;
			Delay_ns(1);
			FPGA_CE_H;
		  Delay_ns(1);
//			g_r_read_page_data[i]=0;
		}
		
	//	printf("%d\r\n",g_r_read_page_data[i-start_point]);
	//	printf("\r\n");
	}
	
	
	return i-start_point;
}

unsigned int Clear_IRQ_LINE(void)
{
	unsigned int data_read=0;
	FPGA_DATA_Config(read);
	FPGA_CTL_Config();
	FPGA_Write_address(0X541<<1);
	Delay_ns(1);
	FPGA_CE_L;
	Delay_ns(1);
	FPGA_RD_L;
	Delay_ns(1);
	data_read=GPIO_ReadInputData(FPGA_DATA_PORT);
	Delay_ns(1);
	FPGA_RD_H;
	Delay_ns(1);
	FPGA_CE_H;
	Delay_ns(1);
	GPIO_ResetBits(FPGA_ADDRESS_PORT,FPGA_ADDRESS_PIN);
	return data_read;
}



void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line15) != RESET) /* 确保是否产生了EXTI Line中断 */
	{
		g_r_FPGA_TEST_OVER_flag=1;
		printf("<<FPGA TEST OEVR Interrupt!>> \r\n");
	
	}	
	
	EXTI_ClearITPendingBit(EXTI_Line15);     //清除中断标志位
}
void Realy_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);//开启IO时钟 
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;	
  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}
void Relay_SET(unsigned char relay_no)
{
	if(relay_no<=4)
			GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	else GPIO_SetBits(GPIOD,GPIO_Pin_14);
	switch(relay_no)
	{
		case 1:
			GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13);
		  //GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_13);
		  break;
		case 2:
			GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_13);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_11);
		  break;
		case 3:
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_11);
		  break;
		case 4:
		//	GPIO_SetBits(GPIOD,GPIO_Pin_13);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_13);
		  break;
		
		case 5:
			GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13);
		  //GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_13);
		  break;
		case 6:
			GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_13);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_11);
		  break;
		case 7:
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_11);
		  break;
		case 8:
		//	GPIO_SetBits(GPIOD,GPIO_Pin_13);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_13);
		  break;
		default:
			GPIO_SetBits(GPIOD,GPIO_Pin_11);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_13);
		break;
		
	}
}

