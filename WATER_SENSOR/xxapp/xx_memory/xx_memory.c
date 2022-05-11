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

#ifdef _XX_MEMORY_H
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
#include <stdlib.h>

/***************************************************************************************************
*                                               MACRO
*                                               宏定义
***************************************************************************************************/
#define XxMemPrintString                                    "MEM  :"
#define xxMemPrint                                          emberAfAppPrint
#define xxMemPrintBuffer                                    emberAfAppPrintBuffer

//#define xxMemPrint(pstring, ...)
//#define xxMemPrintBuffer(x,y,z)

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/
#ifndef XX_MAX_MEM_HEAP
#define XX_MAX_MEM_HEAP                                     (300)
#endif

#define XX_MEM_IN_USE                                       0x8000
#if (XX_MAX_MEM_HEAP & XX_MEM_IN_USE)
#error XX_MAX_MEM_HEAP is too big to manage!
#endif

#define XX_MEM_HDRSZ                                        sizeof(xxMemHdr_t)

// Round a value up to the ceiling of XX_MEM_HDRSZ for critical dependencies on even multiples.
//-- 以XX_MEM_HDRSZ大小为基本大小，对齐为以基本大小的块，例如基础块大小为4，XX_MEM_ROUND(6)即对齐为8
#define XX_MEM_ROUND(X)                                     ((((X) + XX_MEM_HDRSZ - 1) / XX_MEM_HDRSZ) * XX_MEM_HDRSZ)

/* Minimum wasted bytes to justify splitting a block before allocation.
 * Adjust accordingly to attempt to balance the tradeoff of wasted space and runtime throughput
 * spent splitting blocks into sizes that may not be practically usable when sandwiched between
 * two blocks in use (and thereby not able to be coalesced.)
 * Ensure that this size is an even multiple of XX_MEM_HDRSZ.
 */
#if !defined XX_MEM_MIN_BLKSZ
#define XX_MEM_MIN_BLKSZ                                    (XX_MEM_ROUND((XX_MEM_HDRSZ * 2)))
#endif

#if !defined XX_MEM_LL_BLKSZ
#define XX_MEM_LL_BLKSZ                                     0                   //-- (XX_MEM_ROUND(6) + (1 * XX_MEM_HDRSZ))
#endif

/* Adjust accordingly to attempt to accomodate the block sizes of the vast majority of
 * very high frequency allocations/frees by profiling the system runtime.
 * This default of 16 accomodates the OSAL timers block, osalTimerRec_t, and many others.
 * Ensure that this size is an even multiple of XX_MEM_MIN_BLKSZ for run-time efficiency.
 */
#if !defined XX_MEM_SMALL_BLKSZ
#define XX_MEM_SMALL_BLKSZ                                  (XX_MEM_ROUND(16))  //-- 16byte
#endif
#if !defined XX_MEM_SMALL_BLKCNT
#define XX_MEM_SMALL_BLKCNT                                 32                  //-- 16 * 32 = 512 bytes
#endif

/*
 * These numbers setup the size of the small-block bucket which is reserved at the front of the
 * heap for allocations of XX_MEM_SMALL_BLKSZ or smaller.
 */

// Size of the heap bucket reserved for small block-sized allocations.
// Adjust accordingly to attempt to accomodate the vast majority of very high frequency operations.
#define XX_MEM_SMALLBLK_BUCKET                              ((XX_MEM_SMALL_BLKSZ * XX_MEM_SMALL_BLKCNT) + XX_MEM_LL_BLKSZ)

// Index of the first available xxMemHdr_t after the small-block heap which will be set in-use in
// order to prevent the small-block bucket from being coalesced with the wilderness.
#define XX_MEM_SMALLBLK_HDRCNT                              (XX_MEM_SMALLBLK_BUCKET / XX_MEM_HDRSZ)

// Index of the first available xxMemHdr_t after the small-block heap which will be set in-use in
#define XX_MEM_BIGBLK_IDX                                   (XX_MEM_SMALLBLK_HDRCNT + 1)

