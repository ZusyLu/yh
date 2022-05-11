/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                                        .
***************************************************************************************************/

#ifndef _XX_MEMORY_H
#define _XX_MEMORY_H

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
*                                               宏定义
***************************************************************************************************/
#define XX_MAIN_START_3

#ifndef st
#define st( x )                                                 do { x } while (__LINE__ == -1)
#endif

#define HAL_ASSERT(expr)                                        st( if (!( expr )) XxMemAssertHandler(); )
#define HAL_ENTER_CRITICAL_SECTION(x)                           st( __disable_irq(); )
#define HAL_EXIT_CRITICAL_SECTION(x)                            st( __enable_irq(); )

typedef uint32                                                  halDataAlign_t;
typedef unsigned char                                           halIntState_t;

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
#if !defined ( XX_MEM_METRICS )
#define XX_MEM_METRICS                                          TRUE
#endif
    
/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void XxMainStart3(void);
extern void XxMemAssertHandler(void);
extern void XxMemInit( void );                                  //-- Initialize memory manager.
extern void XxMemKick( void );                                  //-- Setup efficient search for the first free block of heap.
extern uint8_t XxMemAddrCheck(void *addr);

#ifdef DPRINTF_OSALHEAPTRACE
extern void *XxMemAllocDbg( uint16_t size, const char *fname, unsigned lnum );
#define XxMemAlloc(_size ) XxMemAllocDbg(_size, __FILE__, __LINE__)
#else /* DPRINTF_OSALHEAPTRACE */
extern void *XxMemAlloc( uint16_t size );                       //-- Allocate a block of memory.
#endif /* DPRINTF_OSALHEAPTRACE */

#ifdef DPRINTF_OSALHEAPTRACE
extern void XxMemFreeDbg( void *ptr, const char *fname, unsigned lnum );
#define XxMemFree(_ptr ) XxMemFreeDbg(_ptr, __FILE__, __LINE__)
#else /* DPRINTF_OSALHEAPTRACE */
extern void XxMemFree( void *ptr );                             //-- Free a block of memory.
#endif /* DPRINTF_OSALHEAPTRACE */

#if ( XX_MEM_METRICS )
extern uint16 XxHeapBlockMax( void );                           //-- Return the maximum number of blocks ever allocated at once.
extern uint16 XxHeapBlockCnt( void );                           //-- Return the current number of blocks now allocated.
extern uint16 XxHeapBlockFree( void );                          //-- Return the current number of free blocks.
extern uint16 XxHeapMemUsed( void );                            //-- Return the current number of bytes allocated.
#endif

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
extern uint16 XxHeapHighWater( void );                          //-- Return the highest number of bytes ever used in the heap.
#endif

/***************************************************************************************************
*                                                END
***************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif



