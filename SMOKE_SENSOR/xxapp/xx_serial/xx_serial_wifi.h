/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                                   .
***************************************************************************************************/
#ifndef _XX_SERIAL_WIFI_H
#define _XX_SERIAL_WIFI_H

#ifdef __cplusplus
extern "C"
{
#endif
/***************************************************************************************************
*                                               NOTE
*                                               说明
***************************************************************************************************/
//-- source insight使用UTF-8解码，配合SimpliCity Studio开发环境，File->Reload As Encoding...->UTF-8
//-- source insight设置默认解码格式，Option->Preferences->Files->Default encoding->UTF-8

//-- 打印串口默认使用PA05(TX),PA06(RX)，与WIFI通讯的串口PC0(TX),PC2(RX)，AT指令串口，暂定PC0(TX),PC2(RX)

/***************************************************************************************************
*                                             INCLUDES
*                                            系统头文件
***************************************************************************************************/

/***************************************************************************************************
*                                             INCLUDES
*                                            用户头文件
***************************************************************************************************/
#include "xxapp/xx_main.h"

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
//#define XX_SERIAL_WIFI_PORT_NUM                             XX_SERIAL_COM_USART2

#define XX_SERIAL_WIFI_TX_POLL_TIME_MILLISECOND             100






    

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef void (*XxSerialWifiCommandCb_t) (uint8 frameIndex, uint8 frameType, uint8 cmd, uint8 cmdOpt, uint8 *data, uint16 len);

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/
extern uint8  xx_serial_wifi_packet_index;

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void   XxSerialWifiHandshakeClear(void);
extern uint8  XxSerialWifiHandshakeStatus(void);
extern uint8  XxSerialWifiCommandPoll(void *com, uint32 sysTimeMillisecond);
extern uint8  XxSerialWifiFramePacket(uint8 frameVersion, uint8 frameIndex, uint8 frameType, uint8 *data, uint16 len, uint8 retryDelaySecond, uint8 retryCount);
extern void   XxSerialWifiCommandCbRegister(XxSerialWifiCommandCb_t f);
extern void   XxSerialWifiFrameDelByIndex(uint8 frameIndex);
extern void   XxSerialWifiFrameDelByFrameType(uint8 frameType);


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