// The size of the wilderness after losing the small-block heap, the wasted header to block the
// small-block heap from being coalesced, and the wasted header to mark the end of the heap.
#define XX_MEM_BIGBLK_SZ                                    (XX_MAX_MEM_HEAP - XX_MEM_SMALLBLK_BUCKET - XX_MEM_HDRSZ*2)

// Index of the last available xxMemHdr_t at the end of the heap which will be set to zero for
// fast comparisons with zero to determine the end of the heap.
#define XX_MEM_LASTBLK_IDX                                  ((XX_MAX_MEM_HEAP / XX_MEM_HDRSZ) - 1)

// For information about memory profiling, refer to SWRA204 "Heap Memory Management", section 1.5.
#if !defined XX_MEM_PROFILER
#define XX_MEM_PROFILER                                     FALSE  // Enable/disable the memory usage profiling buckets.
#endif
#if !defined XX_MEM_PROFILER_LL
#define XX_MEM_PROFILER_LL                                  FALSE  // Special profiling of the Long-Lived bucket.
#endif

#if XX_MEM_PROFILER
#define XX_MEM_INIT                                         'X'
#define XX_MEM_ALOC                                         'A'
#define XX_MEM_REIN                                         'F'
#endif

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef struct 
{
    // The 15 LSB's of 'val' indicate the total item size, including the header, in 8-bit bytes.
    unsigned len : 15;
    
    // The 1 MSB of 'val' is used as a boolean to indicate in-use or freed.
    unsigned inUse : 1;
} xxMemHdrHdr_t;

typedef union 
{
    /* Dummy variable so compiler forces structure to alignment of largest element while not wasting
     * space on targets when the halDataAlign_t is smaller than a UINT16.
     */
    halDataAlign_t alignDummy;
    uint16 val;
    xxMemHdrHdr_t hdr;
}xxMemHdr_t;

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
#if !defined ( ZBIT ) && defined ewarm
static __no_init xxMemHdr_t theHeap[XX_MAX_MEM_HEAP / XX_MEM_HDRSZ];
static __no_init xxMemHdr_t *ff1;                                               // First free block in the small-block bucket.
#else
static xxMemHdr_t theHeap[XX_MAX_MEM_HEAP / XX_MEM_HDRSZ];
static xxMemHdr_t *ff1;                                                         // First free block in the small-block bucket.
#endif

static uint8 osalMemStat;                                                       // Discrete status flags: 0x01 = kicked.

#if XX_MEM_METRICS
static uint16 blkMax;                                                           // Max cnt of all blocks ever seen at once.
static uint16 blkCnt;                                                           // Current cnt of all blocks.
static uint16 blkFree;                                                          // Current cnt of free blocks.
static uint16 memAlo;                                                           // Current total memory allocated.
static uint16 memMax;                                                           // Max total memory ever allocated at once.
#endif

#if XX_MEM_PROFILER
#define XX_MEM_PROMAX  8
/* The profiling buckets must differ by at least XX_MEM_MIN_BLKSZ; the
 * last bucket must equal the max alloc size. Set the bucket sizes to
 * whatever sizes necessary to show how your application is using memory.
 */
static uint16 proCnt[XX_MEM_PROMAX] = {
XX_MEM_SMALL_BLKSZ, 48, 112, 176, 192, 224, 256, 65535 };
static uint16 proCur[XX_MEM_PROMAX] = { 0 };
static uint16 proMax[XX_MEM_PROMAX] = { 0 };
static uint16 proTot[XX_MEM_PROMAX] = { 0 };
static uint16 proSmallBlkMiss;
#endif

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/
#ifdef DPRINTF_HEAPTRACE
extern int dprintf(const char *fmt, ...);
#endif /* DPRINTF_HEAPTRACE */


/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
void XxMainStart3(void)
{
    XxMemInit();
}

