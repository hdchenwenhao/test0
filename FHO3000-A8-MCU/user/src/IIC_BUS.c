#include <include.h>

#define read 0x00
#define write 0x01

#define SDA_IN  IIC_SDA_Config(read)// PC1 IN
#define SDA_OUT IIC_SDA_Config(write) // PC1 OUT

#define SDA_LOW  GPIO_ResetBits(GPIOB,GPIO_Pin_6) // sda=0
#define SDA_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_6) // sda=1

//#define SCL_IN P8DIR&=~BIT2 // PC1 IN
//#define SCL_OUT P8DIR|=BIT2 // PC1 OUT

#define SCL_LOW  GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define SCL_HIGH GPIO_SetBits(GPIOB,GPIO_Pin_5)

//#define SCL    BIT2
//#define SDA    BIT1
#define ACK (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6))
unsigned char g_r_HardWare_TYPE[20]={'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x'};
unsigned char g_r_OTDR_TYPE[10];
unsigned char g_r_SN[20]={'Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y'};
unsigned char g_r_SN_temp[20]={'Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y','Y'};
unsigned char DATAR01,DATAR11,DATAR21,DATAR31,DATAR41;


void IIC_BUS_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void IIC_SDA_Config(unsigned char RorW)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	if(RorW==write)//写模式下，数据接口为输出
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		//GPIO_SetBits(FPGA_DATA_PORT, 0XFFFF);
	}
	else//读模式下，数据接口为输入
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//开启IO时钟 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //GPIO_SetBits(FPGA_DATA_PORT, 0XFFFF);
	}
}

void IIC_START (void)
{
   SDA_HIGH; //SDA=1
   delay_us(20);
   SCL_HIGH; //SCL=1
   delay_us(20);
   SDA_LOW; //SDA=0
   delay_us(20);

   SCL_LOW; //SCL=0
}
void IIC_STOP (void)
{
   SDA_LOW; //SDA=0
   delay_us(20);
   SCL_HIGH;
   delay_us(20);

   SDA_HIGH;
   delay_us(20);

//   SDA_LOW;///????
}
void IIC_ACK (void)
{
   unsigned char  ucErrTime = 255;//???????ACK,????
   SDA_HIGH;
   SDA_IN;

   SCL_LOW;
   delay_us(20);

//   delay_us(10);

   SCL_HIGH;
   delay_us(20);
   while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6))
   {
      ucErrTime--;
      if (ucErrTime == 0)
      {
         IIC_STOP();
         return ;
      }
      delay_us(20);
   }
   SCL_LOW;
   SDA_OUT;
   return;
}
void IIC_TX_DATA (unsigned char Data)
{
   char i=8;
   do
   {
      if((Data&0x80)==0x80)
         SDA_HIGH;
      else
         SDA_LOW;
	  delay_us(20);
      SCL_HIGH;
      
	  delay_us(20);

      SCL_LOW;
	  delay_us(20);
      Data=Data<<1;
   } while(--i!=0);
    IIC_ACK ();
}

unsigned char IIC_RX_DATA (void)
{
    unsigned int Rn=8,DATA=0,RR=0x80;
    SDA_IN; // ?????
    delay_us(30);
    for (;Rn!=0;Rn--)
    {
        SCL_HIGH;
		delay_us(30);

        if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)!=0)
            DATA|=RR;
        else
            DATA&=~RR;
        RR>>=1;
		delay_us(30);
        SCL_LOW;
		delay_us(30);

    }
	delay_us(30);	
    SDA_OUT;
    return DATA;
}

void Host_Ack(unsigned char ack)
{
	if(ack)
		SDA_HIGH;
	else
		SDA_LOW;
	delay_us(30);

	SCL_HIGH;
	delay_us(30);

	SCL_LOW;
}

