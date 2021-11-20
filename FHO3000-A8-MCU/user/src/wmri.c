/***********************************************************************
*	INCLUDE FILES
***********************************************************************/
#include <Include.h>
#ifdef WM_DEMO_SPI
/***********************************************************************
*	APPLICATION GLOBALS
***********************************************************************/
static INT8U gsSeqNum = 0;
u8 RI_Send_SN = 0;
//******************************************************************
//函数名：  StringToIpaddr
//功能：    字符串转IP HEX
//输入参数：buf：字符串  addr：输出IP HEX
//返回值：  无
//******************************************************************
static int StringToIpaddr(const char *buf, INT8U *addr)
{
	int count = 0, rc = 0;
	int in[4];
	char c;

	rc = sscanf(buf, "%u.%u.%u.%u%c", &in[0], &in[1], &in[2], &in[3], &c);
	if (rc != 4 && (rc != 5 || c != '\n'))
	{
		return -1;
	}
	for (count = 0; count < 4; count++) 
	{
		if (in[count] > 255)
		{
			return -1;
		}
		addr[count] = in[count];
	}
	return 0;
}
//******************************************************************
//函数名：  RICheckSum
//功能：    计算校验和
//输入参数：data：需要计算的数据指针首地址  len：计算的数据长度
//返回值：  无
//******************************************************************
static INT8U RICheckSum(INT8U *data, INT16U len)
{
	INT16U i;
	INT16U sum = 0;
	
	for(i = 0; i < len; i++)
	{
		sum += *data++;
	}
	return sum;
}
//******************************************************************
//函数名：  RICreateCmdHeader
//功能：    精简指令加数据头组帧
//输入参数：WM_SPI_HDR *header：8字节帧头  cmd：精简指令内容  len：指令长度
//返回值：  无
//******************************************************************
static INT32U RICreateCmdHeader(WM_SPI_HDR *header, INT8U cmd, INT16U len)
{	
	if(NULL == header)
	{
		return 1;
	}
//	RI_PRINT("debug CMD cmd = 0x%x, len = %d\r\n", cmd, len);
	header->hdr.sync		= RI_CMD_SYNC;
	header->hdr.type		= RI_CMD;
	header->hdr.length		= Swap16(len + sizeof(WM_RI_CMD_HDR));
	header->hdr.seq_num		= gsSeqNum++;
	header->hdr.flag		= 0;
	header->hdr.dest_addr	= 0;
	header->hdr.chk			= RICheckSum(&header->hdr.type, 6);
	header->cmd.msg_type	= RI_CMD_MT_REQ;
	header->cmd.code		= cmd;
	header->cmd.err			= 0;
	if(len)
	{
		header->cmd.ext		= 1;
	}
	else
	{
		header->cmd.ext		= 0;
	}
	return 0;
}
//******************************************************************
//函数名：  RICreateCmdHeader
//功能：    精简指令加数据头组帧
//输入参数：WM_SPI_HDR *header：8字节帧头  cmd：精简指令内容  len：指令长度
//返回值：  无
//******************************************************************
static INT32U RICreateCmdHeaderUDP(WM_SPI_HDR *header, INT8U cmd, INT16U len)
{	
	if(NULL == header)
	{
		return 1;
	}
//	RI_PRINT("debug CMD cmd = 0x%x, len = %d\r\n", cmd, len);
	header->hdr.sync		= RI_CMD_SYNC;
	header->hdr.type		= RI_CMD;
	header->hdr.length		= Swap16(len + sizeof(WM_RI_CMD_HDR));
	header->hdr.seq_num		= gsSeqNum++;
	header->hdr.flag		= 0;
	header->hdr.dest_addr	= 0x40;
	header->hdr.chk			= RICheckSum(&header->hdr.type, 6);
	header->cmd.msg_type	= RI_CMD_MT_REQ;
	header->cmd.code		= cmd;
	header->cmd.err			= 0;
	if(len)
	{
		header->cmd.ext		= 1;
	}
	else
	{
		header->cmd.ext		= 0;
	}
	return 0;
}
//******************************************************************
//函数名：  RICmdQuery
//功能：    精简指令发送
//输入参数：cmd：精简指令内容
//返回值：  无
//******************************************************************
INT32U RICmdQuery(INT8U cmd)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, cmd, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
//******************************************************************
//函数名：  RISaveParam
//功能：    保存参数到Flash
//输入参数：无
//返回值：  无
//******************************************************************
INT32U RISaveParam(void)
{
	INT8U buff[32];
	WM_SPI_HDR *header;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_PMTF, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
//******************************************************************
//函数名：  RIResetDevice
//功能：    复位模块
//输入参数：无
//返回值：  无
//******************************************************************
INT32U RIResetDevice(void)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_RESET, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
//******************************************************************
//函数名：  RISetOneShotCfg
//功能：    设置一键配置
//输入参数：flag
//返回值：  无
//******************************************************************
INT32U RISetOneShotCfg(INT8U flag)
{
//	u8 i =0;
	INT8U buff[32];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = flag; 
	len++;
	RICreateCmdHeader(header, RI_CMD_ONESHOT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
//	for(i = 0;i < (sizeof(WM_SPI_HDR) + len);i++)
//	printf("%2X",buff[i]);
	return 0;
}
//******************************************************************
//函数名：  RISetWlanType
//功能：    设置网络类型
//输入参数：type：网络类型
//返回值：  无
//******************************************************************
INT32U RISetWlanType(INT32U type)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = type; 
	len++;
	RICreateCmdHeader(header, RI_CMD_WPRT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
//******************************************************************
//函数名：  RISetSSID
//功能：    设置SSID
//输入参数：ssid：ssid名字
//返回值：  无
//******************************************************************
INT32U RISetSSID(INT8U *ssid)
{
	INT8U buff[64];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	len = strlen((char *)ssid);
	buff[offset++] = len; 
	memcpy((void *)&buff[offset++], ssid, len);
	len++;
	RICreateCmdHeader(header, RI_CMD_SSID, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
//******************************************************************
//函数名：  RISetKey
//功能：    设置密码
//输入参数：type：密钥类型  index：密钥索引  key：密钥
//返回值：  无
//******************************************************************
INT32U RISetKey(INT8U type, INT8U index, INT8U *key)
{
	INT8U buff[96];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = type; 
	len++;
	buff[offset++] = index; 
	len++;
	buff[offset++] = strlen((char *)key); 
	len++;
	memcpy((void *)&buff[offset++], key, strlen((char *)key));
	len += strlen((char *)key);
	RICreateCmdHeader(header, RI_CMD_KEY, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
//******************************************************************
//函数名：  RISetEncrypt
//功能：    设置加密方式
//输入参数：type：密钥加密方式
//返回值：  无
//******************************************************************
INT32U RISetEncrypt(INT32U type)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = type; 
	len++;
	RICreateCmdHeader(header, RI_CMD_ENCRYPT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
//******************************************************************
//函数名：  RISetNip
//功能：    设置IP类型
//输入参数：dhcp：IP分配方式
//          ip:   IP地址
//          netmast：子网掩码
//          gateway：网关
//          dns：dns地址
//返回值：  无
//******************************************************************
INT32U RISetNip(INT8U dhcp, INT8U *ip, INT8U *netmast, INT8U *gateway, INT8U *dns)
{
	INT8U buff[96];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = dhcp; 
	len++;
//	if(dhcp)
//	{
		StringToIpaddr((char *)ip, &buff[offset]);
		offset += 4; len += 4;
		StringToIpaddr((char *)netmast, &buff[offset]);
		offset += 4; len += 4;
		StringToIpaddr((char *)gateway, &buff[offset]);
		offset += 4; len += 4;
		StringToIpaddr((char *)dns, &buff[offset]);
		offset += 4; len += 4;
//	}
	RICreateCmdHeader(header, RI_CMD_NIP, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;

}
//******************************************************************
//函数名：  RIWlanJoin
//功能：    加入/创建网络 STA模式加入网络；AP模式创建网络
//输入参数：无
//返回值：  无
//******************************************************************
INT32U RIWlanJoin(void)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_WJOIN, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}

INT32U RIWlanScan(void)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_WSCAN, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
	return 0;
}
//******************************************************************
//函数名：  RIGetWlanStatus
//功能：    获取网络连接状态
//输入参数：无
//返回值：  无
//******************************************************************
INT32U RIGetWlanStatus(void)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_LINK_STATUS, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
    
	return 0;
}
//******************************************************************
//函数名：  RISocketCreate
//功能：    创建socket
//输入参数：sk：socket信息
//返回值：  无
//******************************************************************
INT32U RISocketCreate(WM_SOCKET_INFO *sk)
{
	INT8U buff[64];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = sk->protocol; 
	len++;
	buff[offset++] = sk->cs; 
	len++;
	buff[offset++] = 4; 
	len++;
	StringToIpaddr((char *)sk->hostname, &buff[offset]);
	offset += 4; len += 4;
	*(INT16U *)&buff[offset] = Swap16(sk->remote);
	offset += 2; len += 2;
	*(INT16U *)&buff[offset] = Swap16(sk->local);
	offset += 2; len += 2;
	RICreateCmdHeader(header, RI_CMD_SKCT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
//******************************************************************
//函数名：  RISocketCreateUDP
//功能：    创建socket
//输入参数：sk：socket信息
//返回值：  无
//******************************************************************
INT32U RISocketCreateUDP(WM_SOCKET_INFO *sk)
{
	INT8U buff[64];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = sk->protocol; 
	len++;
	buff[offset++] = sk->cs; 
	len++;
	buff[offset++] = 4; 
	len++;
	StringToIpaddr((char *)sk->hostname, &buff[offset]);
	offset += 4; len += 4;
	*(INT16U *)&buff[offset] = Swap16(sk->remote);
	offset += 2; len += 2;
	*(INT16U *)&buff[offset] = Swap16(sk->local);
	offset += 2; len += 2;
	RICreateCmdHeaderUDP(header, RI_CMD_SKCT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}
//******************************************************************
//函数名：  RISocketClose
//功能：    关闭socket连接
//输入参数：fd：socket信息
//返回值：  无
//******************************************************************
INT32U RISocketClose(INT32U fd)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = fd; 
	len++;
	RICreateCmdHeader(header, RI_CMD_SKCLOSE, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
} 
//******************************************************************
//函数名：  RIGetSocketReceiveLen
//功能：    获取当前收到的数据长度
//输入参数：fd：socket信息
//返回值：  无
//******************************************************************
INT32U RIGetSocketReceiveLen(INT32U fd)
{
	INT8U buff[32];
	u8 i=0;
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = fd; 
	len++;
	if(NetWorkType == 0)
		RICreateCmdHeader(header, RI_CMD_SKSTT, len);
	else if(NetWorkType == 1)
		RICreateCmdHeaderUDP(header, RI_CMD_SKSTT, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
//	for(i = 0;i<sizeof(WM_RI_HDR) + len;i++)
//		printf("-%2X",buff[i]);
	return 0;
}
//******************************************************************
//函数名：  RICreateDataHeader
//功能：    数据头组帧
//输入参数：WM_SPI_HDR *header：8字节帧头  fd：socket信息  len：指令长度  type：数据类型
//返回值：  无
//******************************************************************
INT32U RICreateDataHeader(WM_RI_HDR *header, INT32U fd, INT16U len, INT16U type)
{
	if(NULL == header)
	{
		return 1;
	}
//	RI_PRINT("debug CMD cmd = 0x%x, len = %d\r\n", cmd, len);
	header->sync		= RI_CMD_SYNC;
	header->type		= RI_DATA;
	header->length		= Swap16(len);
	header->seq_num		= gsSeqNum++;
	header->flag		= 0;
	header->dest_addr	= fd & 0x3F;
	if(type)
	{
		header->dest_addr |= 0x40; 
	}
	header->chk = RICheckSum(&header->type, 6);
	return 0;
}
//******************************************************************
//函数名：  RICreateDataHeader
//功能：    数据头组帧
//输入参数：WM_SPI_HDR *header：8字节帧头  fd：socket信息  len：指令长度  type：数据类型
//返回值：  无
//******************************************************************
INT32U RICreateDataHeaderUDP(WM_RI_HDR *header, INT32U fd, INT16U len, INT16U type)
{
	if(NULL == header)
	{
		return 1;
	}
//	RI_PRINT("debug CMD cmd = 0x%x, len = %d\r\n", cmd, len);
	header->sync		= RI_CMD_SYNC;
	header->type		= RI_DATA;
	header->length		= Swap16(len);
	header->seq_num		= gsSeqNum++;
	header->flag		= 0;
	header->dest_addr	= 0x7F & fd;//fd & 0x3F;
	if(type)
	{
		header->dest_addr |= 0x40; 
	}
	header->chk = RICheckSum(&header->type, 6);
	return 0;
}
//创建Socket 
//mode： 模式（Sever/Client） 
//SocketHM Server时表示超时连接时间，Clent写server的IP地址
//LoPort：本地端口
//RePort：对端端口
INT32U AppCreatSocketProc(enum socket_protocol ProMode,enum socket_cs_mode mode,char *SocketHM,u16 LoPort,u16 RePort)
{
	WM_SOCKET_INFO sk;
	INT32U ret = 0;

	memset(&sk, 0, sizeof(WM_SOCKET_INFO));
	sk.protocol = ProMode;//SOCKET_PROTO_TCP
	sk.cs =  mode;
    strcpy((char *)sk.hostname, SocketHM);
//	if(ProMode ==SOCKET_PROTO_TCP)
		sk.remote = RePort;
	sk.local = LoPort;
	if(ProMode ==SOCKET_PROTO_TCP)
		ret = RISocketCreate(&sk);
	else if(ProMode ==SOCKET_PROTO_UDP)
		ret = RISocketCreateUDP(&sk);
	return ret;
}
//******************************************************************
//函数名：  RISocketTCPSend
//功能：    socket发送数据
//输入参数：fd：socket信息  data：发送的数据  len：数据长度
//返回值：  无
//******************************************************************
INT32U RISocketTCPSend(INT32U fd, INT8U *data, INT16U len)
{
	INT8U buff[600];
	WM_RI_HDR *header;
	unsigned int temp=0;
	unsigned int time_over_count=0;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_RI_HDR *)buff;
	memcpy((void *)&buff[sizeof(WM_RI_HDR)], data, len);
	RICreateDataHeader(header, fd, len, 0);
//	SPINSS(0);
//	while(temp<sizeof(WM_RI_HDR) + len)
//	{
//	  SPIReadWriteByte(SPI_REG_TX_BUFF_AVAIL);	//查询TX_BUFF_AVAIL
//		temp |= SPIReadWriteByte(0xff);				//读寄存器，字节序为小端
//		temp |= SPIReadWriteByte(0xff) << 8;
//	  SPINSS(1);
//	}
	while(temp==0)
	{
    temp=	SPITxData(buff, sizeof(WM_RI_HDR) + len);
		time_over_count++;
		delay_ms(2);
		if(time_over_count>=10)
		{
			return 0;
		}
	}
	return 1;
}
//******************************************************************
//函数名：  RISocketTCPSend
//功能：    socket发送数据
//输入参数：fd：socket信息  data：发送的数据  len：数据长度
//返回值：  无
//******************************************************************
INT32U RISocketUDPSend(INT32U fd, INT8U *data, INT16U len)
{
	INT8U buff[600];
	u8 i;
	WM_RI_HDR *header;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_RI_HDR *)buff;
	
	RICreateDataHeaderUDP(header, fd, len, 0);
//	UserLayer_TxHandle_UserDat_AddrAdd((void *)&buff[sizeof(WM_RI_HDR)],0xC0A80168,0x1778,0x1778);

//	memcpy((void *)&buff[sizeof(WM_RI_HDR)+8], data, len);
//	
//	SPITxData(buff, sizeof(WM_RI_HDR) + len + 8);
	memcpy((void *)&buff[sizeof(WM_RI_HDR)], data, len);
	
	SPITxData(buff, sizeof(WM_RI_HDR) + len);
//	for(i = 0;i<sizeof(WM_RI_HDR) + len;i++)
//		printf("-%2X",buff[i]);
	return 1;
}

//******************************************************************
//函数名：  TCPSend
//功能：    socket发送数据
//输入参数：fd：socket信息  data：发送的数据  len：数据长度
//返回值：  无
//******************************************************************
//INT32U TCPSend(INT32U fd, INT8U *data, INT16U len)
//{
//	INT8U buff[64];
//	WM_RI_HDR *header;

//	memset((char *)buff, 0, sizeof(buff));
//	header = (WM_RI_HDR *)buff;
//	memcpy((void *)&buff[sizeof(WM_RI_HDR)], data, len);
//	RICreateDataHeader(header, fd, len, 0);
//	SPITxData(buff, sizeof(WM_RI_HDR) + len);
//	return 1;
//}
INT RI_Send_Msg(u8 *pData, INT Type , u32 Datalen)
{
	WM_RI_HDR header; 
	memset(&header, 0, sizeof(WM_RI_HDR));

	header.sync = RI_CMD_SYNC;
	header.type = Type;
	header.length = Swap16(Datalen);
	header.seq_num = (RI_Send_SN++==0)? RI_Send_SN++ : (RI_Send_SN-1);
	header.chk = RICheckSum((u8 *)(&header.type),sizeof(WM_RI_HDR)-2);	

    memcpy(pData,&(header.sync),sizeof(WM_RI_HDR));
	return 0;
}
//******************************************************************
//函数名：  UserLayer_TxHandle_UserDat_ExtADDs
//功能：    用户数据组帧函数，将地址信息组帧到pData
//输入参数：pData：数据指针  RemIP：对端IP
//          RemPort:对端端口 LocPort：本机端口
//SocketNum：TCP连接Socket号 TCPorUDP：0-TCP 1-UDP
//返回值：  无
//******************************************************************
void UserLayer_TxHandle_UserDat_AddrAdd(u8 *pData,u32 RemIP,u16 RemPort,u16 LocPort)
{    
    pData[0]=(RemIP>>24)&0xFF;
    pData[1]=(RemIP>>16)&0xFF;
    pData[2]=(RemIP>>8)&0xFF;
    pData[3]=(RemIP)&0xFF;
    
    pData[4]=(RemPort>>8)&0xFF;
    pData[5]=(RemPort)&0xFF;
    pData[6]=(LocPort>>8)&0xFF;
    pData[7]=(LocPort)&0xFF;
}
//******************************************************************
//函数名：  UserLayer_TxHandle_UserDat_ExtADDs
//功能：    用户数据组帧函数，将地址信息组帧到pData
//输入参数：pData：数据指针  RemIP：对端IP
//          RemPort:对端端口 LocPort：本机端口
//SocketNum：TCP连接Socket号 TCPorUDP：0-TCP 1-UDP
//返回值：  无
//******************************************************************
void UserLayer_TxHandle_UserDat_DataADDs(u8 *pData,u8 *DataAdd,u32 Length)
{
    u16 i;
    for(i=0;i<Length;i++)
    {
        pData[i]=DataAdd[i];
    }
}
#endif

//******************************************************************
//函数名：  RISetSSID
//功能：    设置SSID
//输入参数：ssid：ssid名字
//返回值：  无
//******************************************************************
INT32U wifi_RI_CMD_DNAME(INT8U *name)
{
	INT8U buff[64];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	len = strlen((char *)name);
	buff[offset++] = len; 
	memcpy((void *)&buff[offset++], name, len);
	len++;
	RICreateCmdHeader(header, RI_CMD_DNAME, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
	return 0;
}


//设置一个服务器地址，作为目标地址
void Set_server_address(unsigned char protocol,unsigned char cs,INT8U *ip,unsigned int port)
{
		INT8U buff[96];
	WM_SPI_HDR *header;
	INT16U len = 0, offset;

	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	offset = sizeof(WM_SPI_HDR);
	buff[offset++] = protocol; 
	len++;
	buff[offset++] = cs; 
	len++;
//	if(dhcp)
//	{
		StringToIpaddr((char *)ip, &buff[offset]);
		offset += 4; len += 4;
		buff[offset++] = port/256; 
	  len++;
	  buff[offset++] = port%256; 
	  len++;
//	}
	RICreateCmdHeader(header, RI_CMD_ATRM, len);
	SPITxCmd(buff, sizeof(WM_SPI_HDR) + len);
//	return 0;
}

INT32U RIGetServerStatus(void)
{
	INT8U buff[32];
	WM_SPI_HDR *header;
	
	memset((char *)buff, 0, sizeof(buff));
	header = (WM_SPI_HDR *)buff;
	RICreateCmdHeader(header, RI_CMD_ATRM, 0);
	SPITxCmd(buff, sizeof(WM_SPI_HDR));
    
	return 0;
}

void SET_WIFI_STA_LINK_HUAWEI(void)
{
	wifi_RI_CMD_DNAME("FHO2000");
	delay_ms(1000);
	delay_ms(1000);
	//delay_ms(1000);
	//delay_ms(1000);
	RISetWlanType(0);
	printf("RISetWlanType:STA\r\n");
	delay_ms(1000);
	//RISetSSID("HUAWEI001");
  RISetSSID("OTDR_FHO2001");
	//printf("SSID:HUEWEI001\r\n");
	printf("SSID:OTDR_FHO2001\r\n");
	delay_ms(1000);
	//delay_ms(1000);
	//delay_ms(1000);
	RISetKey(1, 0,"1234567890");
	delay_ms(1000);
	//delay_ms(1000);
	//delay_ms(1000);
	RISetNip(1, "192.168.1.120","255.255.255.0", "192.168.1.1", "192.168.1.1");
	//RISetNip(1, "192.168.43.120","255.255.255.0", "192.168.43.1", "192.168.43.1");
  delay_ms(1000);
	//delay_ms(1000);
	//delay_ms(1000);
	RISaveParam();
	 delay_ms(1000);
	// delay_ms(1000);
	// delay_ms(1000);
	RIResetDevice();
	delay_ms(1000);
	//delay_ms(1000);
	//delay_ms(1000);
	printf("RESET WIFI\r\n");
	RIWlanJoin();
	delay_ms(1000);
	//delay_ms(1000);
	//delay_ms(1000);
	printf("join OTDR_FHO2001!\r\n");
}