void XxMemAssertHandler(void)
{
    // execute code that handles asserts 
    // blink all leds
    while(TRUE)
    {
        //--halLedToggle(1);
        //--halLedToggle(2);
        //--halLedToggle(3);
        //--halLedToggle(4);
        //halMcuWaitMs(50);
    }
}

/**************************************************************************************************
 * @fn          xx_mem_init
 *
 * @brief       This function is the OSAL heap memory management initialization callback.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void XxMemInit(void)
{
    HAL_ASSERT(((XX_MEM_MIN_BLKSZ % XX_MEM_HDRSZ) == 0));
    HAL_ASSERT(((XX_MEM_LL_BLKSZ % XX_MEM_HDRSZ) == 0));
    HAL_ASSERT(((XX_MEM_SMALL_BLKSZ % XX_MEM_HDRSZ) == 0));

    #if XX_MEM_PROFILER
    (void)memset(theHeap, XX_MEM_INIT, XX_MAX_MEM_HEAP);
    #endif

    xxMemPrint("%p XxMemInit(), startAddr:0x%4x, smallBlock:%u, smallCnt:%u, bigBlokcIndex:%u, bigBlockSize:%u\r\n", XxMemPrintString, theHeap, XX_MEM_SMALLBLK_BUCKET, XX_MEM_SMALLBLK_HDRCNT, XX_MEM_BIGBLK_IDX, XX_MEM_BIGBLK_SZ);

    // Setup a NULL block at the end of the heap for fast comparisons with zero.
    //-- 最后一块直接设置为0，这样方便在申请的时候判断是否已经到堆的最末端了。
    theHeap[XX_MEM_LASTBLK_IDX].val = 0;

    // Setup the small-block bucket.
    ff1 = theHeap;
    ff1->val = XX_MEM_SMALLBLK_BUCKET;                   // Set 'len' & clear 'inUse' field.
    
    // Set 'len' & 'inUse' fields - this is a 'zero data bytes' lifetime allocation to block the
    // small-block bucket from ever being coalesced with the wilderness.
    //-- 将small-block后的第一个可用域设置为使用状态，防止在申请small-block时越界到big-block
    theHeap[XX_MEM_SMALLBLK_HDRCNT].val = (XX_MEM_HDRSZ | XX_MEM_IN_USE);

    // Setup the wilderness.
    theHeap[XX_MEM_BIGBLK_IDX].val = XX_MEM_BIGBLK_SZ;  // Set 'len' & clear 'inUse' field.

    /* Start with the small-block bucket and the wilderness - don't count the
     * end-of-heap NULL block nor the end-of-small-block NULL block.
     */
    #if ( XX_MEM_METRICS )
    blkCnt = blkFree = 2;
    #endif
}

/**************************************************************************************************
 * @fn          xx_mem_kick
 *
 * @brief       This function is the OSAL task initialization callback.
 * @brief       Kick the ff1 pointer out past the long-lived OSAL Task blocks.
 *              Invoke this once after all long-lived blocks have been allocated -
 *              presently at the end of osal_init_system().
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void XxMemKick(void)
{
    //-- 此函数主要是用于跳过长期占用的内存
    //-- 在此函数前先申请对应的内存，然后调用此函数，此函数中先申请最小的空间内存，
    //-- 把指针指向该地址，这样就跳过了之前已经申请的内存空间，之前申请的内存空间，是不会再释放的
    //-- 如果没有长期占用的内存，忽略此函数
    halIntState_t intState;
    xxMemHdr_t *tmp = XxMemAlloc(1);

    HAL_ASSERT((tmp != NULL));
    HAL_ENTER_CRITICAL_SECTION(intState);  // Hold off interrupts.

    /* All long-lived allocations have filled the LL block reserved in the small-block bucket.
     * Set 'osalMemStat' so searching for memory in this bucket from here onward will only be done
     * for sizes meeting the XX_MEM_SMALL_BLKSZ criteria.
     */
    ff1 = tmp - 1;       // Set 'ff1' to point to the first available memory after the LL block.
    XxMemFree(tmp);
    osalMemStat = 0x01;  // Set 'osalMemStat' after the free because it enables memory profiling.

    HAL_EXIT_CRITICAL_SECTION(intState);  // Re-enable interrupts.
    (void)intState;
}

