#ifndef __OPM_H
#define	__OPM_H
#define L850 0
#define L1300 1
#define L1310 2
#define L1490 3
#define L1550 4
#define L1625 5
#define L1650 6

#define TWIN_MODE 0X01
#define CW_MODE   0X00
#define POWER_METER_TX_BUFF_SIZE 13

extern unsigned char g_r_opction_work_wave_lenght;
extern unsigned char g_r_opction_work_mode;
extern unsigned char g_r_X6;
extern unsigned char g_r_X5;
extern unsigned char g_r_power_meter_work_on;
extern unsigned char g_r_POWER_METER_TX_BUFF[POWER_METER_TX_BUFF_SIZE];

extern unsigned char g_r_power_meter_wave_lenght;
extern unsigned char g_r_dBm_dB_wM_flag;
extern float g_r_dB_Power_meter;
extern float g_r_mW_Power_meter;
extern unsigned char g_r_opm_HI_LOW_flag;
extern unsigned char g_r_opm_channel_no;
//extern 	unsigned int  opm_adc_value;
extern unsigned char g_r_opm_status;

extern unsigned int ref_power_eeproma[28];
extern unsigned int cal_power_eeproma[64];
extern unsigned int cal_adc_eeproma[64];
extern unsigned int wavelength_coeff_eeproma[28];
extern float wavelength_coeff[7];
extern unsigned long cal_adc[16];
extern float ref_power_eeprom[7];
extern float cal_power[16];

extern unsigned char g_r_opm_set_flag;
extern unsigned int g_r_opm_adc_value;
extern unsigned char g_r_opm_set_channel_no;



#endif