unsigned char READ_BYTE (unsigned int ADDR,unsigned char Chip_slect)///0XA1
{
    unsigned char DATA_R,ADDR_L,ADDR_H;
    unsigned char Chip_slect1=(Chip_slect&0xFE);
    ADDR_L=ADDR%256;
    ADDR_H=ADDR/256;
    SDA_OUT;
    IIC_START ();
    IIC_TX_DATA (Chip_slect1);
	delay_us(20);

    IIC_TX_DATA (ADDR_H);
	delay_us(20);

    IIC_TX_DATA (ADDR_L);
	delay_us(20);

    IIC_START ();
    IIC_TX_DATA (Chip_slect);
	delay_us(20);

    DATA_R=IIC_RX_DATA();
    IIC_STOP ();
    SDA_OUT;
    return DATA_R;
}

void WRITE_BYTE (unsigned int ADDR,unsigned char DATA_W,unsigned char Chip_slect)//0XA0
{
    unsigned char ADDR_H,ADDR_L;
    unsigned char Chip_slect1=(Chip_slect&0xFE);
 //   P6OUT&=0XFD;   //???
    ADDR_L=ADDR%256;
    ADDR_H=ADDR/256;

    SDA_OUT;
    IIC_START ();
    IIC_TX_DATA (Chip_slect1); // E2PROM ??
	  delay_us(50);

    IIC_TX_DATA (ADDR_H); // ??256BIT?EEPROM??????
	  delay_us(50);

    IIC_TX_DATA (ADDR_L);
	  delay_us(50);

    IIC_TX_DATA (DATA_W);
	  delay_us(50);

    IIC_STOP ();
		delay_ms(10);
   // P6OUT&=0X02;   //???
}

unsigned long READ_LONG_DATA(unsigned int address)
{
  unsigned long data_buff=0;
  unsigned char read_char=0;
  unsigned char i=0;
  for(i=0;i<4;i++)
  {
    read_char=READ_BYTE(address+i,0xA1);
    data_buff=(data_buff*256)+read_char;
		delay_us(20);
  }
  return data_buff;
}
void Write_Long_data(unsigned int writeaddress,unsigned long writedata)
{
  unsigned char i=0;
  unsigned char data_buff[4];
  data_buff[3]=writedata%256;
  data_buff[2]=(writedata/256)%256;
  data_buff[1]=((writedata/256)/256)%256;
  data_buff[0]=((writedata/256)/256)/256;
  for(i=0;i<4;i++)
  {
    WRITE_BYTE(writeaddress+i,data_buff[i],0xA0);
    delay_ms(5);
  }
}

void WriteFloatE2prom(float writedata,unsigned int writeaddress)
{
	float    *temp;
	unsigned char 	*e2promBuf; 
	//unsigned int 	e2promAdress;
	unsigned char num=0;
	temp=&writedata; 
	e2promBuf=(unsigned char *)temp;
	for(num=0;num<4;num++)
	{
		WRITE_BYTE(writeaddress+num,*e2promBuf,0xa0);
		e2promBuf++;
		delay_us(20);
	}
}
float ReadFloatE2prom(unsigned int writeaddress)
{
	unsigned char	e2promBuf[4]; 
	float temp;
	unsigned char num=0;
	float	*buf=(float*)e2promBuf;
	for(num=0;num<4;num++)
	{
		e2promBuf[num]=READ_BYTE (writeaddress+num,0xa1);
		delay_us(10);
	}
	temp=*buf;
	return temp;
}
void SendStr(unsigned int suba,unsigned char *s,unsigned char no)
{
   unsigned char i;
   IIC_START ();               /*????*/
   IIC_TX_DATA (0xa0); // E2PROM ??
   delay_us(10);
   i=(unsigned char)(suba>>8);
   IIC_TX_DATA (i); /*???????*/
   delay_us(10);	 
   i=(unsigned char)(suba&0x00ff);
   IIC_TX_DATA (i); /*???????*/
   delay_us(10);	 
   for(i=0;i<no;i++)
    {   
        IIC_TX_DATA (*s); /*????*/
        delay_us(20);	 
        s++;
    } 
    IIC_STOP ();                /*????*/ 
}

