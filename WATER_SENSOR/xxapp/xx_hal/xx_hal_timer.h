/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                                                .
***************************************************************************************************/
#ifndef _XX_HAL_TIMER_H
#define _XX_HAL_TIMER_H

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
*                                               宏定义
***************************************************************************************************/
#define HAL_TIMER_ENTER_CRITICAL_SECTION(x)                     st( __disable_irq(); )
#define HAL_TIMER_EXIT_CRITICAL_SECTION(x)                      st( __enable_irq(); )
#define XX_HAL_TIME_EVENT_TIME_COVER                            1
#define XX_HAL_TIME_EVENT_TIME_NOT_COVER                        0
/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef void (*XxHalTimerEventCb_t) (void);

typedef struct tagXxHalTimer
{
    uint8  taskId;
    uint16 event;
    uint32 timeout;
    uint32 reloadTimeout;
    XxHalTimerEventCb_t f;
    struct tagXxHalTimer *next;
} XxHalTimer_t;

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void   XxHalTimerInit(void);
extern void   XxHalTimerPoll(uint32 sysTimeMillisecond);
extern uint8  XxHalTimerEventStart(uint8 taskId, uint16 event, uint32 timeout, XxHalTimerEventCb_t f);
extern uint8  XxHalTimerEventSet(uint8 taskId, uint16 event, bool whetherCoverTime,uint32 timeout, XxHalTimerEventCb_t f);
extern uint8  XxHalTimerEventStop(uint8 taskId, uint16 event);
extern uint8  XxHalTimerActive(uint8 taskId, uint16 event);

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


