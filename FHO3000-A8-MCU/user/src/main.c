#include "include.h"
#define dBug_information_print_flag 0
#if OPM_A 
unsigned char Version_BUFF[10]={'M','-','M','A','P','8','0','6','0','1'};
#else
unsigned char Version_BUFF[10]={'M','-','M','B','P','8','0','5','3','1'};
#endif

unsigned char g_r_power_flag=0;
unsigned int g_r_dc_in_flag=0;
unsigned int g_r_charge_status=0;
unsigned int g_r_Battery_status=0;
unsigned int g_r_Battery_capacity=0;
unsigned int g_r_Battery_Tem=0;
unsigned int g_r_AverageTimeToFull=0;
unsigned int g_r_AverageTimeToEmpty=0;
unsigned int g_r_Percentage=0;
unsigned int g_r_voltage=0;
unsigned char g_r_VFL_status=0;
unsigned int g_r_second=0;
unsigned char g_r_burst_information_tybe=0;
float g_r_batt_voltage=0;
float g_r_dc_in_voltage=0;
float g_r_opm_voltage=0;

unsigned char g_r_buzz_alarm_flag=0;
unsigned char g_r_A8_BOOT_Loading_flag=0;

unsigned char g_r_LD_wavelength=0;
unsigned char g_r_ld_power_status=0;
unsigned int LD_Voltage=0;
unsigned key_sys_loading_value=0;

unsigned char g_r_laser_souce_enable;
unsigned char g_r_laser_souce_status=0;


void DD_delay_ms(unsigned long time)
{
	while(time--)
	{
		;
		;
		;
	}
}

float g_r_adp_voltage=0;
float g_r_APD_VBR=42;
float g_r_vbr_k=0;
float g_r_temp_pcb=0;
unsigned char g_r_opm_set_uart_comX=0;

int main(void)
{	
	unsigned int i=0;
	TimeDisplay=1;
	SystemInit();//配置系统时钟
	delay_init(72);//滴答时钟配置
	GPIO_OUT_INIT();
	IIC_BUS_GPIO_Config();
	//按键判断
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
	{
		while(1)
		{
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))break;
			delay_ms(1);
			i++;
			if(i>500)break;
		}
		if(i>=490)
		{
			g_r_power_flag=ON;      
		}
		else g_r_power_flag=0;
	}

		load_cal_constant();
		Load_set_status();
	
//	read_rest_flag();
//	g_r_power_flag=on;
	
	if(g_r_power_flag==ON)
	{
//		USB_POWER_CTL(0);
//		ALL_POWER_LOCK(ON);
//		CPU_POWER_CTL(ON);
//		LCD_BL_CTL(ON);
//		Timer_Init();
//		BUZZ_CTL_TIME(1000);
//		g_r_second=1;
//	  Dac1_Init();
//    Dac2_Init();
		POWER_ON_CTL_FUNCTION();		
//		g_r_adp_voltage=g_r_APD_VBR;
//		g_r_vbr_k=0.96;
//	  Apd_voltage_set(g_r_APD_VBR,g_r_vbr_k);
	}
	KEY_EXTI_Config();
	USART1_Config();//用户串口初始化
	
	if(dBug_information_print_flag&&g_r_power_flag==ON)
	{
		printf("************GRANDWAY-OTDR FHO3000Aserial R&D***********\r\n");
		printf("**************MCU:STM32F103RC  ***********************\r\n");
		printf("************Copyright by Grandway****************\r\n");
	}
		//********************实时时钟配置********************
	RTC_EXTI_Config();//RTC配置
	RTC_NVIC_Configuration();
//	LED_POWER_RED_CTL(1);
  
	 ADC_Test_Init();
   WRITE_BYTE_TMP(0x01,0x60,0X92);//精度0.0625
	 g_r_temp_pcb=GET_TMP_PCB_DB();
//	Load_set_status();
	
