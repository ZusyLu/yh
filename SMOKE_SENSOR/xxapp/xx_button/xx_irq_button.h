/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                                           .
***************************************************************************************************/
#ifndef _XX_IRQ_BUTTON_H
#define _XX_IRQ_BUTTON_H

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
*                                               MACRO
*                                            全局宏定义
***************************************************************************************************/
//
#define XX_IRQ_BUTOON_INIT     //
#define XX_IRQ_BSP_BUTTON0_PIN                             (3U)
#define XX_IRQ_BSP_BUTTON0_PORT                            (gpioPortA)
//#define XX_IRQ_BSP_BUTTON0                                 (0U)

    
#define XX_IRQ_BSP_BUTTON1_PIN                             (1U)
#define XX_IRQ_BSP_BUTTON1_PORT                            (gpioPortB)
//#define XX_IRQ_BSP_BUTTON1                                 (1U)

    
#define XX_IRQ_BSP_BUTTON2_PIN                             (0U)
#define XX_IRQ_BSP_BUTTON2_PORT                            (gpioPortA)
//#define XX_IRQ_BSP_BUTTON2                                 (3U)

#define XX_IRQ_BSP_BUTTON3_PIN                             (4U)
#define XX_IRQ_BSP_BUTTON3_PORT                            (gpioPortA)
//#define XX_IRQ_BSP_BUTTON3                               (4U)

#define XX_IRQ_BSP_BUTTON4_PIN                             (4U)
#define XX_IRQ_BSP_BUTTON4_PORT                            (gpioPortA)
//#define XX_IRQ_BSP_BUTTON4                               (4U)

#define XX_IRQ_BUTTON_COUNT                                (0U)
#define XX_IRQ_BSP_BUTTON_CUNT                             (0U)

#define XX_IRQ_BUTTON_PRESSED                              1
#define XX_IRQ_BUTTON_RELEASED                             0



#define XX_IRQ_BSP_BUTTON_INIT                             {}//{{ XX_IRQ_BSP_BUTTON0_PORT, XX_IRQ_BSP_BUTTON0_PIN ,XX_IRQ_BSP_BUTTON0 } }
                                                            //{ XX_IRQ_BSP_BUTTON1_PORT, XX_IRQ_BSP_BUTTON1_PIN ,XX_IRQ_BSP_BUTTON1 },\
                                                            //{ XX_IRQ_BSP_BUTTON2_PORT, XX_IRQ_BSP_BUTTON2_PIN ,XX_IRQ_BSP_BUTTON2 },
                                                            //{ XX_IRQ_BSP_BUTTON3_PORT, XX_IRQ_BSP_BUTTON3_PIN ,XX_IRQ_BSP_BUTTON3 },\
                                                            //{ XX_IRQ_BSP_BUTTON4_PORT, XX_IRQ_BSP_BUTTON4_PIN ,XX_IRQ_BSP_BUTTON4 }}
#define XX_IRQ_BSP_BUTTON_GPIO_DOUT                        (HAL_GPIO_DOUT_LOW)
#define XX_IRQ_BSP_BUTTON_GPIO_MODE                        (HAL_GPIO_MODE_INPUT_PULL)//(HAL_GPIO_MODE_INPUT)
						
#define XX_IRQ_BUTTON_ENABLE                               { 0 }


/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void xxIrqButtonInit( void );







/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