void WriteNData(unsigned int suba,unsigned char *s,unsigned char no)
{
   unsigned char i;
   IIC_START ();              /*????*/
   IIC_TX_DATA (0xa0);           /*??????*/
   delay_us(30);
   if(ACK==0)return;
   i=(unsigned char)(suba>>8);
   IIC_TX_DATA(i);            /*???????*/
    if(ACK==0)return;
   i=(unsigned char)(suba&0x00ff);
   IIC_TX_DATA(i);            /*???????*/
   if(ACK==0)return;
   for(i=0;i<no;i++)
    {   
        IIC_TX_DATA(*s); 
        delay_us(10);
         if(ACK==0)return;
        s++;
    } 
    IIC_STOP ();                /*????*/ 
}
#if 1
void I2cSendStr(unsigned int suba,unsigned char *s,unsigned char no)
{
        unsigned int length=0;
        unsigned int suba1;
        unsigned int add1,add2;
        suba1=suba+no; 
        add1=suba/64;
        add2=suba1/64;
        if(add2>add1+2)return;//???????
        if(add1==add2)/// ???
        {
            WriteNData(suba,s,no);
            delay_us(20);              
        }
        else
        {
             length=suba%64;
             length=64-length;
             WriteNData(suba,s,length);
             delay_us(20);            
             length=no-length;
             suba1=(add1+1)*64;
             WriteNData(suba1,s,length);
             delay_us(20);              
        }
}
#else
void I2cSendStr(unsigned int suba,unsigned char *s,unsigned char no)
{
	unsigned char i=0;
	for(i=0;i<no;i++)
	{

	WRITE_BYTE(suba+i,*(s+i),0xa0);
	delay_ms(10);
	}
}
#endif
unsigned char  RcvByte(void)

{
	unsigned char temp=0;
	temp=IIC_RX_DATA();
	return temp;
}
void I2cRcvStr(unsigned int suba,unsigned char *s,unsigned char no)
{
   unsigned char i;
   unsigned char temp;
   for(i=0;i<no;i++)
   {
       temp=READ_BYTE (suba+i,0xa3);
       *s++=temp;
       delay_us(20);
   }
}

/**********************************************/

void Write_Float_data(unsigned int address,float float_data)	
{
	unsigned char *Float_p;
	unsigned char i =0;
	Float_p = (unsigned char *)&float_data;
	for(i=0;i<4;i++)
	{
		WRITE_BYTE (address+i,*Float_p++,0XA0);
		delay_ms(5);
	}
}


float READ_Float_data(unsigned int address)
{
	float Float_data = 0 ;
	unsigned char FF_BUF = 0 ;
//	unsigned char N_buff[4]={0x00,0x00,0x00,0x00} ;
	unsigned char *N_buff;
	unsigned char i=0;
	N_buff = (unsigned char *)&Float_data;
	for(i=0;i<4;i++)
	{
		N_buff[i]= READ_BYTE(address+i,0xA1);
		if(N_buff[i]==0xff)FF_BUF++;
		delay_us(10);
	}
	if(FF_BUF>=4)Float_data = 0 ;
	return Float_data;

}




///**************************************************************//
void Read_cal_power(void)
{
  unsigned int i=0;
  for(i=0;i<16;i++)
  {
    cal_power[i]=READ_Float_data(Cal_power_buff_first_address+DATA_SAVE_ADDRESS+i*4);
  } 
}
void Read_cal_adc(void)
{
  unsigned int i=0;
  for(i=0;i<16;i++)
  {
		cal_adc[i]=0;
    cal_adc[i]=READ_LONG_DATA(Cal_adc_buff_first_address+DATA_SAVE_ADDRESS+i*4);
	//	cal_adc[i]=cal_adc[i]&0xffff;
  }
  
}

void Read_wavelength_coeff(void)
{
  unsigned int i=0;
  for(i=0;i<7;i++)
  {
    wavelength_coeff[i]=READ_Float_data(Wavelength_coeff_first_address+DATA_SAVE_ADDRESS+i*4);
  } 
}

