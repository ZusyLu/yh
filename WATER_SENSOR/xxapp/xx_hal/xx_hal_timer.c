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

#ifdef _XX_HAL_TIMER_H
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

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/
XxHalTimer_t *timerHead = 0;
uint32 xx_hal_timer_time_backup = 0;

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
XxHalTimer_t *xxHalFindTimer(uint8 taskId, uint16 event_flag)
{
    XxHalTimer_t *srchTimer;

    // Head of the timer list
    srchTimer = timerHead;

    // Stop when found or at the end
    while ( srchTimer )
    {
        if (srchTimer->taskId == taskId && srchTimer->event == event_flag)
        {
            break;
        }

        // Not this one, check another
        srchTimer = srchTimer->next;
    }

    return ( srchTimer );
}

uint8 XxHalTimerActive(uint8 taskId, uint16 event)
{
    return xxHalFindTimer(taskId, event)!=0 ? 1 : 0;
}

uint8 XxHalTimerEventStop(uint8 taskId, uint16 event)
{
    XxHalTimer_t *foundTimer;

    HAL_TIMER_ENTER_CRITICAL_SECTION(0);

    // Find the timer to stop
    foundTimer = xxHalFindTimer(taskId, event);
    if ( foundTimer )
    {
        // Clear the event flag and XxHalTimerPoll() will delete the timer from the list.
        foundTimer->event = 0;
    }

    HAL_TIMER_EXIT_CRITICAL_SECTION(0);

    return ( (foundTimer != NULL) ? 1 : 0 );
}

//uint8 XxHalTimerEventStartReload(uint16 event, uint32 timeout_value )
//{
//  XxHalTimer_t *newTimer;

//  HAL_TIMER_ENTER_CRITICAL_SECTION(0);

//  // Add timer
//  newTimer = osalAddTimer( taskID, event_id, timeout_value );
//  if ( newTimer )
//  {
//        // Load the reload timeout value
//        newTimer->reloadTimeout = timeout_value;
//  }

//  HAL_TIMER_EXIT_CRITICAL_SECTION(0);

//  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
//}

uint8 XxHalTimerEventStart(uint8 taskId, uint16 event, uint32 timeout, XxHalTimerEventCb_t f)
{
    XxHalTimer_t *newTimer;
    XxHalTimer_t *srchTimer;

    HAL_TIMER_ENTER_CRITICAL_SECTION(0);

    // Look for an existing timer first
    newTimer = xxHalFindTimer(taskId, event);
    if ( newTimer )
    {
        // Timer is found - update it.
        newTimer->timeout = timeout;
    }
    else
    {
        // New Timer
        HAL_TIMER_EXIT_CRITICAL_SECTION(0);
        newTimer = XxMemAlloc( sizeof( XxHalTimer_t ) );
        HAL_TIMER_ENTER_CRITICAL_SECTION(0);
    
        if ( newTimer )
        {
            // Fill in new timer
            newTimer->taskId = taskId;
            newTimer->event = event;
            newTimer->timeout = timeout;
            newTimer->reloadTimeout = 0;
            newTimer->f = f;
            newTimer->next = (void *)NULL;
        
            // Does the timer list already exist
            if ( timerHead == NULL )
            {
                // Start task list
                timerHead = newTimer;
            }
            else
            {
                // Add it to the end of the timer list
                srchTimer = timerHead;
        
                // Stop at the last record
                while ( srchTimer->next )
                {
                    srchTimer = srchTimer->next;
                }
        
                // Add to the list
                srchTimer->next = newTimer;
            }
        }
    }

    HAL_TIMER_EXIT_CRITICAL_SECTION(0);

    return ( (newTimer != NULL) ? 1 : 0 );
}