uint8 XxMemAddrCheck(void *addr)
{
    xxMemHdr_t *hdr = (xxMemHdr_t *)addr - 1;
        
    if(((uint8 *)addr >= (uint8 *)theHeap) && ((uint8 *)addr < (uint8 *)theHeap+XX_MAX_MEM_HEAP) && hdr->hdr.inUse)
        return 1;
    else
        return 0;
}

/**************************************************************************************************
 * @fn          XxMemAlloc
 *
 * @brief       This function implements the OSAL dynamic memory allocation functionality.
 *
 * input parameters
 *
 * @param size - the number of bytes to allocate from the HEAP.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
#ifdef DPRINTF_OSALHEAPTRACE
void *XxMemAllocDbg( uint16 size, const char *fname, unsigned lnum )
#else /* DPRINTF_OSALHEAPTRACE */
void *XxMemAlloc( uint16 size )
#endif /* DPRINTF_OSALHEAPTRACE */
{
    xxMemHdr_t *prev = NULL;
    xxMemHdr_t *hdr;
    halIntState_t intState;
    uint8 coal = 0;
    uint16 remainSize = 0xffff;

    size += XX_MEM_HDRSZ;

    // Calculate required bytes to add to 'size' to align to halDataAlign_t.
    //-- 申请空间与最小单元对齐，不是最小单元的倍数则要增加到最小单元的倍数
    if ( sizeof( halDataAlign_t ) == 2 )
    {
        size += (size & 0x01);
    }
    else if ( sizeof( halDataAlign_t ) != 1 )
    {
        const uint8 mod = size % sizeof( halDataAlign_t );

        if ( mod != 0 )
        {
            size += (sizeof( halDataAlign_t ) - mod);
        }
    }

    /*xxMemPrint("%p XxMemAlloc(%u)\r\n", XxMemPrintString, size);

    if (size <= XX_MEM_SMALL_BLKSZ)
    {
        xxMemPrint("%p     Use small block\r\n", XxMemPrintString);
    }
    else
    {
        xxMemPrint("%p     Use big block\r\n", XxMemPrintString);
    }*/

    HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

    // Smaller allocations are first attempted in the small-block bucket, and all long-lived
    // allocations are channeled into the LL block reserved within this bucket.
    //if ((osalMemStat == 0) || (size <= XX_MEM_SMALL_BLKSZ))
    if (size <= XX_MEM_SMALL_BLKSZ)
    {
        hdr = ff1;
    }
    else
    {
        hdr = (theHeap + XX_MEM_BIGBLK_IDX);
    }

    do
    {
        XX_WATCHDOG_RESET;
        
        //-- coal主要用于标识连续的两个空的被申请过又释放的空间加起来是否满足被申请的空间大小
        if ( hdr->hdr.inUse )
        {
            coal = 0;
            //HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.
            //xxMemPrint("%p     0x%4x is in use, len:%u\r\n", XxMemPrintString, hdr, hdr->hdr.len);
            //HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.
        }
        else
        {
            if ( coal != 0 )
            {
                #if ( XX_MEM_METRICS )
                blkCnt--;
                blkFree--;
                #endif

                prev->hdr.len += hdr->hdr.len;

                if ( prev->hdr.len >= size )
                {
                    hdr = prev;
                    break;
                }
            }
            else
            {
                if ( hdr->hdr.len >= size )
                {
                    break;
                }

                coal = 1;
                prev = hdr;
            }
        }

        hdr = (xxMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);

        //-- 堆的最后一块被设置为0，这里判断是否到了堆的末端，如果到末端了说明没有更多空间被申请了，直接退出
        if ( hdr->val == 0 )
        {
            hdr = NULL;
            break;
        }
    } while (1);

    XX_WATCHDOG_RESET;

    if ( hdr != NULL )
    {
        remainSize = hdr->hdr.len - size;

        // Determine whether the threshold for splitting is met.
        //-- 如果剩下的大小比最小块要大的话，那么久分割开不同的块
        if ( remainSize >= XX_MEM_MIN_BLKSZ )
        {
            // Split the block before allocating it.
            xxMemHdr_t *next = (xxMemHdr_t *)((uint8 *)hdr + size);
            next->val = remainSize;                 // Set 'len' & clear 'inUse' field.
            hdr->val = (size | XX_MEM_IN_USE);      // Set 'len' & 'inUse' field.

            #if ( XX_MEM_METRICS )
            blkCnt++;
            if ( blkMax < blkCnt )
            {
                blkMax = blkCnt;
            }
            memAlo += size;
            #endif
        }
        else
        {
            #if ( XX_MEM_METRICS )
            memAlo += hdr->hdr.len;
            blkFree--;
            #endif

            hdr->hdr.inUse = TRUE;
        }

        
        #if ( XX_MEM_METRICS )
        if ( memMax < memAlo )
        {
            memMax = memAlo;
        }
        #endif

        #if ( XX_MEM_PROFILER )
        #if !XX_MEM_PROFILER_LL
        if (osalMemStat != 0)  // Don't profile until after the LL block is filled.
        #endif
        {
            uint8 idx;

            for ( idx = 0; idx < XX_MEM_PROMAX; idx++ )
            {
                if ( hdr->hdr.len <= proCnt[idx] )
                {
                    break;
                }
            }
            
            proCur[idx]++;
            
            if ( proMax[idx] < proCur[idx] )
            {
                proMax[idx] = proCur[idx];
            }
            
            proTot[idx]++;

            /* A small-block could not be allocated in the small-block bucket.
            * When this occurs significantly frequently, increase the size of the
            * bucket in order to restore better worst case run times. Set the first
            * profiling bucket size in proCnt[] to the small-block bucket size and
            * divide proSmallBlkMiss by the corresponding proTot[] size to get % miss.
            * Best worst case time on TrasmitApp was achieved at a 0-15% miss rate
            * during steady state Tx load, 0% during idle and steady state Rx load.
            */
            if ((hdr->hdr.len <= XX_MEM_SMALL_BLKSZ) && (hdr >= (theHeap + XX_MEM_BIGBLK_IDX)))
            {
                proSmallBlkMiss++;
            }
        }

        (void)memset((uint8 *)(hdr+1), XX_MEM_ALOC, (hdr->hdr.len - XX_MEM_HDRSZ));
        #endif

        if ((osalMemStat != 0) && (ff1 == hdr))
        {
            ff1 = (xxMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);
        }

        hdr++;
    }

    HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.
    (void)intState;

    HAL_ASSERT(((size_t)hdr % sizeof(halDataAlign_t)) == 0);

    #ifdef DPRINTF_OSALHEAPTRACE
    dprintf("XxMemAlloc(%u)->%lx:%s:%u\n", size, (unsigned) hdr, fname, lnum);
    #endif /* DPRINTF_OSALHEAPTRACE */

    if (hdr)
    {
        //xxMemPrint("%p     0x%4x is free, blkCnt:%u, remain:%u, memAlo:%u\r\n", XxMemPrintString, (unsigned) (hdr-1), blkCnt, remainSize, memAlo);
    }
    else
    {
        xxMemPrint("%p     fail, blkMax:%u, blkCnt:%u, blkFree:%u, memAlo:%u, memMax:%u, remain:%u\r\n", XxMemPrintString, blkMax, blkCnt, blkFree, memAlo, memMax, remainSize);
    }

    return (void *)hdr;
}