//	CPU_POWER_CTL(1);
	CHARGE_EANBLE(1);
	while(1)
	{		
		while(g_r_opm_set_flag)//功率计校准状态
		{
			if(g_r_CT_command==2)
				{
					COM1_FUNCTION();
			    g_r_CT_command=0;
					g_r_opm_set_uart_comX=1;					
				}
			if(g_r_com3_rx_end_flag==ON)//内串口数据分析,功率计读取
			{
				COM3_RXD_FUNCTION();
				g_r_com3_rx_end_flag=0;
				g_r_com3_rxd_point=0;
				g_r_opm_set_uart_comX=3;
			}
			if(g_r_opm_set_flag==1)
			{
			  OPM_SET_FUNCTION(g_r_power_meter_wave_lenght,g_r_opm_set_channel_no);
				if(dBug_information_print_flag)
				{
					if(g_r_opm_set_uart_comX==1)
					printf("OPMSET-->档位校准-->%2d--ADC:%3d\r\n",g_r_opm_set_channel_no,g_r_opm_adc_value);
					else if(g_r_opm_set_uart_comX==3)
					{
						UARTx_Send_Str(USART3,"OPMSET-->档位校准-->--ADC:");
						while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		        USART_SendData(USART3,g_r_opm_set_channel_no+0x30); 
						UARTx_Send_Int_4Char(USART3,g_r_opm_adc_value);
						UARTx_Send_Str(USART3,"\r\n");
						//UARTx_Send_Float_5Char(USART3,g_r_opm_adc_value);
					}
					
				}
			}
			else if(g_r_opm_set_flag==2)
			{
				OPM_TEST();
				if(dBug_information_print_flag)
				{
					if(g_r_power_meter_wave_lenght==0)
					{
						if(g_r_opm_set_uart_comX==1)
						printf("OPMSET-->波长校准-->-LAMDA=850nm--->功率值:%3f dBm\r\n",g_r_dB_Power_meter);
						else if(g_r_opm_set_uart_comX==3)
						{
							UARTx_Send_Str(USART3,"OPMSET-->lamda-->-LAMDA=850nm--->Value:");
						  UARTx_Send_Float_5Char(USART3,g_r_dB_Power_meter);
							UARTx_Send_Str(USART3,"\r\n");
						}
					}
					
					else if(g_r_power_meter_wave_lenght==1)
					{
						if(g_r_opm_set_uart_comX==1)
							printf("OPMSET-->波长校准-->-LAMDA=1300nm--->功率值:%3f dBm\r\n",g_r_dB_Power_meter);
							else if(g_r_opm_set_uart_comX==3)
							{
								UARTx_Send_Str(USART3,"OPMSET-->lamda-->-LAMDA=1300nm--->Value:");
								UARTx_Send_Float_5Char(USART3,g_r_dB_Power_meter);
								UARTx_Send_Str(USART3,"\r\n");
							}
					}
					//
					else if(g_r_power_meter_wave_lenght==2)
					{
						if(g_r_opm_set_uart_comX==1)
						printf("OPMSET-->波长校准-->-LAMDA=1310nm--->功率值:%3f dBm\r\n",g_r_dB_Power_meter);
						else if(g_r_opm_set_uart_comX==3)
						{
							UARTx_Send_Str(USART3,"OPMSET-->lamda-->-LAMDA=1310nm--->Value:");
							UARTx_Send_Float_5Char(USART3,g_r_dB_Power_meter);
							UARTx_Send_Str(USART3,"\r\n");
						}
					}
					//
					else if(g_r_power_meter_wave_lenght==3)
					{
						
						if(g_r_opm_set_uart_comX==1)
						printf("OPMSET-->波长校准-->-LAMDA=1490nm--->功率值:%3f dBm\r\n",g_r_dB_Power_meter);
						else if(g_r_opm_set_uart_comX==3)
						{
							UARTx_Send_Str(USART3,"OPMSET-->lamda-->-LAMDA=1490nm--->Value:");
							UARTx_Send_Float_5Char(USART3,g_r_dB_Power_meter);
							UARTx_Send_Str(USART3,"\r\n");
						}
					}
				//	
					else if(g_r_power_meter_wave_lenght==4)
					{
						if(g_r_opm_set_uart_comX==1)
						printf("OPMSET-->波长校准-->-LAMDA=1550nm--->功率值:%3f dBm\r\n",g_r_dB_Power_meter);
						else if(g_r_opm_set_uart_comX==3)
						{
							UARTx_Send_Str(USART3,"OPMSET-->lamda-->-LAMDA=1550nm--->Value:");
							UARTx_Send_Float_5Char(USART3,g_r_dB_Power_meter);
							UARTx_Send_Str(USART3,"\r\n");
						}
					}
				//	
					else if(g_r_power_meter_wave_lenght==5)
					{
						if(g_r_opm_set_uart_comX==1)
						printf("OPMSET-->波长校准-->-LAMDA=1625nm--->功率值:%3f dBm\r\n",g_r_dB_Power_meter);
						else if(g_r_opm_set_uart_comX==3)
						{
							UARTx_Send_Str(USART3,"OPMSET-->lamda-->-LAMDA=1625nm--->Value:");
							UARTx_Send_Float_5Char(USART3,g_r_dB_Power_meter);
							UARTx_Send_Str(USART3,"\r\n");
						}
					}
					//
					else if(g_r_power_meter_wave_lenght==6)
					{
						if(g_r_opm_set_uart_comX==1)
						printf("OPMSET-->波长校准-->-LAMDA=1650nm--->功率值:%3f dBm\r\n",g_r_dB_Power_meter);
						else if(g_r_opm_set_uart_comX==3)
						{
							UARTx_Send_Str(USART3,"OPMSET-->lamda-->-LAMDA=1650nm--->Value:");
							UARTx_Send_Float_5Char(USART3,g_r_dB_Power_meter);
							UARTx_Send_Str(USART3,"\r\n");
						}
					}
					
				}
				
				
			}
			delay_ms(500);
		}
		if(g_r_A8_BOOT_Loading_flag)//进入A8系统升级模式
		{
			g_r_VFL_status=2;
			key_sys_loading_value=0;
			VFL_KEY_FUNNCTION();
			BUZZ_CTL_TIME(2000);
			Loading_CH(ON);
			delay_ms(100);
			CPU_POWER_CTL(OFF);
			delay_ms(500);
			CPU_POWER_CTL(ON);
			i=300;
			g_r_uart4_rx_end=0;
			g_r_uart4_rx_point=0;
			while(i)
			{
				if(g_r_uart4_rx_end==2)
				{
					COM4_RXD_FUNCTION();
				}
				delay_ms(500);
				if(i%2)
				{
					LED_POWER_RED_CTL(ON);
					LED_POWER_GREED_CTL(OFF);
				}
				else
				{
					LED_POWER_RED_CTL(OFF);
					LED_POWER_GREED_CTL(ON);
				}
				if(i<295)
				{
					if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2))
					{
						delay_ms(10);
						if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2))
						{
							g_r_A8_BOOT_Loading_flag=0;
						}
					}
				}
				i--;
				if(g_r_A8_BOOT_Loading_flag==0)break;
			}
			g_r_A8_BOOT_Loading_flag=0;
			g_r_second=1;
			Loading_CH(OFF);
			delay_ms(100);
			CPU_POWER_CTL(OFF);
			delay_ms(100);
			CPU_POWER_CTL(ON);
			delay_ms(100);
			BUZZ_CTL_TIME(800);
		}
		if(g_r_power_flag==1)
		{
			POWER_OFF_CTL_FUNCTION();
			g_r_power_flag=0;
		}
		if(g_r_buzz_alarm_flag)
		{
			g_r_buzz_alarm_flag=0;
			BUZZ_CTL_TIME(300);
		}
		if(g_r_uart4_rx_end==2)
		{
			COM4_RXD_FUNCTION();
			g_r_uart4_rx_end=0;
			g_r_uart4_rx_point=0;
		}
		if(TimeDisplay)
		{
			g_r_second++;
			if(g_r_second>=60000)g_r_second=20000;
			Second_function();
			if((g_r_second%3)==0)
			{
				g_r_temp_pcb=GET_TMP_PCB_DB();
				if(g_r_temp_pcb>=TEM_POWER_OFF)
				{
					if(g_r_power_flag)
					{
						BUZZ_CTL_TIME(500);
						delay_ms(500);
						BUZZ_CTL_TIME(500);
						delay_ms(500);
						BUZZ_CTL_TIME(500);
					}
					g_r_power_flag=1;
					
				}
				else if(g_r_temp_pcb>=TEM_BUZZ_ALARM)
				{
					 BUZZ_CTL_TIME(200);
				}
			}
			if(g_r_second==12&&g_r_power_flag==ON)
			{
				USB_POWER_CTL(0);
				delay_ms(1000);
				USB_POWER_CTL(1);
			}
			TimeDisplay=0;
		}
		if(g_r_key_value)
		{
			switch(g_r_key_value)
			{
				case POWER_KEY:
					   KEY_FUNCTION();
				     break;
				case VFL_KEY:
						 VFL_KEY_FUNNCTION();
					   break;
				default:break;
			}
			g_r_key_value=0;
		}
		if(g_r_Com3_txd_flag)
		{
			if(g_r_power_flag==ON)
			UART3_TXD_BUFF();
			g_r_Com3_txd_flag=0;
			g_r_com3_txd_point=0;
		}
		if(g_r_burst_information_tybe)//有突发指令，进行数据打包
		{
			Burst_information();
			g_r_burst_information_tybe=0;
		}
		if(g_r_burst_send_time)//发送突发指令包
		{
			if(g_r_power_flag==ON)
			UART3_TXD_Burst_information();
			g_r_burst_send_time=0;
		}
		if(g_r_com3_rx_end_flag==ON)//内串口数据分析,功率计读取
		{
			COM3_RXD_FUNCTION();
			g_r_com3_rx_end_flag=0;
			g_r_com3_rxd_point=0;
		}
		if(g_r_CT_command==2)//外串口数据分析，功率计校准
		{
			COM1_FUNCTION();
			g_r_CT_command=0;
		}
		if(g_r_power_flag==ON&&(g_r_power_meter_work_on&0x01))
		{
			OPM_TEST();
		}
		if(g_r_laser_souce_enable)
		{
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9))//OTDR控制
			{
				if(g_r_laser_souce_status)
				{
					GPIO_ResetBits(GPIOC,GPIO_Pin_13)	;//关闭电源
					Laser_souce_lamda_set(0);
					Dac2_Set_Vol(0);
					g_r_laser_souce_status=0;
				}
			}
