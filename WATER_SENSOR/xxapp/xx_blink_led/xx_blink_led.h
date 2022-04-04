/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                .
***************************************************************************************************/
#ifndef _XX_BLINK_LED_H
#define _XX_BLINK_LED_H

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
/***************************************************************************************************
*                                               MACRO
*                                             全局宏定义
***************************************************************************************************/
#define XX_BLINK_LED_INIT

#define XX_BLINK_LED_BSP_LED0_PIN                          (4U)
#define XX_BLINK_LED_BSP_LED0_PORT                         (gpioPortA)
#define XX_BLINK_LED_BSP_LED0                               0

#define XX_BLINK_LED_BSP_LED1_PIN                          (0U)
#define XX_BLINK_LED_BSP_LED1_PORT                         (gpioPortA)
#define XX_BLINK_LED_BSP_LED1                               1

#define XX_BLINK_LED_BSP_LED2_PIN                          (0U)
#define XX_BLINK_LED_BSP_LED2_PORT                         (gpioPortC)
//#define XX_BLINK_LED_BSP_LED2                               2

#define XX_BLINK_LED_BSP_LED3_PIN                          (1U)
#define XX_BLINK_LED_BSP_LED3_PORT                         (gpioPortC)
//#define XX_BLINK_LED_BSP_LED3                               3

#define XX_BLINK_LED_BSP_LED4_PIN                          (2U)
#define XX_BLINK_LED_BSP_LED4_PORT                         (gpioPortC)
//#define XX_BLINK_LED_BSP_LED4                               4



#define XX_BLINK_LED_BSP_LED_COUNT                         (2U)
#define XX_BLINK_LED_BSP_LED_INIT                          { { XX_BLINK_LED_BSP_LED0_PORT, XX_BLINK_LED_BSP_LED0_PIN },\
                                                             { XX_BLINK_LED_BSP_LED1_PORT, XX_BLINK_LED_BSP_LED1_PIN } }
                                                             //{ XX_BLINK_LED_BSP_LED2_PORT, XX_BLINK_LED_BSP_LED2_PIN },\
                                                            // { XX_BLINK_LED_BSP_LED3_PORT, XX_BLINK_LED_BSP_LED3_PIN },\
                                                             //{ XX_BLINK_LED_BSP_LED4_PORT, XX_BLINK_LED_BSP_LED4_PIN }}

#define XX_BLINK_LED_HAL_LED_COUNT                         (2U)
#define XX_BLINK_LED_HAL_LED_ENABLE                        {0,1} // { 0, 1 , 2 , 3 , 4 }
#define XX_BLINK_LED_BSP_LED_POLARITY                      (0)


#define XX_BLINK_LED_BLINK_TIME                            300
#define XX_BLINK_LED_FAST_BLINK_TIME                       100

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             全局变量
***************************************************************************************************/


#ifdef XX_BLINK_LED_BSP_LED0
extern EmberEventControl xxBlnkLed0Event;
#endif

#ifdef XX_BLINK_LED_BSP_LED1
extern EmberEventControl xxBlnkLed1Event;
#endif


/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void xxBlinkMultiLedBlinkBlink( uint8_t  count, uint16_t blinkTimeMs, uint8_t  led );
extern void xxBlinkMultiLedBlinkLedOn( uint8_t timeMs, uint8_t led );
extern void XxBlinkLedInit( void );






/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


