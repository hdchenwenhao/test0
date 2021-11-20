#include "Include.h"


//******************************************************************
//函数名：  NVIC_Configuration
//功能：    配置嵌套向量中断控制器NVIC
//输入参数：无
//返回值：  无
//******************************************************************
void Freaq_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 9;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	
}
//******************************************************************
//函数名：  EXTI_Config
//功能：    配置线中断口，并设置中断优先级
//输入参数：无
//返回值：  无
//******************************************************************
void Freaq_EXTI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline(PA0 PA1) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
												
	/* config the NVIC(PA0 PA1) */
	Freaq_NVIC_Configuration();

	/* EXTI line gpio config(PA0 PA1) */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//  | GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4; //  
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4; //  	
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;	 // GPIO_Mode_IN_FLOATING 
  GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* EXTI line(PA0 PA1) mode config */
 // GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1|GPIO_PinSource2|GPIO_PinSource3|GPIO_PinSource4); // | GPIO_PinSource1
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;//|EXTI_Line2|EXTI_Line3|EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
 

}
void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line9) != RESET) /* 确保是否产生了EXTI Line中断 */
	{   
    
	}  
	EXTI_ClearITPendingBit(EXTI_Line9);     //清除中断标志位
}



void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*
 * 函数名：TIM2_Configuration
 * 描述  ：TIM2初始化
 * 输入  ：无
 * 输出  ：无	
 * TIM_Period--1000   
 * TIM_Prescaler--71 ---- 中断周期为1ms
*/
void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period=50000;//1000;	/* 自动重装载寄存器周期的值 */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);/* 时钟预分频数 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV2; /* 分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* 向上计数模式 */
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);/* 清除溢出中断标志 */
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3, ENABLE);																		/* 开启时钟 */
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE);/*关闭定时器等待使用*/    
}
void Timer3_Init(void)
{
	TIM3_NVIC_Configuration();
	TIM3_Configuration();
}
void Timer3_start(unsigned char flag)
{
	if(flag)
	{
		 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
     TIM_Cmd(TIM3, ENABLE);
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
     TIM_Cmd(TIM3, DISABLE);
	}
}

void TIM3_IRQHandler(void)//50ms中断一次
{
	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);    
	
	}		 	
}

