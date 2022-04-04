/**************************************************************************************************
  Filename:       xx_example.c
  Revised:        $Date: 2021-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.1.0(2 stacks)

  Copyright 2022 lu.
                       
***************************************************************************************************/
#include "xxapp/xx_main.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _XX_HAL_H
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
*                                               MACRO
*                                               宏定义
***************************************************************************************************/
//#define xxHalPrint                                   emberAfAppPrint
//#define xxHalPrintBuffer                             emberAfAppPrintBuffer

#define xxHalPrint(pstring, ...)
#define xxHalPrintBuffer(x,y,z)

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
void XxHalPoll(uint32 sysTimeMillisecond)
{
    if( XX_SERIAL_TEST_MODE_FLAG )
        return;
    #ifdef _XX_HAL_TIMER_H
    //XxHalTimerPoll(sysTimeMillisecond);
    #endif

    #ifdef _XX_HAL_KEY_H
    //XxHalKeyPoll(sysTimeMillisecond);
    #endif

    #ifdef _XX_HAL_LED_H
    //XxHalLedPoll(sysTimeMillisecond);
    #endif
}

void XxHalInit(void)
{
    #ifdef _XX_HAL_TIMER_H
    //XxHalTimerInit();
    #endif
    
    #ifdef _XX_HAL_KEY_H
    //XxHalKeyInit();
    #endif

    #ifdef _XX_HAL_LED_H
    //XxHalLedInit();
    #endif

}

//void XxMainStart1(void)
//{
//    XxMainInitCbRegister(XxHalInit);
//    //XxMainTickCbRegister(XxHalPoll);
//}

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


