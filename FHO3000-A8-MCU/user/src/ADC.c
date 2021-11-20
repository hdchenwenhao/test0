#include "Include.h"
#define N 10
#define M 3
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
#define ADC1_DR_Address    ((uint32_t)0x4001244C)	 //ADC���ݼĴ����Ļ���ַ
__IO uint16_t ADCConvertedValue;
__IO uint16_t  AD_Value[N][M];//[N][M];
//******************************************************************
//��������  ADC_Init_DMA
//���ܣ�    DMA�Լ�ADC��ʼ������
//�����������
//����ֵ��  ��
//******************************************************************

void ADC_GPIO_Config(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//����IOʱ��  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);//����IOʱ��  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void ADC_Init_DMA(void)
{
/* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		 //ʹ��DMAʱ��

  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	  //ʹ��ADC��GPIOCʱ��
  
  /* DMA1 channel1 configuration ----------------------------------------------*/
 // DMA_DeInit(DMA1_Channel1);		  //����DMA1�ĵ�һͨ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;		  //DMA��Ӧ���������ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&AD_Value;//ADCConvertedValue;   //�ڴ�洢����ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;				  //DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
  DMA_InitStructure.DMA_BufferSize = N*M;		   //DMA�����С��30��
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//����һ�����ݺ��豸��ַ��ֹ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//DMA_MemoryInc_Disable;	//�رս���һ�����ݺ�Ŀ���ڴ��ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //�����������ݿ��Ϊ16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA�������ݳߴ磬HalfWord����Ϊ16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   //ת��ģʽ��ѭ������ģʽ��
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA���ȼ���
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		  //M2Mģʽ����
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);          
  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//������ת��ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //����ɨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   //��������ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC�ⲿ���أ��ر�״̬
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //���뷽ʽ,ADCΪ12λ�У��Ҷ��뷽ʽ
  ADC_InitStructure.ADC_NbrOfChannel = M;	 //����ͨ������1��
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel9 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_239Cycles5);
                     //ADCͨ���飬 ��15��ͨ�� ����˳��1��ת��ʱ��  

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);	  //ADC���ʹ��
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  //����ADC1

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);	  //����У׼
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));  //�ȴ�����У׼���

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);		//��ʼУ׼
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	   //�ȴ�У׼���
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//����ת����ʼ��ADCͨ��DMA��ʽ���ϵĸ���RAM����
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
	ADCConvertedValueLocal = ADCConvertedValue;//��ȡDMAͨ����ADC��ѹ����ֵ
  opm_voltage = ADCConvertedValueLocal*33/4096;//ת��Ϊ��ѹ
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