void Save_wavelength_coeff(void)
{
   unsigned int i=0;
  for(i=0;i<7;i++)
  {
    Write_Float_data(Wavelength_coeff_first_address+i*4+DATA_SAVE_ADDRESS,wavelength_coeff[i]);
    delay_ms(5);
  }
}

void save_cal_constant(void)
{
  unsigned int i=0;
  for(i=0;i<16;i++)
  {
    Write_Float_data(Cal_power_buff_first_address+i*4+DATA_SAVE_ADDRESS,cal_power[i]);
    delay_ms(5);
    Write_Long_data(Cal_adc_buff_first_address+i*4+DATA_SAVE_ADDRESS,cal_adc[i]);
    delay_ms(5);
  }
  for(i=0;i<7;i++)
  {
    Write_Float_data(Wavelength_coeff_first_address+i*4+DATA_SAVE_ADDRESS,wavelength_coeff[i]);
    delay_ms(5);
  }
}
void reset_cal_constant(void)
{
  unsigned int i=0;
#if POWER_METER_A
  float cal_power_reset[16]={50.1187286,6.30957365,3.1622777,0.501187205,0.251188636,0.0630957261,0.0316227749,0.00630957261,0.00316227763,0.000630957249,0.000316227757,6.30957657e-005,
                     3.16227779e-005,6.30957629e-006,5.11600001e-006,2.02399997e-006};
 unsigned long cal_adc_reset[16]={3312,799,3646,618,3207,809,3865,776,3889,779,3924,784,3089,729,532,211};
#else
float cal_power_reset[16]={50.1187286,6.30957365,3.1622777,0.501187205,0.251188636,0.0630957261,0.0316227749,0.00630957261,0.00316227763,0.000630957249,0.000316227757,6.30957657e-005,
                     3.16227779e-005,6.30957629e-006,5.11600001e-006,2.02399997e-006};
 unsigned long cal_adc_reset[16]={3312,799,3646,618,3207,809,3865,776,3889,779,3924,784,3089,729,532,211};
#endif
  
	float wavelength_coeff_reset[7]={4.99930096,1.19675398,1.1150012,0.998819292,1.0,1.05852938,1.0958333};
 
  for(i=0;i<16;i++)
  {
    cal_power[i]=cal_power_reset[i];
    Write_Float_data(Cal_power_buff_first_address+i*4+DATA_SAVE_ADDRESS,cal_power_reset[i]);
    delay_ms(5);
    Write_Long_data(Cal_adc_buff_first_address+i*4+DATA_SAVE_ADDRESS,cal_adc_reset[i]);
    cal_adc[i]=cal_adc_reset[i];
    delay_ms(5);
  }
  for(i=0;i<7;i++)
  {
    wavelength_coeff[i]=wavelength_coeff_reset[i];
    Write_Float_data(Wavelength_coeff_first_address+i*4+DATA_SAVE_ADDRESS,wavelength_coeff_reset[i]);
    delay_ms(5);
  }
}
void read_rest_flag(void)
{
	unsigned char flag=0; 
	flag=READ_BYTE(REST_FLAG_ADDRESS,0xA1);
	if(flag!=0XAA)
	{  
		reset_cal_constant();
		Write_set_status(0xff);
	}
	else
	{
		load_cal_constant();
		Load_set_status();
	}
}

void load_cal_constant(void)
{
  Read_cal_power();
  Read_cal_adc();
  Read_wavelength_coeff();
  
}

