#ifndef __IIC_BUS_H
#define	__IIC_BUS_H
 
 void IIC_BUS_GPIO_Config(void);
 void IIC_SDA_Config(unsigned char RorW);
 unsigned char READ_BYTE (unsigned int ADDR,unsigned char Chip_slect);
 void WRITE_BYTE (unsigned int ADDR,unsigned char DATA_W,unsigned char Chip_slect);//0XA0;
 unsigned long READ_LONG_DATA(unsigned int address);
 void Write_Long_data(unsigned int writeaddress,unsigned long writedata);
 void WriteFloatE2prom(float writedata,unsigned int writeaddress);
 float ReadFloatE2prom(unsigned int writeaddress);
 void Write_Float_data(unsigned int address,float float_data)	;
 float READ_Float_data(unsigned int address);
 void Read_cal_power(void);
 void Read_cal_adc(void);
 void Read_wavelength_coeff(void);
 void Save_wavelength_coeff(void);
 void save_cal_constant(void);
 void reset_cal_constant(void);
 void load_cal_constant(void);
 void Write_set_status(unsigned char flag);
 void Load_set_status(void);
 void Load_SN_status(void);
 void WRITE_BYTE_TMP (unsigned char Rigith,unsigned char DATA_W,unsigned char Chip_slect);
 
 void read_rest_flag(void);
 
 #define REST_FLAG_ADDRESS 1
 
 #define DATA_SAVE_ADDRESS  250
 #define Cal_power_buff_first_address 10
 #define Wavelength_coeff_first_address 160
 #define Cal_adc_buff_first_address  80
 #define ADDRESS_Debug_flag   400
 #define ADDRESS_SN           501
 #define ADDRESS_TYPE         530    //20
 #define ADDRESS_HardWare_TYPE 560   //20
 #define APD_VBR_ADDRESS       460   //4
 #define Fiber_alive_lever_ADDRESS 464
 
 
extern unsigned char dBug_information_print_flag;
extern unsigned char g_r_SN[];
extern unsigned char g_r_SN_temp[];
extern unsigned char g_r_OTDR_TYPE[];
extern unsigned char g_r_HardWare_TYPE[];
extern float g_r_adp_voltage;
extern float g_r_APD_VBR;
 
 
#endif

