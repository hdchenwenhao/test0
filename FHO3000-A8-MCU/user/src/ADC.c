#include "Include.h"
#define N 10
#define M 3
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
#define ADC1_DR_Address    ((uint32_t)0x4001244C)	 //ADC数据寄存器的基地址
__IO uint16_t ADCConvertedValue;
__IO uint16_t  AD_Value[N][M];//[N][M];
//******************************************************************
//函数名：  ADC_Init_DMA
//功能：    DMA以及ADC初始化配置
//输入参数：无
//返回值：  无
//******************************************************************

void ADC_GPIO_Config(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);//开启IO时钟  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void ADC_Init_DMA(void)
{
/* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		 //使能DMA时钟

  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	  //使能ADC和GPIOC时钟
  
  /* DMA1 channel1 configuration ----------------------------------------------*/
 // DMA_DeInit(DMA1_Channel1);		  //开启DMA1的第一通道
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;		  //DMA对应的外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&AD_Value;//ADCConvertedValue;   //内存存储基地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;				  //DMA的转换模式为SRC模式，由外设搬移到内存
  DMA_InitStructure.DMA_BufferSize = N*M;		   //DMA缓存大小，30个
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//接收一次数据后，设备地址禁止后移
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//DMA_MemoryInc_Disable;	//关闭接收一次数据后，目标内存地址后移
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //定义外设数据宽度为16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA搬移数据尺寸，HalfWord就是为16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   //转换模式，循环缓存模式。
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA优先级高
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		  //M2M模式禁用
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);          
  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//独立的转换模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //开启扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   //开启连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC外部开关，关闭状态
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //对齐方式,ADC为12位中，右对齐方式
  ADC_InitStructure.ADC_NbrOfChannel = M;	 //开启通道数，1个
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel9 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_239Cycles5);
                     //ADC通道组， 第15个通道 采样顺序1，转换时间  

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);	  //ADC命令，使能
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  //开启ADC1

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);	  //重新校准
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));  //等待重新校准完成

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);		//开始校准
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	   //等待校准完成
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//连续转换开始，ADC通过DMA方式不断的更新RAM区。
}
void ADC_Test_Init(void)
{
	ADC_GPIO_Config();
	ADC_Init_DMA();
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
}



void Voltage_ADC_OPM(void)
{
	unsigned int ADCConvertedValueLocal=0;
	float opm_voltage=0;
	ADCConvertedValueLocal = ADCConvertedValue;//获取DMA通道的ADC电压数字值
  opm_voltage = ADCConvertedValueLocal*33/4096;//转换为电压
}
void Filter(void)
{
	unsigned char i=0,j=0;
	unsigned long sum=0;
	unsigned long AVG[2];
  float batt_voltage=0;
	float opm_voltage=0;
	for(i=0;i<2;i++)
	{
		sum=0;
		for(j=0;j<N;j++)
		{
			sum+=AD_Value[j][i]%0xffff;
		//	sum+=AD_Value[2*j+i]%0xffff;
		}
		AVG[i]=sum/N;
	}
	//AVG[0]=AD_Value[0];
	//AVG[1]=AD_Value[1];
	batt_voltage=AVG[1];
	batt_voltage=batt_voltage*3.3/4096*2;
	opm_voltage=AVG[0];
	opm_voltage=opm_voltage*3.3/4096;
	
//	printf("batt_voltage:%3f\r\n",batt_voltage);
//	printf("OPM_Voltage:%3f\r\n",opm_voltage);
}
float Get_opm_voltage(void)
{
	unsigned char i=0;
	unsigned int sum=0;
	float opm_voltage=0;
	for(i=0;i<N;i++)
	{
		sum=sum+(AD_Value[i][2]);
	}
	sum=sum/N;
	opm_voltage=sum;
	opm_voltage=opm_voltage*3.3/4096;
	//printf("OPM_Voltage:%3f\r\n",opm_voltage);
	return opm_voltage;

}
unsigned int Get_opm_adc_value(void)
{
	unsigned char i=0;
	unsigned int sum=0;
	unsigned int adc_value=0;
	for(i=0;i<N;i++)
	{
		sum=sum+(AD_Value[i][2]);
	}
	sum=sum/N;
	adc_value=sum;
	//printf("adc_value:%2d\r\n",adc_value);
	return adc_value;
	

}
float Get_batt_voltage(void)
{
	unsigned char i=0;
	unsigned int sum=0;
	float batt_voltage=0;
	for(i=0;i<N;i++)
	{
		sum=sum+(AD_Value[i][0]);
	}
	sum=sum/N;
	batt_voltage=sum;
	batt_voltage=batt_voltage*3.3/4096*5.99;
	//printf("BATT_Voltage:%3f\r\n",batt_voltage);
	return batt_voltage;
	
}


float Get_dc_in_voltage(void)
{
	unsigned char i=0;
	unsigned int sum=0;
	float dc_in_voltage=0;
	for(i=0;i<N;i++)
	{
		sum=sum+(AD_Value[i][1]);
	}
	sum=sum/N;
	dc_in_voltage=sum;
	dc_in_voltage=dc_in_voltage*3.3/4096*5.99;
	//printf("BATT_Voltage:%3f\r\n",batt_voltage);
	return dc_in_voltage;
	
}






