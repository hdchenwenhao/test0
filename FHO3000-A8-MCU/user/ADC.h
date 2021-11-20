#ifndef __ADC_H
#define	__ADC_H


void ADC_Init_DMA(void);
void ADC_Test_Init(void);
void Voltage_ADC_OPM(void);
float Get_opm_voltage(void);
float Get_batt_voltage(void);
void OPM_Channel_config(void);
void OPM_Channel_ch(unsigned char channel);
void OPM_Discharge_config(void);
void OPM_Discharge_CTL(unsigned int time_ms);
void Filter(void);
extern __IO uint16_t ADCConvertedValue;
#endif
