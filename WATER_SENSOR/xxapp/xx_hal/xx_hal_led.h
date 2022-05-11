/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.   .
***************************************************************************************************/
#ifndef _XX_HAL_LED_H
#define _XX_HAL_LED_H

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
#define XX_HAL_LED_BLINK

#define XX_HAL_LED_NONE                                 0x00
#define XX_HAL_LED_1                                    0x01
#define XX_HAL_LED_2                                    0x02
#define XX_HAL_LED_3                                    0x04
#define XX_HAL_LED_4                                    0x08
#define XX_HAL_LED_5                                    0x10
#define XX_HAL_LED_6                                    0x20
#define XX_HAL_LED_7                                    0x40
#define XX_HAL_LED_8                                    0x80
#define XX_HAL_LED_ALL                                  0x7f           //-- LED8 for other
    
#define XX_HAL_LED_MODE_OFF                             0x00
#define XX_HAL_LED_MODE_ON                              0x01
#define XX_HAL_LED_MODE_BLINK                           0x02
#define XX_HAL_LED_MODE_FLASH                           0x04
#define XX_HAL_LED_MODE_TOGGLE                          0x08
    
#define XX_HAL_LED_DEFAULT_MAX_LEDS                     8               //-- modify by matthew
#define XX_HAL_LED_DEFAULT_DUTY_CYCLE                   5
#define XX_HAL_LED_DEFAULT_FLASH_COUNT                  25              //-- modify by matthew,before is 50
#define XX_HAL_LED_DEFAULT_FLASH_TIME                   1000
#define XX_HAL_LED_ALWAYS_FLASH_CUNT                    0XFF
    
/***************************************************************************************************
*                                               MACRO
*                                               宏定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef struct 
{
    uint8  mode;                                                        /* Operation mode */
    uint8  todo;                                                        /* Blink cycles left */
    uint8  onPct;                                                       /* On cycle percentage */
    uint16 time;                                                        /* On/off cycle time (msec) */
    uint32 next;                                                        /* Time for next change */
} XxHalLedControl_t;

typedef struct
{
    XxHalLedControl_t HalLedControlTable[XX_HAL_LED_DEFAULT_MAX_LEDS];
    uint8           sleepActive;
} XxHalLedStatus_t;

#ifdef XX_HAL_LED_BLINK
extern XxHalLedStatus_t XxHalLedStatusControl;
#endif

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void   XxHalLedInit(void);                                       //-- Initialize LED Service.
extern void   XxHalLedPoll(uint32_t sysTimeMillisecond);
extern uint8  XxHalLedSet( uint8 led, uint8 mode );                     //-- Set the LED ON/OFF/TOGGLE.
extern void   XxHalLedBlink( uint8 leds, uint8 cnt, uint8 duty, uint16 time, uint8 donotChangeFirstState);//-- Blink the LED.
extern void   XxHalLedEnterSleep( void );                               //-- Put LEDs in sleep state - store current values
extern void   XxHalLedExitSleep( void );                                //-- Retore LEDs from sleep state
extern uint8  XxHalLedGetState( void );                                 //-- Return LED state
extern uint8  XxHalLedBlinkGetCount(uint8 leds);                        //-- Get Led Blink Count
extern uint8  XxHalLedFindBlinkEvent(void);                             //-- Find Led Blink Event
extern void   XxHalLedBlinkAddCount(uint8 leds, uint8 numBlinks);       //-- Add Led Blink Count
extern void   XxHalLedBlinkClear(uint8 leds);                           //-- Clear Led Blink Count
extern uint8  XxHalLedBlinkGetLeds(void);
extern void   XxHalLedOnOff(uint8 leds, uint8 mode);

extern void   XxExternalFuncLedBlinkUpdate(uint8 leds, uint8 numBlinks, uint8 mode, uint8 end);
extern void   XxExternalFuncLedBlinkEnd(uint8 leds, uint8 numBlinks, uint8 mode, uint8 end);

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


