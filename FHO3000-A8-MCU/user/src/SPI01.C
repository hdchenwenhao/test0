#include <include.h>

WM_SPI_RX_DESC gsSPIRxDesc[SPI_RX_DESC_NUM];
//******************************************************************
//函数名：  spiGpioInit
//功能：    wifi模块IO初始化
//输入参数：无
//返回值：  无
//******************************************************************
void spiGpioInit(void)
{
    
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOB, ENABLE); //| RCC_APB2Periph_AFIO
  /* SPI1 Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  //Disable JTAG   
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable , ENABLE);//GPIO_Remap_SWJ_JTAGDisable 
    
////    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
////    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
////    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////    GPIO_Init(GPIOB, &GPIO_InitStructure);
////	
////	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
////    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
////    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  //Disable JTAG   
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable , ENABLE);//GPIO_Remap_SWJ_JTAGDisable 
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* EXTI line gpio config(PA2) */	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;    
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING	 GPIO_Mode_IPU
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    
      //!< Configure WIFI_CS_PIN pin: WIFI CS pin 
    GPIO_InitStructure.GPIO_Pin = WIFI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WIFI_CS_PORT, &GPIO_InitStructure);
		
		GPIO_SetBits(WIFI_CS_PORT,WIFI_CS_PIN);//
		
		
}
//******************************************************************
//函数名：  spiCfg
//功能：    wifi模块SPI模式初始化
//输入参数：SPIx：wifi模块使用的SPI接口  prescaler：SPI分频值
//返回值：  无
//******************************************************************
void spiCfg(SPI_TypeDef* SPIx, uint16_t prescaler)
{
	SPI_InitTypeDef  SPI_InitStructure;
    memset(gsSPIRxDesc, 0, sizeof(gsSPIRxDesc));
	SPI_Cmd(SPIx, DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  
	SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;//SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStructure);
	/*Enable SPIx.NSS as a GPIO*/
	//SPI_SSOutputCmd(SPIx, ENABLE);//这条语句是使得spi的cr2寄存器ssoe位置1，软件模式无需使用
	SPI_Cmd(SPIx, ENABLE);
}
//******************************************************************
//函数名：  WIFI_NVIC_Configuration
//功能：    wifi模块中断IO初始化
//输入参数：无
//返回值：  无
//******************************************************************
void WIFI_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
//******************************************************************
//函数名：  WIFI_EXTI_Config
//功能：    wifi模块中断IO初始化
//输入参数：无
//返回值：  无
//******************************************************************
void WIFI_EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;										
	
	/* EXTI line(PG2) mode config */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource2); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 