//			else
//			{
//				GPIO_SetBits(GPIOC,GPIO_Pin_13)	;//打开电源
//			}
		}
		
//		g_r_ld_power_status=1;
//		if(g_r_ld_power_status)LD_FUNCTION();
		
		
		
	}
}
//0 关闭
//1 1550
//2 1310 
//3 1625

void LD_FUNCTION(void)
{
	//电源打开
	g_r_LD_wavelength++;
	LD_Voltage=LD_Voltage+10;
	LD_Voltage=900;
	if(LD_Voltage>=3300)LD_Voltage=0;
	if(g_r_LD_wavelength>=4)g_r_LD_wavelength=0;
  LD_POWER_CTL(g_r_ld_power_status);
	LD_Channel_CTL(1);
	Dac2_Set_Vol(LD_Voltage);
	delay_ms(1000);
}

void VFL_KEY_FUNNCTION(void)
{
	if(g_r_power_flag==POWER_ON)
	 {
		 if(g_r_VFL_status==OFF)
		 {
			 g_r_VFL_status=1;
			 LED_VFL_RED_CTL(1);
			 VFL_CTL(1);
		 }
		 else if(g_r_VFL_status==1)g_r_VFL_status=2;
		 else if(g_r_VFL_status==2)
		 {
			 g_r_VFL_status=0;
			 LED_VFL_RED_CTL(0);
			 VFL_CTL(0);
		 }
		 if(g_r_VFL_status==2)
			{
				Timer_start(1);
				if(key_sys_loading_value==0)
				 key_sys_loading_value=1;
				else if(key_sys_loading_value==2)key_sys_loading_value=3;
				else key_sys_loading_value=0;
			}
			else
			{
				Timer_start(0);
			}
		BUZZ_CTL_TIME(500);	
	 }
}

