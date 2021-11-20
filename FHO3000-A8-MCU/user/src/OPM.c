#include "Include.h"
unsigned char g_r_opction_work_wave_lenght=0;
unsigned char g_r_opction_work_mode=0;
unsigned char g_r_power_meter_work_on=0;

unsigned char g_r_X6=0;
unsigned char g_r_X5=0;
unsigned char g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE];

unsigned char g_r_opm_channel_no=4;
unsigned char g_r_opm_status=0;

//float cal_power[16]={2e0,4e0,2e-1,5e-1,2e-2,5e-2,2e-3,5e-3,2e-4,5e-4,2e-5,5e-5,2e-6,5e-6,0,5e-7};
float cal_power[16]={50.1187286,6.30957365,3.1622777,0.501187205,0.251188636,0.0630957261,0.0316227749,0.00630957261,0.00316227763,0.000630957249,0.000316227757,6.30957657e-005,
                     3.16227779e-005,6.30957629e-006,5.11600001e-006,2.02399997e-006};
float ref_power_eeprom[7]={1.0,1.0,1.0,1.0,1.0,1.0,1.0};
unsigned long cal_adc[16]={3312,799,3646,618,3207,809,3865,776,3889,779,3924,784,3089,729,532,211};
float wavelength_coeff[7]={4.99930096,1.19675398,1.1150012,0.998819292,1.0,1.05852938,1.0958333};

unsigned int ref_power_eeproma[28];
unsigned int cal_power_eeproma[64];
unsigned int cal_adc_eeproma[64];
unsigned int wavelength_coeff_eeproma[28];
float g_r_dB_Power_meter=0;
float g_r_mW_Power_meter=0;
unsigned char g_r_opm_HI_LOW_flag=0;
unsigned char g_r_power_meter_wave_lenght=4;
unsigned char g_r_dBm_dB_wM_flag=0;

unsigned char g_r_opm_set_flag=0;
unsigned int g_r_opm_adc_value=0;
unsigned char g_r_opm_set_channel_no;




///////////////////////********************OPM****************************//
void OPM_Channel_config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG, ENABLE);//开启IO时钟 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

	  GPIO_ResetBits(GPIOG,GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13);// IO输出低电平 
}
void OPM_Channel_ch(unsigned char channel)
{
	switch(channel)
	{
		case 0x00:
		{
			 GPIO_ResetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12);
			 GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		case 0X01:
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12);
			GPIO_SetBits(GPIOA,GPIO_Pin_8);
			GPIO_SetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		case 0X02:
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_12);
			GPIO_SetBits(GPIOA,GPIO_Pin_11);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		case 0X03:
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_12);
			GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_11);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		case 0X04:
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_11);
			GPIO_SetBits(GPIOA,GPIO_Pin_12);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		case 0X05:
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_12);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		case 0X06:
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);
			GPIO_SetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		case 0X07:
		{
//			GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		}
		default:break;
	}
}

void OPM_Discharge_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);//开启IO时钟 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	  GPIO_ResetBits(GPIOC,GPIO_Pin_7);// IO输出低电平 
}
void OPM_Discharge_CTL_ms(unsigned int time_ms)
{

		GPIO_SetBits(GPIOC,GPIO_Pin_7);
	  delay_ms(time_ms);
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
}



