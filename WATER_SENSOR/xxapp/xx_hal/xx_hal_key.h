/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.
                                                  .
***************************************************************************************************/
#ifndef _XX_HAL_KEY_H
#define _XX_HAL_KEY_H

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
/* Interrupt option - Enable or disable */
#define XX_HAL_KEY_INTERRUPT_DISABLE                            0x00
#define XX_HAL_KEY_INTERRUPT_ENABLE                             0x01

/* Key state - shift or nornal */
#define XX_HAL_KEY_STATE_NORMAL                                 0x00
#define XX_HAL_KEY_STATE_SHIFT                                  0x01

#define XX_HAL_KEY_1                                            0x01
#define XX_HAL_KEY_2                                            0x02
#define XX_HAL_KEY_3                                            0x04
#define XX_HAL_KEY_4                                            0x08
#define XX_HAL_KEY_5                                            0x10
#define XX_HAL_KEY_6                                            0x20
#define XX_HAL_KEY_7                                            0x40
#define XX_HAL_KEY_8                                            0x80

/* Joystick */
#define XX_HAL_KEY_UP                                           0x01
#define XX_HAL_KEY_RIGHT                                        0x02
#define XX_HAL_KEY_CENTER                                       0x04
#define XX_HAL_KEY_LEFT                                         0x08
#define XX_HAL_KEY_DOWN                                         0x10

/* Buttons */  
#define XX_HAL_PUSH_BUTTON_RIGHT                                0x01
#define XX_HAL_PUSH_BUTTON_LEFT                                 0x02
#define XX_HAL_PUSH_BUTTON_SELECT                               0x04
#define XX_HAL_KEY_BUTTON_UP                                    0x40
#define XX_HAL_KEY_BUTTON_DOWN                                  0x80

#define XX_HAL_INPUT_IO_PULL_UP                                 1                       //-- 表示上拉
#define XX_HAL_INPUT_IO_PULL_DOWN                               0                       //-- 表示下拉

#define XX_HAL_INPUT_IO_STATE_PULL                              1                       //-- 表示上下拉模式
#define XX_HAL_INPUT_IO_STATE_TRISTATE                          0                       //-- 表示三态模式
    
#define XX_HAL_INT_IO_EDGE_RISING                               1                       //-- 表示上升沿中断触发
#define XX_HAL_INT_IO_EDGE_FALLING                              0                       //-- 表示下降沿中断触发
    
#define XX_HAL_KEY_MODE_INTERRUPT                               1                       //-- 表示该按钮是中断模式
#define XX_HAL_KEY_MODE_POLL                                    0                       //-- 表示该按钮是轮询模式
        
#define XX_HAL_KEY_ACTIVE_LOW                                   !                       //-- 表示低有效，则IO为上拉
#define XX_HAL_KEY_ACTIVE_HI                                    !!                      //-- 表示高有效，则IO为下拉

/***************************************************************************************************
*                                               MACRO
*                                               宏定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef uint8 (*XxHalKeyCBack_t) (uint8 curKeys, uint8 preKeys, uint32 pollTimeMsec);

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/
extern uint16_t Xx_hal_key_press_time_count[];
extern uint16_t Xx_hal_key_release_time_count[];

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void   XxHalKeyInit(void);
extern void   XxHalKeyPoll(uint32 sysTimeMillisecond);

extern void   XxHalKeyRegister( XxHalKeyCBack_t cback );                              //-- Register for all key events
extern void   XxHalKeySetScanTimeout(uint32 timeoutMsec);
extern uint32 XxHalKeyGetScanTime(void);
extern void   XxHalKeyConfig( bool interruptEnable, const XxHalKeyCBack_t cback);     //-- Configure the Key Service
extern uint8  XxHalKeyRead( void);                                                    //-- Read the Key status
extern void   XxHalKeyEnterSleep ( void );                                            //-- Enter sleep mode, store important values
extern uint8  XxHalKeyExitSleep ( void );                                             //-- Exit sleep mode, retore values
extern bool   XxHalKeyPressed( void );                                                //-- This is for internal used by hal_sleep
extern uint8  XxHal_key_keys(void);                                           
extern uint8  XxHal_key_int_keys(void);


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif

