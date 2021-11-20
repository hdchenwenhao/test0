#include "Include.h"
u8 SendFlag;
u8 WlanStatus = 0;
u8 NetType;
u8 DA_Num= 0;
u8 RxBuffer[10];//接收缓冲
u8 SendFinish = 1;
u8 TxBuf[10];
#define WIFI_SPI_RX_BUFF_SIZE 512
unsigned char g_r_WIFI_SPI_RX_BUFF[WIFI_SPI_RX_BUFF_SIZE];
unsigned int  g_r_WIFI_SPI_RX_BUFF_point=0;
unsigned int g_r_WIFI_read_word=0;
unsigned char g_r_WIFI_Read_opm_type=0;
unsigned char g_r_WIFI_SPI_DATA_flag;
extern unsigned char dBug_information_print_flag;

void CleanBuf(void)
{
	u8 ClearCounter;
	for(ClearCounter=0;ClearCounter<10;ClearCounter++)
		RxBuffer[ClearCounter]=0;
	for(ClearCounter=0;ClearCounter<10;ClearCounter++)
		TxBuf[ClearCounter]=0;	 
}
//******************************************************************
//函数名：  SPI_RECIVE_DATA
//功能：    SPI从wifi模块接收的数据解析
//输入参数：无
//返回值：  无
//******************************************************************
void SPI_RECIVE_DATA(void)
{
    u8 databuf[64];
    u8 Len1,Len2;
    u8 CodeType;
    u16 datalen;
	INT16U temp = 0;
	INT16U i;
    INT8U tempdata;
	WM_SPI_RX_DESC* rxdesc;
	SPINSS(0);
	SPIReadWriteByte(SPI_REG_INT_STTS);		//查询SPI_INT_HOST_STTS
	temp |= SPIReadWriteByte(0xff);			//读寄存器，字节序为小端
	temp |= SPIReadWriteByte(0xff) << 8;
	SPINSS(1);
	if((temp != 0xffff) && (temp & 0x01))	//数据或命令已经准备好
	{
		SPINSS(0);
		SPIReadWriteByte(SPI_REG_RX_DAT_LEN);	//查询RX_DAT_LEN                                                                                                                                                                                                             
		temp |= SPIReadWriteByte(0xff);
		temp |= SPIReadWriteByte(0xff) << 8;
		SPINSS(1);

		if(temp > 0)
		{
			if(temp % 4)
			{
				temp = ((temp + 3) / 4) << 2;
			}
			//SPI_PRINT("debug rx len = %d\r\n",temp);
			rxdesc = SPIGetRxBuff(temp);
			if(rxdesc)
			{
				SPINSS(0);
				SPIReadWriteByte(SPI_CMD_RX_DATA);	//读数据命令
				for(i = 0; i < temp; i++)
				{
					*(rxdesc->buff+ i) = SPIReadWriteByte(0xff);
                   // printf("[%d]=[%x]\r\n", i, *(rxdesc->buff + i));
				}
				SPINSS(1);
			}
            else
            {
                SPINSS(0);
                SPIReadWriteByte(SPI_CMD_RX_DATA);	//读数据命令
                for(i = 0; i < temp; i++)
                {
                    tempdata = SPIReadWriteByte(0xff);
       //         	SPI_PRINT("[%d]=[%x]\r\n", i, *(rxdesc->buff + i));
                }
                SPINSS(1);
            }

		}
	}
            //判断数据类型
            if(*(rxdesc->buff + 0) == 0xAA )
            {
                 Len1=*(rxdesc->buff + 2);
                 Len2=*(rxdesc->buff + 3);            
                 datalen=((Len1<<8)+Len2);			
//                 DesdAddress = *(rxdesc->buff + 6);
//                 printf("\r\nSNY:%02X\r\n",*(rxdesc->buff + 0));
//                 printf("Type:%02X\r\n",(*(rxdesc->buff + 1)));
//                 printf("Len1:%02X\r\n",Len1);
//                 printf("Len2:%02X\r\n",Len2);				 
//                 printf("L:%04X\r\n",datalen);
//                 printf("SN:%02X\r\n",*(rxdesc->buff + 4));
//                 printf("FLG:%02X\r\n",*(rxdesc->buff + 5));
//                 printf("DA:%02X\r\n",*(rxdesc->buff + 6));
				 DA_Num = *(rxdesc->buff + 6);// & 0x40
//                 printf("CHK:%02X\r\n",*(rxdesc->buff + 7));
                 if(*(rxdesc->buff + 1)==0x01)
                 {
					 u8 j = 0;
//                        printf("MT:%02X\r\n",*(rxdesc->buff + 8));
//                        printf("CODE:%02X\r\n",*(rxdesc->buff + 9));
//                        printf("ERR:%02X\r\n",*(rxdesc->buff + 10));
//                        printf("EXT:%02X\r\n",*(rxdesc->buff + 11));
	
//                          printf("\r\n");
                         CodeType = *(rxdesc->buff + 9);
                         switch(CodeType)
                         {
                             case RI_CMD_SKCT://创建Socket
                                 DesdAddress =  *(rxdesc->buff + 12);
//								 printf("Socket Number is %d\r\n",DesdAddress);
                             break;
                             
                             case RI_CMD_LINK_STATUS://网络连接状态
                                 if(*(rxdesc->buff + 12) == 1)
                                 {
                                    WlanStatus = 1;
																	 if(dBug_information_print_flag)
                                    printf("IP is:%d.%d.%d.%d\r\n",*(rxdesc->buff + 13),
                                     *(rxdesc->buff + 14),*(rxdesc->buff + 15),*(rxdesc->buff + 16));
                                    if(dBug_information_print_flag)  
                                    printf("netmask is::%d.%d.%d.%d\r\n",*(rxdesc->buff + 17),
                                     *(rxdesc->buff + 18),*(rxdesc->buff + 19),*(rxdesc->buff + 20));
                                     if(dBug_information_print_flag) 
                                    printf("gateway is::%d.%d.%d.%d\r\n",*(rxdesc->buff + 21),
                                     *(rxdesc->buff + 22),*(rxdesc->buff + 23),*(rxdesc->buff + 24));
                                     if(dBug_information_print_flag) 
                                    printf("DNS1 is::%d.%d.%d.%d\r\n",*(rxdesc->buff + 25),
                                     *(rxdesc->buff + 26),*(rxdesc->buff + 27),*(rxdesc->buff + 28));
                                      if(dBug_information_print_flag)
                                    printf("DNS2 is::%d.%d.%d.%d\r\n\r\n",*(rxdesc->buff + 29),
                                     *(rxdesc->buff + 30),*(rxdesc->buff + 31),*(rxdesc->buff + 32));
                                 }
                                 else
                                 {
                                    WlanStatus = 0;
																	  if(dBug_information_print_flag)
																	  printf("WlanStatus=0\r\n");
                                 }
                             break;
                             
                             case RI_EVENT_LINKUP://网络层状态
                                 DesdAddress =  *(rxdesc->buff + 12);
                             break;
                             
                             case RI_CMD_WPRT://网络类型
                                 if(*(rxdesc->buff + 12) == 0)
                                 {
                                    NetType = Net_STA;
                                 }
                                 else if(*(rxdesc->buff + 12) == 1)
                                 {
                                    NetType = Net_adhoc;
                                 }
                                 else if(*(rxdesc->buff + 12) == 2)
                                 {
                                    NetType = Net_AP;
                                 }
																// printf("NetWork_RI_CMD_WPRT is:    %d\r\n",*(rxdesc->buff + 12));
																// NetType=0;
                             break;
								 
							 case RI_CMD_MAC://查询MAC
								  if(dBug_information_print_flag)
								 printf("\r\nMAC:");
								 for(j = 0;j < 6;j++)
							    {
									MacBuf[j] =  *(rxdesc->buff + 12 + j);
								 if(dBug_information_print_flag)
									printf("%2X",MacBuf[j]);
								}
								 if(dBug_information_print_flag)printf("\r\n");
                break;
               case RI_CMD_SKSTT:
								 	
								 if(dBug_information_print_flag)printf("Socket:");
								for(i=0;i<datalen;i++)
								 { 
									 if(dBug_information_print_flag) printf("%02X",*(rxdesc->buff + 12 + i));
								 }
								 if(dBug_information_print_flag) printf("\r\n");
                             break;    
							 
                             default:
                                 
                             break;
                         }
                            
                 }
                 
                 else if(*(rxdesc->buff + 1)==0x03)
                 {                
                    if(*(rxdesc->buff + 2)==0x81)
                    {
                        if(dBug_information_print_flag) printf("Empty Buffer Number is %d\r\n",databuf[4]);
                    }
                    if(*(rxdesc->buff + 2)==0x82)
                    {
                        if(dBug_information_print_flag) printf("%d datas\r\n",*(rxdesc->buff + 3));
                    }       
                   if(dBug_information_print_flag) printf("\r\n");	
                 }
                 
                 else if(*(rxdesc->buff + 1)==0x00)
                 { 
					u16 i=0;
                     //printf("%d,",datalen);         
//					printf("DA_Num:%02X\r\n",DA_Num);
					if(DA_Num > 0x40)//UDP
					{
						NetWorkType = 1;
						UdpSocketnum = DA_Num & 0x3F;
//						printf("UDP Message:");
						 if(dBug_information_print_flag)printf("UDP:");
						for(i = 0; i < 8; i++)
						{
							 if(dBug_information_print_flag)printf("\r%2X",*(rxdesc->buff + 8 + i));
						}
//						printf("\r\n");
						
//						printf("Receive UDP Data is:");
						for(i = 0; i < datalen; i++)
						{
							 if(dBug_information_print_flag)printf("\r%2X",*(rxdesc->buff + 16 + i));
						}
						 if(dBug_information_print_flag)printf("\r\n");
											//收到APP请求发送MAC 由于是UDP通讯，第9到第16字节是远端的IP 本地端口 远端端口
					//所以用户数据是从第17字节，也就是指针的16地址开始
						if( (*(rxdesc->buff + 16 ) == 0x06) && (*(rxdesc->buff + 17 ) == 0x4D)  
						&& (*(rxdesc->buff + 18 ) == 0x42)  && (*(rxdesc->buff + 19 ) == 0x43))
						{	
	//						printf("Send MAC\r\n");
							RISocketUDPSend(UdpSocketnum, MacBuf, 6);
						} 					 
	  
						 //手动APP请求传图 Udp协议
						if( (*(rxdesc->buff + 16 ) == 0x03) && (*(rxdesc->buff + 17 ) == 0xFF) )
						{
	//						printf("Recive Send Pictrue\r\n");
							if(SendFinish == 1)
							{
								SendFlag = 1;
							}
							else
							{
								SendFlag = 0;
							}
						} 
						
					}
					else//TCP
					{
						NetWorkType = 0;
						TcpSocketnum = DA_Num & 0x3F;
//						printf("AA TcpSocketnum is \r%02X\r\n",TcpSocketnum);
//						printf("Receive TCP Data is:");
						if(dBug_information_print_flag)
						  printf("TCP:");
						g_r_WIFI_SPI_RX_BUFF_point=0;
						for(i = 0; i < datalen; i++)
						{
							NetWorkType = 1;
							if(dBug_information_print_flag)
							  printf("\r%2X",*(rxdesc->buff + 8 + i));
							g_r_WIFI_SPI_RX_BUFF[g_r_WIFI_SPI_RX_BUFF_point++]=*(rxdesc->buff + 8 + i);
							if(g_r_WIFI_SPI_RX_BUFF_point>=WIFI_SPI_RX_BUFF_SIZE)
							{
								g_r_WIFI_SPI_RX_BUFF_point=0;
								 if(dBug_information_print_flag)printf("WIFI RECEIVE DATA TOO LONG-->>erro!");
								break;
							}
						}
						if(g_r_WIFI_SPI_RX_BUFF_point)g_r_WIFI_SPI_DATA_flag=0xff;
						if(dBug_information_print_flag) printf("\r\n");
						RxBuffer[0] = *(rxdesc->buff + 8 );
						RxBuffer[1] = *(rxdesc->buff + 9 );
					}
 					
//                    printf("\r\n");	
                 }
          }//if AA
    SPIFreeRxBuff(rxdesc);
}