// same as XxHalTimerEventStart
uint8 XxHalTimerEventSet(uint8 taskId, uint16 event, bool whetherCoverTime,uint32 timeout, XxHalTimerEventCb_t f)
{
    XxHalTimer_t *newTimer;
    XxHalTimer_t *srchTimer;

    HAL_TIMER_ENTER_CRITICAL_SECTION(0);

    // Look for an existing timer first
    newTimer = xxHalFindTimer(taskId, event);
    if ( newTimer )
    {
        if ( whetherCoverTime )
        {
            // Timer is found - update it.
            newTimer->timeout = timeout;
        }
    }
    else
    {
        // New Timer
        HAL_TIMER_EXIT_CRITICAL_SECTION(0);
        newTimer = XxMemAlloc( sizeof( XxHalTimer_t ) );
        HAL_TIMER_ENTER_CRITICAL_SECTION(0);
    
        if ( newTimer )
        {
            // Fill in new timer
            newTimer->taskId = taskId;
            newTimer->event = event;
            newTimer->timeout = timeout;
            newTimer->reloadTimeout = 0;
            newTimer->f = f;
            newTimer->next = (void *)NULL;
        
            // Does the timer list already exist
            if ( timerHead == NULL )
            {
                // Start task list
                timerHead = newTimer;
            }
            else
            {
                // Add it to the end of the timer list
                srchTimer = timerHead;
        
                // Stop at the last record
                while ( srchTimer->next )
                {
                    srchTimer = srchTimer->next;
                }
        
                // Add to the list
                srchTimer->next = newTimer;
            }
        }
    }

    HAL_TIMER_EXIT_CRITICAL_SECTION(0);

    return ( (newTimer != NULL) ? 1 : 0 );
}


void XxHalTimerPoll(uint32 sysTimeMillisecond)
{
    XxHalTimer_t *srchTimer;
    XxHalTimer_t *prevTimer;
    uint32 decTime = 0;
    
    if ( timerHead == NULL )
        return;
        
    // Add it to the end of the timer list
    srchTimer = timerHead;
    prevTimer = (void *)NULL;

    HAL_TIMER_ENTER_CRITICAL_SECTION(0);
    
    if (sysTimeMillisecond >= xx_hal_timer_time_backup)
    {
        decTime = sysTimeMillisecond - xx_hal_timer_time_backup;
    }
    else
    {
        //-- 
    }

    xx_hal_timer_time_backup = sysTimeMillisecond;
    HAL_TIMER_EXIT_CRITICAL_SECTION(0);
    
    // Look for open timer slot
    while ( srchTimer )
    {
        XxHalTimer_t *freeTimer = NULL;
    
        HAL_TIMER_ENTER_CRITICAL_SECTION(0);
    
        if (srchTimer->timeout > decTime)
        {
            srchTimer->timeout -= decTime;
        }
        else
        {
            srchTimer->timeout = 0;
        }
    
        // Check for reloading
        if ((srchTimer->reloadTimeout) && (srchTimer->event))
        {
            // Notify the function of a timeout
            srchTimer->f();
    
            // Reload the timer timeout value
            srchTimer->timeout = srchTimer->reloadTimeout;
        }
    
        // When timeout or delete (event_flag == 0)
        if (srchTimer->timeout == 0 || srchTimer->event == 0)
        {
            // Take out of list
            if ( prevTimer == NULL )
            {
                timerHead = srchTimer->next;
            }
            else
            {
                prevTimer->next = srchTimer->next;
            }
    
            // Setup to free memory
            freeTimer = srchTimer;
    
            // Next
            srchTimer = srchTimer->next;
        }
        else
        {
            // Get next
            prevTimer = srchTimer;
            srchTimer = srchTimer->next;
        }
    
        HAL_TIMER_EXIT_CRITICAL_SECTION(0);
    
        if ( freeTimer )
        {
            XxHalTimerEventCb_t f = 0;
            if (freeTimer->timeout == 0 && freeTimer->f)
            {
                f = freeTimer->f;
            }
            
            XxMemFree( freeTimer );

            if (f)
            {
                f();
            }
        }
    }
}

void XxHalTimerInit(void)
{
    xx_hal_timer_time_backup = 0;
}

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