/* config the NVIC(PG2) */
	WIFI_NVIC_Configuration();
    
}
//******************************************************************
//函数名：  spiInit
//功能：    spi初始化
//输入参数：无
//返回值：  无
//******************************************************************
void spiInit(void)
{
	spiGpioInit();
	
    spiCfg(SPI2, SPI_BaudRatePrescaler_2);//
	
}
//******************************************************************
//函数名：  SPIReadWriteByte
//功能：    SPI读写一个字节
//输入参数：TxData:要写入的字节
//返回值：  ret:读取到的字节
//******************************************************************
INT8U SPIReadWriteByte(INT8U TxData)
{		
	INT16U retry = 0;	
	INT8U ret;
	while(0 == (SPI2->SR & 1 << 1)) //等待发送区空	
	{
		retry++;
		if(retry > SPI_REG_TIMEOUT)
		{
			SPI_PRINT("debug SPI Send Data TIMEOUT\r\n");
			return 0;
		}
	}			   
	SPI2->DR = TxData; //发送一个byte 
	retry = 0;
	while(0 == (SPI2->SR & 1 << 0)) //等待接收完一个byte  
	{
		retry++;
		if(retry > SPI_REG_TIMEOUT)
		{
			SPI_PRINT("debug SPI Send Data TIMEOUT\r\n");
			return 0;
		}
	}	  						    
	ret = SPI2->DR; //返回收到的数据			    
	return ret;
}
//******************************************************************
//函数名：  SPINSS
//功能：    SPI片选
//输入参数：status：1、失能    0：使能
//返回值：  无
//******************************************************************
void SPINSS(INT8U status)
{
	if(status)
	{
		WIFI_CS_Dis();
	}
	else
	{
		WIFI_CS_EN();
	}
}
//******************************************************************
//函数名：  SPIGetRxBuff
//功能：    申请数据内存
//输入参数：size：数据大小
//返回值：  无
//******************************************************************
WM_SPI_RX_DESC* SPIGetRxBuff(INT32U size)
{
	INT32U i;

	for(i = 0; i < SPI_RX_DESC_NUM; i++)
	{
		if(0 == gsSPIRxDesc[i].valid)
		{
			gsSPIRxDesc[i].buff = WM_MALLOC(size);
			if(gsSPIRxDesc[i].buff)
			{
				gsSPIRxDesc[i].valid = 1;
				return &gsSPIRxDesc[i];
			}
			else
			{
				SPI_PRINT("debug SPIGetRxBuff malloc err\r\n");
				return NULL;
			}
		}
	}
	SPI_PRINT("debug SPIGetRxBuff gsSPIRxDesc err\r\n");
	return NULL;
//	INT32U i;

//	for(i = 0; i < SPI_RX_DESC_NUM; i++)
//	{
//		if(0 == gsSPIRxDesc[i].valid)
//		{
//			gsSPIRxDesc[i].valid = 1;
//			return &gsSPIRxDesc[i];
//		}
//	}

//	return NULL;
}
//******************************************************************
//函数名：  SPIFreeRxBuff
//功能：    释放内存
//输入参数：desc：释放内存目标
//返回值：  无
//******************************************************************
void SPIFreeRxBuff(WM_SPI_RX_DESC* desc)
{
	if(NULL == desc)
	{
		return;
	}
	if(desc->buff)
	{
		WM_FREE(desc->buff);
		desc->buff = NULL;
	}
	desc->valid = 0;
	return;
//    
//    desc->valid = 0;
//	return;
}
//******************************************************************
//函数名：  SPIRxData
//功能：    SPI从wifi模块获取数据
//输入参数：无
//返回值：  无
//******************************************************************
void SPIRxData(void)
{
	INT16U temp = 0;
	INT16U i;
	WM_SPI_RX_DESC* rxdesc;
    INT8U tempdata;
	printf("Enter Rxdata\r\n");
	SPINSS(0);
	SPIReadWriteByte(SPI_REG_INT_STTS);		//查询SPI_INT_HOST_STTS
	temp |= SPIReadWriteByte(0xff);			//读寄存器，字节序为小端
	temp |= SPIReadWriteByte(0xff) << 8;
	SPINSS(1);
    printf("SPIRxData temp is %d\r\n",temp);
	if((temp != 0xffff) && (temp & 0x01))	//数据或命令已经准备好
	{
//        printf("enter if.....................\r\n");
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
			SPI_PRINT("debug rx len = %d\r\n",temp);
			rxdesc = SPIGetRxBuff(temp);
			if(rxdesc)
			{
				SPINSS(0);
				SPIReadWriteByte(SPI_CMD_RX_DATA);	//读数据命令
				for(i = 0; i < temp; i++)
				{
					*(rxdesc->buff+ i) = SPIReadWriteByte(0xff);
//					SPI_PRINT("[%d]=[%x]\r\n", i, *(rxdesc->buff + i));
                    printf("[%d]=[%x]\r\n", i, *(rxdesc->buff + i));
				}
				SPINSS(1);
				//SPIFreeRxBuff(temp);
//				AppSendMsg(MSG_SPI, (INT32U)rxdesc);
			}
		}
        else
        {
            SPINSS(0);
            SPIReadWriteByte(SPI_CMD_RX_DATA);	//读数据命令
            for(i = 0; i < temp; i++)
            {
                tempdata = SPIReadWriteByte(0xff);
            //	SPI_PRINT("[%d]=[%x]\r\n", i, *(rxdesc->buff + i));
            }
            SPINSS(1);
        }
	}
}
//******************************************************************
//函数名：  SPITxCmd
//功能：    往wifi模块发送精简指令
//输入参数：TXBuf：发送缓存  CmdLen：命令长度
//返回值：  无
//******************************************************************
INT8U SPITxCmd(INT8U *TXBuf, INT16U CmdLen)
{
//	INT8U temp = 0;
	INT16U temp = 0;
	INT16U i;
	INT32U retry = 0;
	
	if(NULL == TXBuf)
	{
		SPI_PRINT("debug SPITxCmd buff == NULL\r\n");
		return 0;
	}
	SPINSS(0);
    
	while((temp != 0xffff) && (0 == (temp & 0x02)))	
	{
//        u8 pData[2]={0};
		retry++;
//        pData[0]=SPI_REG_TX_BUFF_AVAIL;
		SPIReadWriteByte(SPI_REG_TX_BUFF_AVAIL);		//查询TX_BUFF_AVAIL
        
		temp |= SPIReadWriteByte(0xff);					//读寄存器，字节序为小端
		temp |= SPIReadWriteByte(0xff) << 8;
        
		delay_ms(5);
//        printf("temp is %d\r\n",temp);
		if(retry > SPI_TIMEOUT)
		{
			SPI_PRINT("debug SPI Send CMD TIMEOUT\r\n");
			return 0;
		}
	}
	SPINSS(1);
	if(CmdLen > 0)
	{
		if(CmdLen % 4)
		{
			CmdLen = ((CmdLen + 3) / 4) << 2;
		}
//		SPI_PRINT("debug TX_BUFF_AVAIL = %d, cmdlen=%d\r\n", temp, CmdLen);
		SPINSS(0);
		SPIReadWriteByte(SPI_CMD_TX_CMD);	//写命令命令
		for(i = 0; i < CmdLen; i ++)
		{
			SPIReadWriteByte(*(TXBuf + i));
//			printf("S 0x%2X",*(TXBuf + i));
		}
		SPINSS(1);
	}

	return 1;	
}
//******************************************************************
//函数名：  SPITxData
//功能：    通过SPI发送数据到wifi模块
//输入参数：TXBuf：发送缓存  DataLen：数据长度
//返回值：  无
//******************************************************************
INT8U SPITxData(INT8U *TXBuf, INT16U DataLen)
{
	u16 temp = 0;
	u16 i;
	u16 retry=0;
	
	if(NULL == TXBuf)
	{
		return 0;
	}
	SPINSS(0);
	//while((temp != 0xffff) && (0 == (temp & 0x01)))	
	while(((temp != 0xffff) && (0 == (temp & 0x01))))//||temp<DataLen)	
	{
		retry ++;
		temp=0;
		SPIReadWriteByte(SPI_REG_TX_BUFF_AVAIL);	//查询TX_BUFF_AVAIL
		temp |= SPIReadWriteByte(0xff);				//读寄存器，字节序为小端
		temp |= SPIReadWriteByte(0xff) << 8;
		delay_ms(1);
		if(retry > SPI_TIMEOUT)
		{
			printf("WIFI BUFF ERRO !!!\r\n");
			return 0;
		}
	}
//	printf("WIFI BUFF SIZE:%2d\r\n",temp);
	SPINSS(1);
	if(DataLen > 0)
	{
		if(DataLen % 4)
		{
			DataLen = ((DataLen + 3) / 4) << 2;
		}
		SPINSS(0);
		SPIReadWriteByte(SPI_CMD_TX_DATA);	//写数据命令
		for(i = 0; i < DataLen; i ++)
		{
			SPIReadWriteByte(*(TXBuf + i));
		}
		SPINSS(1);
	}	
    
	return 1;
}


