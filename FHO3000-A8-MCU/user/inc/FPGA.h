#ifndef __FPGA_H
#define	__FPGA_H

#define FPGA_DATA_CLK        RCC_APB2Periph_GPIOF //LED时钟源定义
#define FPGA_ADDRESS_CLK     RCC_APB2Periph_GPIOE //LED时钟源定义
#define FPGA_DATA_PORT       GPIOF //LED IO组定义
#define FPGA_ADDRESS_PORT       GPIOE //LED IO组定义
#define FPGA_ADDRESS_PIN     0X0FFF

#define FPGA_CE_H  GPIO_SetBits(GPIOE,GPIO_Pin_12);
#define FPGA_CE_L  GPIO_ResetBits(GPIOE,GPIO_Pin_12);

#define FPGA_RD_H  GPIO_SetBits(GPIOE,GPIO_Pin_13);
#define FPGA_RD_L  GPIO_ResetBits(GPIOE,GPIO_Pin_13);

#define FPGA_WR_H  GPIO_SetBits(GPIOE,GPIO_Pin_14);
#define FPGA_WR_L  GPIO_ResetBits(GPIOE,GPIO_Pin_14);

#define read 0x00
#define write 0x01

void FPGA_DATA_Config(unsigned char RorW);
void FPGA_ADDRESS_Config(void);
void FPGA_Write_address(unsigned int address);
void FPGA_IRQ_NVIC_Configuration(void);
void FPGA_IRQ_EXTI_Config(unsigned char flag);
void FPGA_VCC_CTL_Config(void);
void FPGA_VCC_CTL(unsigned char flag);
void FPGA_Init_Config(void);

int FPGA_READ_PAGE_DATA(unsigned int address_H,unsigned int sample_max_point);


void FPGA_TEST_START_FUNCTION(unsigned int relay,unsigned int wavelenght,unsigned int Ls_type,unsigned int FreaqShitNo, unsigned int pulse_width,
                              unsigned int sample_point,unsigned int avg_number);

void Relay_SET(unsigned char relay_no);
void Realy_GPIO_Config(void);

unsigned int Clear_IRQ_LINE(void);

extern unsigned int g_r_read_page_data[1024];

#endif 