void Write_set_status(unsigned char flag)
{
	unsigned char i=0;
	if(flag==1||flag==0xff)
	{		
	  WRITE_BYTE(REST_FLAG_ADDRESS,0XAA,0XA0); //写复位标志 
    //delay_ms(10);		
	}
	if(flag==2||flag==0xff)
	{
		for(i=0;i<20;i++)
		{ 			
			WRITE_BYTE(ADDRESS_SN+i,g_r_SN[i],0XA0);
			//delay_ms(10);
			if(g_r_SN[i]=='\0')
				break;
			if(g_r_SN[i]=='\r')
				break;
		}
  }
	if(flag==3||flag==0xff)
	{
		for(i=0;i<20;i++)
		{ 			
			WRITE_BYTE(ADDRESS_TYPE+i,g_r_OTDR_TYPE[i],0XA0);
			//delay_ms(10);
			if(g_r_OTDR_TYPE[i]=='\0')break;
			if(g_r_OTDR_TYPE[i]=='\r')break;
		}
	}
	if(flag==4||flag==0xff)
	{
		for(i=0;i<20;i++)
		{
			WRITE_BYTE(ADDRESS_HardWare_TYPE+i,g_r_HardWare_TYPE[i],0XA0);
			//delay_ms(10);
			if(g_r_HardWare_TYPE[i]=='\0')break;
			if(g_r_HardWare_TYPE[i]=='\r')break;
		}
	}
	if(flag==5||flag==0xff)
	{
		 Write_Float_data(APD_VBR_ADDRESS,g_r_APD_VBR);
	}
	
	
}
void Load_SN_status(void)
{
	unsigned char i=0;
//	unsigned long read_long_data=0;
	unsigned char flag=0;
//	dBug_information_print_flag=READ_BYTE(ADDRESS_Debug_flag,0xA1);
	for(i=0;i<20;i++)
	{
		delay_us(10);
		g_r_SN[i]=READ_BYTE(ADDRESS_SN+i,0XA1);
		//if(g_r_SN[i]==0XFF)g_r_SN[i]='*';
		if(g_r_SN[i]=='\0'||g_r_SN[i]=='\r')
		{
			flag=1;
		}
		if(flag)
		{
			g_r_SN[i++]='\r';
			g_r_SN[i++]='\n';
			break;
		}
	}
}



void Load_set_status(void)
{
	unsigned char i=0;
	unsigned long read_long_data=0;
	unsigned char flag=0;
//	dBug_information_print_flag=READ_BYTE(ADDRESS_Debug_flag,0xA1);
	for(i=0;i<20;i++)
	{
		delay_us(10);
		g_r_SN[i]=READ_BYTE(ADDRESS_SN+i,0XA1);
		g_r_SN_temp[i]=g_r_SN[i];
		if(g_r_SN[i]==0XFF)
		{
			g_r_SN[i]='*';
			g_r_SN_temp[i]='*';
		}
		if(g_r_SN[i]=='\0'||g_r_SN[i]=='\r')
		{
			flag=1;
		}
		if(flag)
		{
			g_r_SN[i++]='\r';
			g_r_SN[i++]='\n';
			i=i-2;
			g_r_SN_temp[i++]='\r';
			g_r_SN_temp[i++]='\n';
			break;
		}
	}
	flag=0;
	for(i=0;i<20;i++)
	{
		delay_us(10);
		g_r_OTDR_TYPE[i]=READ_BYTE(ADDRESS_TYPE+i,0XA1);
		if(g_r_OTDR_TYPE[i]==0XFF)
		{
			g_r_OTDR_TYPE[i]='*';
		}
		if(g_r_OTDR_TYPE[i]=='\0'||g_r_OTDR_TYPE[i]=='\r')
		{
			flag=1;
		}
		if(flag)
		{
			g_r_OTDR_TYPE[i++]='\r';
			g_r_OTDR_TYPE[i++]='\n';
			break;
		}
		//if()break;
	}
	flag=0;
	for(i=0;i<20;i++)
	{
		delay_us(10);
		g_r_HardWare_TYPE[i]=READ_BYTE(ADDRESS_HardWare_TYPE+i,0XA1);
		if(g_r_HardWare_TYPE[i]==0XFF)
		{
			g_r_HardWare_TYPE[i]='*';
		}
		if(g_r_HardWare_TYPE[i]=='\0'||g_r_HardWare_TYPE[i]=='\r')
		{
			flag=1;
		}
		if(flag)
		{
			g_r_HardWare_TYPE[i++]='\r';
			g_r_HardWare_TYPE[i++]='\n';
			break;
		}
	}
	g_r_APD_VBR=READ_Float_data(APD_VBR_ADDRESS);
	if(g_r_APD_VBR>=90||g_r_APD_VBR<=24)g_r_APD_VBR=42;
//	read_long_data=READ_LONG_DATA(Fiber_alive_lever_ADDRESS);
//	if(read_long_data<=65535)Fiber_alive_lever=read_long_data;
}