INT8U SPITxData1(INT8U *TXBuf, INT16U DataLen)//原始版本
{
	u16 temp = 0;
	u16 i;
	u16 retry=0;
	
	if(NULL == TXBuf)
	{
		return 0;
	}
	SPINSS(0);
	while((temp != 0xffff) && (0 == (temp & 0x01)))	
	{
		retry ++;
		SPIReadWriteByte(SPI_REG_TX_BUFF_AVAIL);	//查询TX_BUFF_AVAIL
		temp |= SPIReadWriteByte(0xff);				//读寄存器，字节序为小端
		temp |= SPIReadWriteByte(0xff) << 8;
		delay_ms(1);
		if(retry > SPI_TIMEOUT)
		{
			return 0;
		}
	}
	SPINSS(1);
	if(DataLen > 0)
	{
		if(DataLen % 4)
		{
			DataLen = ((DataLen + 3) / 4) << 2;
		}
		SPINSS(0);
		SPIReadWriteByte(SPI_CMD_TX_DATA);	//写数据命令
		for(i = 0; i < DataLen; i ++)
		{
			SPIReadWriteByte(*(TXBuf + i));
		}
		SPINSS(1);
	}	
    
	return 1;
}

u8 ReciveState;
extern u8 InterruptNum;
//////////WIFI模块收数据中断***********************//
void EXTI2_IRQHandler(void)
{
  //  printf("Interrupt_WIFI_Receive_data\r\n");
	
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
		{
					EXTI_ClearITPendingBit(EXTI_Line2);
					ReciveState=1; 
//			    printf("WIFI_SPI中断\r\n");
					if( InterruptNum == 0 )
					{
						SPI_RECIVE_DATA();
					}
					else
					{
						
					}
		 }
		
   
}



