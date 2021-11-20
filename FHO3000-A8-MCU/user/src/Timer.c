#include "Include.h"

/*
 * 函数名：TIM2_NVIC_Configuration
 * 描述  ：TIM2中断优先级配置
 * 输入  ：无
 * 输出  ：无	
 */
 
unsigned char g_r_VFL_time=0;
unsigned char g_r_TIMER_TIME=0;
extern unsigned char g_r_VFL_status;

void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
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
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=50000;//1000;	/* 自动重装载寄存器周期的值 */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);/* 时钟预分频数 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; /* 分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* 向上计数模式 */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);/* 清除溢出中断标志 */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);																		/* 开启时钟 */
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);/*关闭定时器等待使用*/    
}
void Timer_Init(void)
{
	TIM2_NVIC_Configuration();
	TIM2_Configuration();
}
void Timer_start(unsigned char flag)
{
	if(flag)
	{
		 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
     TIM_Cmd(TIM2, ENABLE);
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
     TIM_Cmd(TIM2, DISABLE);
	}
}

void TIM2_IRQHandler(void)//50ms中断一次
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);    
		g_r_TIMER_TIME++;
		if((g_r_TIMER_TIME%5)==0)
		{
			if(g_r_VFL_status==2)
			{
       	g_r_VFL_time++;				
				if(g_r_VFL_time%2)
				{
					 VFL_CTL(1);
					 LED_VFL_RED_CTL(1);
				}
				else 
				{
					VFL_CTL(0);
					LED_VFL_RED_CTL(0);
				}
			}
		}
			
	}		 	
}


