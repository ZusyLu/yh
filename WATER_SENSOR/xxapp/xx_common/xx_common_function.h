/**************************************************************************************************
Filename:       xx_example.h
Revised:        $Date: 202-3-27 18:29
Revision:       $Revision: V1.0.0

Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

Copyright 2022 lu.      
***************************************************************************************************/
#ifndef _XX_COMMON_FUNCTION_H
#define _XX_COMMON_FUNCTION_H

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
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern uint8  XxFuncHex2Asc(uint8 h, uint8 *c);
extern uint8  XxFuncAsc2Hex(uint8 c, uint8 *h);
extern void   XxFuncByte2HexStr(const unsigned char* source, char* dest, int sourceLen);
extern void   XxFuncHex2Str(const char *sSrc,  char *sDest, uint8 nSrcLen, uint8 format);
extern uint8  XxFuncStr2Hex(const char* source, unsigned char* dest, int sourceLen);
extern uint8  XxFuncStr2Num(const char* source, unsigned long *num, int sourceLen);
extern uint8  XxFuncDataPointCheck(uint8 *data, uint16 len);
extern uint8  XxFuncDataPointPick(uint8 dpID, uint8 dpIDLen, uint8 *data, uint16 *len, uint8 *dpData, uint16 *dpDataLen, uint8 littleEndianMode, uint8 delFlag, uint8 print);
extern void   XxFuncDataPointPrint(uint8 id, uint8 dataType, uint8 *data, uint16 len);
extern uint16 XxFuncDataPointPacket(uint8 dpID, uint8 dpType, uint8 *dpData, uint16 dpLen, uint8 *buf, uint16 *len);
extern uint8  XxFuncBuildZCLAttrPayload(uint8 *buf, uint8 *len,uint16 attrID, uint8 status, uint8 dataType, const uint8 *data, uint8 dataLen);
extern uint32_t XxjitterTimeDelayMs( uint32_t maxRanomNumber, uint32_t basicTime );

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


