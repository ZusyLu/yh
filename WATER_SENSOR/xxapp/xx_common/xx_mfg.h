/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                               .
***************************************************************************************************/
#ifndef _XX_MFG_H
#define _XX_MFG_H

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
//mfg model event 1~14 
#define XX_MFG_ERASS_FLASH_EVENT                              1
#define XX_MFG_WRITE_TRIAD_EVENT                              2
#define XX_MFG_READ_TRIAD_EVENT                               3
#define XX_MFG_READ_MAC_EVENT                                 4
#define XX_MFG_GPIO_TEST_INIT_EVENT                           5
#define XX_MFG_GPIO_TEST_EVENT                                6
#define XX_MFG_READ_version_EVENT                             7
#define XX_MFG_RF_TEST_EVENT                                  8


//event time 
#define XX_MFG_ERASS_FLASH_EVENT_TIME                         500
#define XX_MFG_WRITE_TRIAD_EVENT_TIME                         500
#define XX_MFG_READ_TRIAD_EVENT_TIME                          500
#define XX_MFG_READ_MAC_EVENT_TIME                            500
#define XX_MFG_GPIO_TEST_INIT_EVENT_TIME                      500
#define XX_MFG_GPIO_TEST_EVENT_TIME                           300
#define XX_MFG_READ_version_EVENT_TIME                        500
#define XX_MFG_RF_TEST_EVENT_TEIME                            500



/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern int8_t xx_mfg_rssi;
extern void xxMfgDelayEventMfgModelFunction(uint8_t index, uint8_t DelayEvent);



/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