void POWER_ON_CTL_FUNCTION(void)
{
	GPIO_OUT_INIT();
	
	ALL_POWER_LOCK(ON);
	CPU_POWER_CTL(ON);
	CHARGE_EANBLE(1);
	LCD_BL_CTL(ON);
	Timer_Init();
	BUZZ_CTL_TIME(1000);
	g_r_second=1;
	Dac1_Init();
	Dac2_Init();
	USART3_Config();
	USART4_Config();
	if(g_r_laser_souce_enable)
	  Laser_souce_IO_Init();
}

void POWER_OFF_CTL_FUNCTION(void)
{
	VFL_CTL(0);
	//关闭串口
	USART3_OFF();
	USART4_OFF();
	USB_POWER_CTL(0);
	
	LCD_BL_CTL(OFF);
	
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);
	DAC_SetChannel2Data(DAC_Align_12b_R, 0);
//	CHARGE_EANBLE(0);
	
	ALL_POWER_LOCK(OFF);
	CPU_POWER_CTL(OFF);
	if(g_r_laser_souce_enable)
	  Laser_souce_Set(0,0,0);
	
	
}
void Second_function(void)
{
	unsigned int i=0;
	float Float_data=0;
	g_r_batt_voltage=Get_batt_voltage();
	g_r_dc_in_voltage=Get_dc_in_voltage();
//	g_r_opm_voltage=Get_opm_voltage();
	Float_data=g_r_batt_voltage*1000;
	g_r_voltage=Float_data;
	if(g_r_long_Key_time)
	{
		if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
		  g_r_long_Key_time++;
		else g_r_long_Key_time=0;
		if(g_r_long_Key_time>=LONG_KEY_POWER_OFF_TIME)
		{
			BUZZ_CTL_TIME(1000);
			g_r_power_flag=1;
		}
	}
//	if(dBug_information_print_flag)
//	{
//		printf("BATT Voltage:%3f\r\n",g_r_batt_voltage);
//		printf("DC_IN Voltage:%3f\r\n",g_r_dc_in_voltage);
//		printf("OPM Voltage:%3f\r\n",g_r_opm_voltage);
//	}
//	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)//外接电源插入
	{
		if(g_r_dc_in_flag==0)
		{
			 BUZZ_CTL_TIME(500);
		}
		if(g_r_dc_in_voltage>6.0&&g_r_dc_in_flag==0)
		{
		   g_r_dc_in_flag=ON;
			 g_r_burst_information_tybe=0XA1;//外电插入
		}
		if(g_r_dc_in_voltage>=9&&g_r_dc_in_voltage<=14)CHARGE_EANBLE(1);
		else 
		{
			CHARGE_EANBLE(0);
			if(g_r_dc_in_voltage<=16&&g_r_dc_in_voltage>=14)
			{
				BUZZ_CTL_TIME(50);
			}
			else if(g_r_dc_in_voltage>16)
			{
				ALL_POWER_LOCK(OFF);
				CPU_POWER_CTL(OFF);
				LCD_BL_CTL(OFF);
				i=0;
				for(i=0;i<5;i++)
				{
					BUZZ_CTL_TIME(500);
					delay_ms(500);
				}
			}
		}
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0)LED_POWER_RED_CTL(0);
		else LED_POWER_RED_CTL(1);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0)LED_POWER_GREED_CTL(0);
		else LED_POWER_GREED_CTL(1);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)&&GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6))//电池不存在
		{
			g_r_Percentage=0;
			g_r_Battery_status=0;//电池不存在
		}
		else g_r_Battery_status=1;
	}
	else
	{
		g_r_Battery_status=1;
		if(g_r_dc_in_flag)
		{
			 BUZZ_CTL_TIME(500);
			 CHARGE_EANBLE(0);
		 	g_r_burst_information_tybe=0XA2;//外电拔出
		}
		if(g_r_dc_in_voltage<=6.0)
		   g_r_dc_in_flag=0;
		if(g_r_dc_in_flag==0)
		{
			LED_POWER_RED_CTL(0);
			LED_POWER_GREED_CTL(1);
			
		}
	}
	if(g_r_Battery_status)
	{
		if(g_r_dc_in_voltage>=9.6)g_r_batt_voltage=g_r_batt_voltage-0.15;
		g_r_voltage=g_r_batt_voltage*100;
		if(g_r_batt_voltage>=8.1)g_r_Percentage=100;
		else if(g_r_batt_voltage>=7.9)
		{
			Float_data=20/(8.2-7.9);
			Float_data=Float_data*(g_r_batt_voltage-7.9);
			g_r_Percentage=80+Float_data;
		}
		else if(g_r_batt_voltage>=7.7)
		{
			Float_data=20/(7.9-7.7);
			Float_data=Float_data*(g_r_batt_voltage-7.7);
			g_r_Percentage=60+Float_data;
		}
		else if(g_r_batt_voltage>=7.4)
		{
			Float_data=20/(7.7-7.4);
			Float_data=Float_data*(g_r_batt_voltage-7.4);
			g_r_Percentage=40+Float_data;
		}
		else if(g_r_batt_voltage>=6.5)
		{
			Float_data=40/(7.4-6.5);
			Float_data=Float_data*(g_r_batt_voltage-6.5);
			g_r_Percentage=Float_data;
		}
	}
	
	
}




