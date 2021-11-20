#include "Include.h"

#define POWER_KEY  0x01
#define  VFL_KEY    0X02

unsigned char g_r_key_value=0;
unsigned char g_r_long_Key_time=0;
//unsigned char g_r_power_flag=0;
					 
//******************************************************************
//函数名：  NVIC_Configuration
//功能：    配置嵌套向量中断控制器NVIC
//输入参数：无
//返回值：  无
//******************************************************************
void KEY_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}
//******************************************************************
//函数名：  EXTI_Config
//功能：    配置线中断口，并设置中断优先级
//输入参数：无
//返回值：  无
//******************************************************************
void KEY_EXTI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline(PA0 PA1) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
												
	/* config the NVIC(PA0 PA1) */
	KEY_NVIC_Configuration();

	/* EXTI line gpio config(PA0 PA1) */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//  | GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4; //  
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4; //  	
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;	 // GPIO_Mode_IN_FLOATING 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* EXTI line(PA0 PA1) mode config */
 // GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1|GPIO_PinSource2|GPIO_PinSource3|GPIO_PinSource4); // | GPIO_PinSource1
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//|EXTI_Line2|EXTI_Line3|EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line1 ;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);


}
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET) /* 确保是否产生了EXTI Line中断 */
	{   
    g_r_key_value = POWER_KEY;
		//g_r_power_flag= ON;
	}  
	EXTI_ClearITPendingBit(EXTI_Line0);     //清除中断标志位
}

void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET) /* 确保是否产生了EXTI Line中断 */
	{		
     g_r_key_value = VFL_KEY;
		
	}  
	EXTI_ClearITPendingBit(EXTI_Line1);     //清除中断标志位
}


void KEY_FUNCTION(void)
{
	unsigned int i=0;
	 if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
	 {
		 while(1)
		 {
			 if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
			 {
				 delay_ms(1);
				 i++;
				 if(i>=500)
				 {
					 break;
				 }
			 }
			 else break;
		 }
		 if(i>20&&key_sys_loading_value)
		 {
			 if(key_sys_loading_value==1)
			   key_sys_loading_value=2;
			 else if (key_sys_loading_value==3)g_r_A8_BOOT_Loading_flag=1;
			 else key_sys_loading_value=0;
			 BUZZ_CTL_TIME(200);
			 
		 }
		 else  key_sys_loading_value=0;
		 if(i>=490)
		 {
			 if(g_r_power_flag==POWER_OFF)
			 {
				 g_r_power_flag=POWER_ON;
				 POWER_ON_CTL_FUNCTION();
				 BUZZ_CTL_TIME(1000);
				 g_r_long_Key_time=1;
				 
			 }
//			 else if(g_r_power_flag==POWER_OFF_CHARGE)
//			 {
//				 g_r_power_flag=POWER_ON;
//				 BUZZ_CTL_TIME(1000);
//			 }
			 else if(g_r_power_flag==POWER_ON)
			 {
				 g_r_burst_information_tybe=0XA0;
				 g_r_long_Key_time=1;
				
				 //BUZZ_CTL_TIME(1000);
			 }
		 }
	 }
}

