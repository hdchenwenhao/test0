#ifndef __SPIWIFI_H__
#define __SPIWIFI_H__

extern u8 DesdAddress;
extern u8 SendFlag;
extern u8 NetStatus;
void SPI_RECIVE_DATA(void);
void UserLayer_TxHandle_UserDat_AddrAdd(u8 *pData,u32 RemIP,u16 RemPort,u16 LocPort);
void UserLayer_TxHandle_UserDat_DataADDs(u8 *pData,u8 *DataAdd,u32 Length);
extern u8 WlanStatus;
extern u8 TcpSocketnum;//TCP SocketºÅ
extern u8 UdpSocketnum;//Udp SocketºÅ
extern u8 MacBuf[6];
void CleanBuf(void);
#define Net_STA			0
#define Net_adhoc		1
#define Net_AP		    2 

#endif