//	unsigned int  opm_adc_value=0;
void OPM_TEST(void)
{


	
//	unsigned int i=0,j=0;
  unsigned long x=0,y=0;
//  unsigned long total_data=0;
  float p0=0,p1=0;
	float optical_power=0;
	
	float    *temp;
  unsigned char 	*tx_buff; 
  unsigned char num=0;
	delay_ms(10);
	OPM_Channel_ch(g_r_opm_channel_no);
	delay_ms(10);
	while(1)
	{
		g_r_opm_adc_value=Get_opm_adc_value();
		if(g_r_opm_adc_value>3850)
		{
			if(g_r_opm_channel_no>0)
			{
				g_r_opm_channel_no--;
				OPM_Channel_ch(g_r_opm_channel_no);
	      OPM_Discharge_CTL_ms(10);
				delay_ms(50);
			}
			else
			{
//				HI_BIT=1;
//				LOW_BIT=0;
			}
		}
		else if(g_r_opm_adc_value<150)
		{
			if(g_r_opm_channel_no>=6)
			{
//				HI_BIT=0;
//				LOW_BIT=1;
				break;
			}
			else
			{
				g_r_opm_channel_no++;
				OPM_Channel_ch(g_r_opm_channel_no);
				delay_ms(20);
			}
		}
		else//计算光功率计值
		{
			//delay_ms(10);
			break;
		}
	}
	if(g_r_opm_channel_no==6)
	{
		if(g_r_opm_adc_value>=cal_adc[12])
		{
			p0=cal_power[12];
			p1=cal_power[13];
			x=cal_adc[12];
			y=cal_adc[13];
		}
		else if(g_r_opm_adc_value<=cal_adc[15])
		{
			p0=cal_power[14];
			p1=cal_power[15];
			x=cal_adc[14];
			y=cal_adc[15];
		}
		else 
		{
			p0=cal_power[15];
			p1=cal_power[12];
			x=cal_adc[15];
			y=cal_adc[12];
		}
	}
	else
	{
		p0=cal_power[2*g_r_opm_channel_no];
		p1=cal_power[2*g_r_opm_channel_no+1];
		x=cal_adc[2*g_r_opm_channel_no];
		y=cal_adc[2*g_r_opm_channel_no+1];
	} 
	optical_power=(p0+ ((long)(g_r_opm_adc_value-x))*(p1-p0)/((long)(y-x)))*wavelength_coeff[g_r_power_meter_wave_lenght];
        g_r_dB_Power_meter=10*log10(optical_power);
        g_r_mW_Power_meter=optical_power;
	if(g_r_opm_channel_no==6&&g_r_opm_adc_value<30)g_r_opm_HI_LOW_flag=2;
	else if(g_r_opm_channel_no==0&&g_r_opm_adc_value>4080)g_r_opm_HI_LOW_flag=1;
	else g_r_opm_HI_LOW_flag=0;
	
	
	      g_r_X5&=~(0XF0);
        if(g_r_opm_HI_LOW_flag==1)
        {
          g_r_X5|=0X10;
        }
        else if(g_r_opm_HI_LOW_flag==2)
        {
          g_r_X5|=0X20;
        }
        g_r_POWER_METER_TX_BUFF[7]=g_r_X5;
	       // if(optical_power<=0) optical_power=-70;
	#if DeBug_Enable
	printf("CH: %1d -->OPW_ADC:%2d\r\n",g_r_opm_channel_no,g_r_opm_adc_value);
	printf("CH: %1d -->OPM:%2f\r\n",g_r_opm_channel_no,g_r_mW_Power_meter);
	printf("CH: %1d -->OPM:%2f\r\n",g_r_opm_channel_no,g_r_dB_Power_meter);
	#endif
	
	//**********************将浮点数，光功率计放入发送BUFF中*******************//       
        

  temp=&optical_power; 
  tx_buff=(unsigned char *)temp;
  for(num=0;num<4;num++)
  {
    g_r_POWER_METER_TX_BUFF[3+num]=*tx_buff;
 //   usart_TxBuf[5+num]=g_r_POWER_METER_TX_BUFF[3+num];
    tx_buff++;
  }
 // adc_value=g_r_adc_voltage;
//  wavelength=g_r_opction_work_wave_lenght;
  optical_power=g_r_mW_Power_meter;
//  rec_sig_freq=g_r_freaq;
	
	
}


void OPM_SET_FUNCTION(unsigned char wavelength,unsigned char channel)
{
	unsigned char i=0;
	unsigned long x=0,y=0;
  float p0=0,p1=0;
	float optical_power=0;
	
	
	if(wavelength==4&&channel<8)//档位校准模式
	{
		
		OPM_Channel_ch(g_r_opm_channel_no);
		OPM_Discharge_CTL_ms(2);//放电
		delay_ms(5);//充电
		g_r_power_meter_wave_lenght=4;
		for(i=0;i<5;i++)
		  g_r_opm_adc_value=Get_opm_adc_value();
		g_r_opm_adc_value=0;
		for(i=0;i<5;i++)
		   g_r_opm_adc_value=g_r_opm_adc_value+Get_opm_adc_value();
		g_r_opm_adc_value=g_r_opm_adc_value/5;
		
		if(channel==6)
		{
				if(g_r_opm_adc_value>=cal_adc[12])
				{
					p0=cal_power[12];
					p1=cal_power[13];
					x=cal_adc[12];
					y=cal_adc[13];
				}
				else if(g_r_opm_adc_value<=cal_adc[15])
				{
					p0=cal_power[14];
					p1=cal_power[15];
					x=cal_adc[14];
					y=cal_adc[15];
				}
				else 
				{
					p0=cal_power[15];
					p1=cal_power[12];
					x=cal_adc[15];
					y=cal_adc[12];
				}
			}
			else
			{
				p0=cal_power[2*channel];
				p1=cal_power[2*channel+1];
				x=cal_adc[2*channel];
				y=cal_adc[2*channel+1];
			} 
			optical_power=(p0+ ((long)(g_r_opm_adc_value-x))*(p1-p0)/((long)(y-x)))*wavelength_coeff[g_r_power_meter_wave_lenght];
			g_r_dB_Power_meter=10*log10(optical_power);
			g_r_mW_Power_meter=optical_power;
		}
}



