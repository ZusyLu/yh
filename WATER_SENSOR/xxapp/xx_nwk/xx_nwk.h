/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                                  .
***************************************************************************************************/
#ifndef _XX_NWK_H
#define _XX_NWK_H

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
#define XX_NWK_LEAVE_NETWORK_INTERVAL_TIME                          (1500)
#define XX_NWK_LEAVE_NETWORK_MIN_TIME                               3000
#define XX_NWK_LEAVE_NETWORK_MAX_TIME                               8000


#define XX_NWK_KEY_PRESS_NUMBER                                         8
#define XX_NWK_UNICASE_SEND_METHOD                                       0
#define XX_NWK_BROADCAST_SEND_METHOD                                     1
#define XX_NWK_MULTICAST_SEND_METHOD                                     2

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern uint8 XxNwkSendRfPacket(uint8 srcEp, uint8 dstEp, uint16 dstAddr, uint8 frameControl, uint16 clusterID, uint8 command, uint8 *buf, uint8 len);
extern uint8_t XxNwkMultipleModeSendRfPacket(uint8_t srcEp, uint8_t dstEp, uint16_t dstAddr, uint8_t sendMethod, uint8_t frameControl, uint16_t clusterID, uint8_t command, uint8_t *buf, uint8_t len);
extern void XxNwkNetWorkStatus(void);
extern void XxZbStackStatusCallback(EmberStatus status);
extern void xxNwkKeepAliveFunction( void );
extern uint8_t XxReportSpecificAttributeEx(uint16_t   u16ClusterID,  uint8_t  u8AttrNums,   uint16_t   *u16AttrID,\
                                    uint8_t u8SrcEndPoint,    uint8_t  u8DestEndPoint,    uint16_t   ManufacturerCode);
/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/


extern uint8_t Xx_nwk_key_cunt;
extern uint8_t Xx_nwk_net_leave_flat;
extern uint32_t Xx_nwk_leave_start_time;
extern EmberApsFrame globalApsFrame;
/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