unsigned int IIC_RX_DATA_W_TMP (void)
{
    unsigned int Rn=8,DATA=0,DATA1=0,RR=0x80;
    SDA_IN; // ?????
    delay_us (50);
    for (;Rn!=0;Rn--)
    {
        delay_us (100);
        SCL_HIGH;
        
        delay_us (100);
        if ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6))!=0)
            DATA|=RR;
        else
            DATA&=~RR;
        
        RR>>=1;
        SCL_LOW;
        
    }
    delay_us (50);
    SDA_OUT;
    SDA_LOW;
    //SCL_HIGH;
    delay_us (50);
    SCL_HIGH;
    delay_us (50);
    SCL_LOW;
    SDA_IN;
    delay_us (50);
    Rn=8;
    RR=0x80;
    for (;Rn!=0;Rn--)
    {
       delay_us (100);
       SCL_HIGH;
        
        delay_us (100);
        if ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6))!=0)
            DATA1|=RR;
        else
            DATA1&=~RR;
        
        RR>>=1;
        SCL_LOW;
        
    }
    delay_us (50);
     SDA_OUT;//????
    SDA_LOW;
    //SCL_HIGH;
    delay_us (50);
    SCL_HIGH;
    delay_us (50);
    SCL_LOW;
    SDA_IN;
    delay_us (50);
    
    
    delay_us (50);
    SDA_OUT;
    DATA=DATA*256+DATA1;
    DATA=DATA>>4;
    return DATA;
}

//////////////TMP175操作///////////////////
void WRITE_BYTE_TMP (unsigned char Rigith,unsigned char DATA_W,unsigned char Chip_slect)
{
    unsigned char Chip_slect1=(Chip_slect&0xFE);

    SDA_OUT;
	 delay_us (50);
    IIC_START();
	delay_us (50);
    IIC_TX_DATA(Chip_slect1); // E2PROM ??
	delay_us (50);

    IIC_TX_DATA(Rigith); // ??256BIT?EEPROM??????
	delay_us (50);
    IIC_TX_DATA(DATA_W);
	delay_us (50);
    IIC_STOP();
    delay_us (0x3F00); // ?????
   // PORTC&=0X40;
}

unsigned int READ_WORD_TMP (unsigned char ADDR,unsigned char Chip_slect)
{
    unsigned int DATA_R=0;
    unsigned char Chip_slect1=(Chip_slect&0xFE);
    

    
     SDA_OUT;
     IIC_START();
	  IIC_TX_DATA(Chip_slect1);
    delay_us(50);
    IIC_TX_DATA(ADDR);

    
    delay_us(50);
   IIC_START();
   // SDA_IN;
    IIC_TX_DATA(Chip_slect);
    delay_us(50);
   // SDA_IN;
    DATA_R=IIC_RX_DATA_W_TMP();
   IIC_STOP();

   SDA_OUT;
    return DATA_R;
}

float GET_TMP_PCB_DB(void)
{
  unsigned int read_data=0;
  float temp=0;
  read_data=READ_WORD_TMP(0X00,0X93);
  if(read_data<=0x7ff)
  {
    temp=read_data;
    temp=temp*0.0625;
  }
  else
  {
    temp=0xfff-read_data;
    temp=-temp*0.0625;
  }
  return temp;
}