/**************************************************************************************************
 * @fn          XxMemFree
 *
 * @brief       This function implements the OSAL dynamic memory de-allocation functionality.
 *
 * input parameters
 *
 * @param ptr - A valid pointer (i.e. a pointer returned by XxMemAlloc()) to the memory to free.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
#ifdef DPRINTF_OSALHEAPTRACE
void XxMemFreeDbg(void *ptr, const char *fname, unsigned lnum)
#else /* DPRINTF_OSALHEAPTRACE */
void XxMemFree(void *ptr)
#endif /* DPRINTF_OSALHEAPTRACE */
{
    xxMemHdr_t *hdr;
    halIntState_t intState;

    hdr = (xxMemHdr_t *)ptr - 1;

    //-- xxMemPrint("%p XxMemFree(%u)\r\n", XxMemPrintString, hdr->hdr.len);

    #ifdef DPRINTF_OSALHEAPTRACE
    dprintf("XxMemFree(%lx):%s:%u\n", (unsigned) ptr, fname, lnum);
    #endif /* DPRINTF_OSALHEAPTRACE */

    HAL_ASSERT(((uint8 *)ptr >= (uint8 *)theHeap) && ((uint8 *)ptr < (uint8 *)theHeap+XX_MAX_MEM_HEAP));
    HAL_ASSERT(hdr->hdr.inUse);

    HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.
    hdr->hdr.inUse = FALSE;

    if (ff1 > hdr)
    {
        ff1 = hdr;
    }

    #if XX_MEM_PROFILER
    #if !XX_MEM_PROFILER_LL
    if (osalMemStat != 0)  // Don't profile until after the LL block is filled.
    #endif
    {
        uint8 idx;

        for (idx = 0; idx < XX_MEM_PROMAX; idx++)
        {
            XX_WATCHDOG_RESET;
            if (hdr->hdr.len <= proCnt[idx])
            {
                break;
            }
        }

        proCur[idx]--;
    }

    (void)memset((uint8 *)(hdr+1), XX_MEM_REIN, (hdr->hdr.len - XX_MEM_HDRSZ) );
    #endif
    
    #if XX_MEM_METRICS
    memAlo -= hdr->hdr.len;
    blkFree++;
    #endif

    HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.

    //-- xxMemPrint("%p     0x%4x, blkCnt:%d, memAlo:%u\r\n", XxMemPrintString, (unsigned) hdr, blkCnt, memAlo);

    (void)intState;
}

