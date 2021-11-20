#ifndef __ADC_H
#define	__ADC_H
#define POWER_METER_A 1

#define dBm   0
#define dB    1
#define mW    2

void ADC_Init_DMA(void);
void ADC_Test_Init(void);
void Voltage_ADC_OPM(void);
float Get_opm_voltage(void);
float Get_batt_voltage(void);
float GET_TMP_PCB_DB(void);

void OPM_Channel_config(void);
void OPM_Channel_ch(unsigned char channel);
void OPM_Discharge_config(void);
void OPM_Discharge_CTL_ms(unsigned int time_ms);
void Filter(void);
void OPM_TEST(void);
void OPM_SET_FUNCTION(unsigned char wavelength,unsigned char channel);
float Get_dc_in_voltage(void);
unsigned int Get_opm_adc_value(void);



//extern __IO uint16_t ADCConvertedValue;
#endif