#if XX_MEM_METRICS
/*********************************************************************
 * @fn      XxHeapBlockMax
 *
 * @brief   Return the maximum number of blocks ever allocated at once.
 *
 * @param   none
 *
 * @return  Maximum number of blocks ever allocated at once.
 */
uint16 XxHeapBlockMax( void )
{
    return blkMax;
}

/*********************************************************************
 * @fn      XxHeapBlockCnt
 *
 * @brief   Return the current number of blocks now allocated.
 *
 * @param   none
 *
 * @return  Current number of blocks now allocated.
 */
uint16 XxHeapBlockCnt( void )
{
    return blkCnt;
}

/*********************************************************************
 * @fn      XxHeapBlockFree
 *
 * @brief   Return the current number of free blocks.
 *
 * @param   none
 *
 * @return  Current number of free blocks.
 */
uint16 XxHeapBlockFree( void )
{
    return blkFree;
}

/*********************************************************************
 * @fn      XxHeapMemUsed
 *
 * @brief   Return the current number of bytes allocated.
 *
 * @param   none
 *
 * @return  Current number of bytes allocated.
 */
uint16 XxHeapMemUsed( void )
{
    return memAlo;
}
#endif

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
/*********************************************************************
 * @fn      XxHeapHighWater
 *
 * @brief   Return the highest byte ever allocated in the heap.
 *
 * @param   none
 *
 * @return  Highest number of bytes ever used by the stack.
 */
uint16 XxHeapHighWater( void )
{
    #if ( XX_MEM_METRICS )
      return memMax;
    #else
      return XX_MAX_MEM_HEAP;
    #endif
}
#endif

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
    }
#endif
#endif

