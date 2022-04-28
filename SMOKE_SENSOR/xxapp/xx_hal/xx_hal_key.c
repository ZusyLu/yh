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

#ifdef _XX_HAL_KEY_H
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
//#define xxHalKeyPrint                                         emberAfAppPrint
//#define xxHalKeyPrintBuffer                                   emberAfAppPrintBuffer

#define xxHalKeyPrint(pstring, ...)
#define xxHalKeyPrintBuffer(x,y,z)

//#ifndef XX_HAL_KEY_1_PIN
//#define XX_HAL_KEY_1_PIN                                        PORTB_PIN(0)
//#define XX_HAL_KEY_1_INIT()                                     halGpioSetConfig(XX_HAL_KEY_1_PIN, _GPIO_P_MODEL_MODE0_INPUT)
//#define XX_HAL_KEY_1_READ()                                     halGpioRead(XX_HAL_KEY_1_PIN)
//#define XX_HAL_KEY_1_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
//#endif

//#ifndef XX_HAL_KEY_2_PIN
//#define XX_HAL_KEY_2_PIN                                      PORTB_PIN(1)
//#define XX_HAL_KEY_2_INIT()                                   halGpioSetConfig(XX_HAL_KEY_2_PIN, _GPIO_P_MODEL_MODE0_INPUT);
//#define XX_HAL_KEY_2_READ()                                   halGpioRead(XX_HAL_KEY_2_PIN)
//#endif

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
#ifndef XX_HAL_KEY_NUM
#define XX_HAL_KEY_NUM                                          0
#endif

#ifndef XX_HAL_KEY_1_ACTIVE 
#define XX_HAL_KEY_1_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

#ifndef XX_HAL_KEY_2_ACTIVE 
#define XX_HAL_KEY_2_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

#ifndef XX_HAL_KEY_3_ACTIVE 
#define XX_HAL_KEY_3_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

#ifndef XX_HAL_KEY_4_ACTIVE 
#define XX_HAL_KEY_4_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

#ifndef XX_HAL_KEY_5_ACTIVE 
#define XX_HAL_KEY_5_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

#ifndef XX_HAL_KEY_6_ACTIVE 
#define XX_HAL_KEY_6_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

#ifndef XX_HAL_KEY_7_ACTIVE 
#define XX_HAL_KEY_7_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

#ifndef XX_HAL_KEY_8_ACTIVE 
#define XX_HAL_KEY_8_ACTIVE                                     XX_HAL_KEY_ACTIVE_LOW
#endif

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/
XxHalKeyCBack_t pXxHalKeyProcessFunction;
uint8 registeredKeysTaskID = 0;

#ifdef XX_EXTERNAL_FUNC_KEY_EVENT_TIMEOUT
uint32 xx_hal_key_scan_timeout = XX_EXTERNAL_FUNC_KEY_EVENT_TIMEOUT;
#else
uint32 xx_hal_key_scan_timeout = 100;
#endif
uint8  xx_hal_key_init_flag = 0;
uint8  xx_hal_key_interrupt_flag = 0;
uint8  xx_hal_key_scan_flag;
uint8  xx_hal_key_saved_keys;                                   /* used to store previous key state in polling mode */

uint32 xx_hal_key_event_next_time_millisecond = 0xffffffff;
uint8  xx_hal_key_event_flag = 1;                               //-- 上电初始化完成，执行一次扫描

uint16 Xx_hal_key_press_time_count[8];
uint16 Xx_hal_key_release_time_count[8];


/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
uint32 xxHalKeyGetSystemClockMillisecond(void)
{
    return halCommonGetInt32uMillisecondTick();
}

void xxHalKeyEventNextTime(uint32 time)
{
    xx_hal_key_event_next_time_millisecond = xxHalKeyGetSystemClockMillisecond() + time;
    //-- osal_start_timerEx( Hal_TaskID, HAL_KEY_EVENT, time);
}

void xxHalKeyEventSet(void)
{
    xx_hal_key_event_flag = 1;
    //-- osal_stop_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT);
    //-- osal_set_event (Hal_TaskID, HAL_LED_BLINK_EVENT);
}

void xxHalKeyEventClear(void)
{
    xx_hal_key_event_next_time_millisecond = 0xffffffff;
    //-- osal_stop_timerEx(Hal_TaskID, HAL_KEY_EVENT);  /* Cancel polling if active */
    //-- osal_clear_event( Hal_TaskID, HAL_KEY_EVENT);
}

void XxHalKeySetScanTimeout(uint32 timeoutMsec)
{
    xx_hal_key_scan_timeout = timeoutMsec;
}

uint32 XxHalKeyGetScanTime(void)
{
    return xx_hal_key_scan_timeout;
}

void XxHalKeyRegister( XxHalKeyCBack_t cback )
{
    //-- Enable/Disable Interrupt
    //-- xx_hal_key_scan_flag = XX_HAL_KEY_MODE_INTERRUPT;

    //-- Register the callback fucntion
    pXxHalKeyProcessFunction = cback;
}

uint8 XxHalKeyRead(void)
{
    uint8 keys = 0;
  
    #if defined(XX_HAL_KEY_1_ACTIVE) && defined(XX_HAL_KEY_1_PIN)
    if(XX_HAL_KEY_1_ACTIVE(XX_HAL_KEY_1_READ()))
    {
        keys |= XX_HAL_KEY_1;
    }
    #endif

    #if defined(XX_HAL_KEY_2_ACTIVE) && defined(XX_HAL_KEY_2_PIN)
    if(XX_HAL_KEY_2_ACTIVE(XX_HAL_KEY_2_READ()))
    {
        keys |= XX_HAL_KEY_2;
    }
    #endif

    #if defined(XX_HAL_KEY_3_ACTIVE) && defined(XX_HAL_KEY_3_PIN)
    if(XX_HAL_KEY_3_ACTIVE(XX_HAL_KEY_3_READ()))
    {
        keys |= XX_HAL_KEY_3;
    }
    #endif

    #if defined(XX_HAL_KEY_4_ACTIVE) && defined(XX_HAL_KEY_4_PIN)
    if(XX_HAL_KEY_4_ACTIVE(XX_HAL_KEY_4_READ()))
    {
        keys |= XX_HAL_KEY_4;
    }
    #endif

    #if defined(XX_HAL_KEY_5_ACTIVE) && defined(XX_HAL_KEY_5_PIN)
    if(XX_HAL_KEY_5_ACTIVE(XX_HAL_KEY_5_READ()))
    {
        keys |= XX_HAL_KEY_5;
    }
    #endif

    #if defined(XX_HAL_KEY_6_ACTIVE) && defined(XX_HAL_KEY_6_PIN)
    if(XX_HAL_KEY_6_ACTIVE(XX_HAL_KEY_6_READ()))
    {
        keys |= XX_HAL_KEY_6;
    }
    #endif

    #if defined(XX_HAL_KEY_7_ACTIVE) && defined(XX_HAL_KEY_7_PIN)
    if(XX_HAL_KEY_7_ACTIVE(XX_HAL_KEY_7_READ()))
    {
        keys |= XX_HAL_KEY_7;
    }
    #endif

    #if defined(XX_HAL_KEY_8_ACTIVE) && defined(XX_HAL_KEY_8_PIN)
    if(XX_HAL_KEY_8_ACTIVE(XX_HAL_KEY_8_READ()))
    {
        keys |= XX_HAL_KEY_8;
    }
    #endif

    return keys;
}

//void halKeyProcessInterrupt(void)
//{
//#if (defined HAL_KEY) && (HAL_KEY == TRUE)
//    uint8 keys=0;

//#if defined(HAL_KEY1_BIT) && defined(HAL_KEY1_PXIFG)
//    if (HAL_KEY1_PXIFG & HAL_KEY1_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY1_PXIFG &= ~(HAL_KEY1_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_;//-- matthew
//    }
//#endif

//#if defined(HAL_KEY2_BIT) && defined(HAL_KEY2_PXIFG)
//    if (HAL_KEY2_PXIFG & HAL_KEY2_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY2_PXIFG &= ~(HAL_KEY2_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_2;//-- matthew
//    }
//#endif

//#if defined(HAL_KEY3_BIT) && defined(HAL_KEY3_PXIFG)
//    if (HAL_KEY3_PXIFG & HAL_KEY3_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY3_PXIFG &= ~(HAL_KEY3_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_3;//-- matthew
//    }
//#endif

//#if defined(HAL_KEY4_BIT) && defined(HAL_KEY4_PXIFG)
//    if (HAL_KEY4_PXIFG & HAL_KEY4_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY4_PXIFG &= ~(HAL_KEY4_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_4;//-- matthew
//    }
//#endif

//#if defined(HAL_KEY5_BIT) && defined(HAL_KEY5_PXIFG)
//    if (HAL_KEY5_PXIFG & HAL_KEY5_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY5_PXIFG &= ~(HAL_KEY5_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_5;//-- matthew
//    }
//#endif

//#if defined(HAL_KEY6_BIT) && defined(HAL_KEY6_PXIFG)
//    if (HAL_KEY6_PXIFG & HAL_KEY6_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY6_PXIFG &= ~(HAL_KEY6_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_6;//-- matthew
//    }
//#endif

//#if defined(HAL_KEY7_BIT) && defined(HAL_KEY7_PXIFG)
//    if (HAL_KEY7_PXIFG & HAL_KEY7_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY7_PXIFG &= ~(HAL_KEY7_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_7;//-- matthew
//    }
//#endif

//#if defined(HAL_KEY8_BIT) && defined(HAL_KEY8_PXIFG)
//    if (HAL_KEY8_PXIFG & HAL_KEY8_BIT)  /* Interrupt Flag has been set *///-- matthew
//    {
//    HAL_KEY8_PXIFG &= ~(HAL_KEY8_BIT); /* Clear Interrupt Flag *///-- matthew
//    keys |= XX_HAL_KEY_8;//-- matthew
//    }
//#endif

//#ifdef _IOT_H
//    if(iotExternalFuncKeyInterrupt(keys))
//    return;
//#endif

//    if (keys)
//    {
//    xx_hal_key_interrupt_flag=1;
//    osal_set_event(Hal_TaskID, HAL_KEY_EVENT);
//    }
//#endif
//}

//void HalKeyEnterSleep( void )
//{
//    HalKeyRead();
//}

//uint8 HalKeyExitSleep( void )
//{
//    /* Wake up and read keys */
//    uint8 key=0;
//  
//    key=HalKeyRead();
//    
//    #ifdef _IOT_ZSTACK_CALLBACKS_H
//    iotKeyExitSleep(key);
//    #endif

//    return ( key );
//}

/***************************************************************************************************
 *                                    INTERRUPT SERVICE ROUTINE
 ***************************************************************************************************/
//#if (defined HAL_KEY) && (HAL_KEY == TRUE)
/**************************************************************************************************
 * @fn      halKeyPort0Isr
 *
 * @brief   Port0 ISR
 *
 * @param
 *
 * @return
 **************************************************************************************************/
//HAL_ISR_FUNCTION( halKeyPort0Isr, P0INT_VECTOR )
//{
//  HAL_ENTER_ISR();

//  halKeyProcessInterrupt();

//  /*
//    Clear the CPU interrupt flag for Port_0
//    PxIFG has to be cleared before PxIF
//  */
//  HAL_KEY_CPU_PORT_0_IF = 0;
//  
//  CLEAR_SLEEP_MODE();
//  HAL_EXIT_ISR();
//}

/**************************************************************************************************
// * @fn      halKeyPort1Isr
// *
// * @brief   Port1 ISR
// *
// * @param
// *
// * @return
// **************************************************************************************************/
//HAL_ISR_FUNCTION( halKeyPort1Isr, P1INT_VECTOR )
//{
//  HAL_ENTER_ISR();
//  
//  halKeyProcessInterrupt();

//  /*
//    Clear the CPU interrupt flag for Port_2
//    PxIFG has to be cleared before PxIF
//  */
//  HAL_KEY_CPU_PORT_1_IF = 0;

//  CLEAR_SLEEP_MODE();
//  HAL_EXIT_ISR();
//}


/**************************************************************************************************
// * @fn      halKeyPort2Isr
// *
// * @brief   Port2 ISR
// *
// * @param
// *
// * @return
// **************************************************************************************************/
//HAL_ISR_FUNCTION( halKeyPort2Isr, P2INT_VECTOR )
//{
//  HAL_ENTER_ISR();
//  
//  halKeyProcessInterrupt();

//  /*
//    Clear the CPU interrupt flag for Port_2
//    PxIFG has to be cleared before PxIF
//    Notes: P2_1 and P2_2 are debug lines.
//  */
//  HAL_KEY_CPU_PORT_2_IF = 0;

//  CLEAR_SLEEP_MODE();
//  HAL_EXIT_ISR();
//}
//#endif




void XxHalKeyScan(void)
{
    uint8 keys = 0;
    uint8 prekeys=0;
    uint8 keychange_flag=0;

    #if defined(XX_HAL_KEY_1_ACTIVE) && defined(XX_HAL_KEY_1_PIN)
    if (XX_HAL_KEY_1_ACTIVE(XX_HAL_KEY_1_READ()))
    {
        keys |= XX_HAL_KEY_1;
    }
    #endif

    #if defined(XX_HAL_KEY_2_ACTIVE) && defined(XX_HAL_KEY_2_PIN)
    if(XX_HAL_KEY_2_ACTIVE(XX_HAL_KEY_2_READ()))
    {
        keys |= XX_HAL_KEY_2;
    }
    #endif

    #if defined(XX_HAL_KEY_3_ACTIVE) && defined(XX_HAL_KEY_3_PIN)
    if(XX_HAL_KEY_3_ACTIVE(XX_HAL_KEY_3_READ()))
    {
        keys |= XX_HAL_KEY_3;
    }
    #endif

    #if defined(XX_HAL_KEY_4_ACTIVE) && defined(XX_HAL_KEY_4_PIN)
    if(XX_HAL_KEY_4_ACTIVE(XX_HAL_KEY_4_READ()))
    {
        keys |= XX_HAL_KEY_4;
    }
    #endif

    #if defined(XX_HAL_KEY_5_ACTIVE) && defined(XX_HAL_KEY_5_PIN)
    if(XX_HAL_KEY_5_ACTIVE(XX_HAL_KEY_5_READ()))
    {
        keys |= XX_HAL_KEY_5;
    }
    #endif

    #if defined(XX_HAL_KEY_6_ACTIVE) && defined(XX_HAL_KEY_6_PIN)
    if(XX_HAL_KEY_6_ACTIVE(XX_HAL_KEY_6_READ()))
    {
        keys |= XX_HAL_KEY_6;
    }
    #endif

    #if defined(XX_HAL_KEY_7_ACTIVE) && defined(XX_HAL_KEY_7_PIN)
    if(XX_HAL_KEY_7_ACTIVE(XX_HAL_KEY_7_READ()))
    {
        keys |= XX_HAL_KEY_7;
    }
    #endif

    #if defined(XX_HAL_KEY_8_ACTIVE) && defined(XX_HAL_KEY_8_PIN)
    if(XX_HAL_KEY_8_ACTIVE(XX_HAL_KEY_8_READ()))
    {
        keys |= XX_HAL_KEY_8;
    }
    #endif

    /* If interrupts are not enabled, previous key status and current key status
    * are compared to find out if a key has changed status.
    */
    prekeys=xx_hal_key_saved_keys;
    if (keys != xx_hal_key_saved_keys)
    {
        /* Store the current keys for comparation next time */
        xx_hal_key_saved_keys = keys;
        keychange_flag=1;
    }

    /* Invoke Callback if new keys were depressed */
    if(pXxHalKeyProcessFunction)
    {
        xx_hal_key_init_flag=0;
        xx_hal_key_scan_flag = (pXxHalKeyProcessFunction) (keys, prekeys, xx_hal_key_scan_timeout);
    }
    else
    {
        xx_hal_key_scan_flag = 0;
    }
    
    xx_hal_key_interrupt_flag=0;
    //-- iotExternalFuncKeyPoll();

    /* if interrupt disabled, do next polling */
    if(xx_hal_key_scan_flag)
    {
        xxHalKeyEventNextTime(xx_hal_key_scan_timeout);
    }
    else
    {
        xxHalKeyEventClear();

        xx_hal_key_scan_flag=1;
        xx_hal_key_saved_keys=0;
    }

    (void)keychange_flag;
}


void XxHalKeyPoll(uint32_t sysTimeMillisecond)
{
    if (xx_hal_key_event_flag || 
        (sysTimeMillisecond >= xx_hal_key_event_next_time_millisecond))
    {
        xx_hal_key_event_flag = 0;                  //-- one time
        xx_hal_key_event_next_time_millisecond = 0xffffffff;
        
        XxHalKeyScan();
    }
}


void XxHalKeyInit(void)
{
    /* Initialize previous key to 0 */
    xx_hal_key_saved_keys = 0;

    /* Initialize callback function */
    pXxHalKeyProcessFunction  = NULL;

    /* Start with key is not configured */
    xx_hal_key_init_flag=1;

    #ifdef XX_HAL_KEY_1_PIN
    XX_HAL_KEY_1_INIT();
    
    #if defined(XX_HAL_KEY_1_IO_STATE) && (XX_HAL_KEY_1_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)
    HAL_KEY1_INP |= HAL_KEY1_BIT;
    #endif

    #if defined(XX_HAL_KEY_1_MODE) && (XX_HAL_KEY_1_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY1_ICTL |= HAL_KEY1_ICTLBIT;
    HAL_KEY1_IEN |= HAL_KEY1_IENBIT;
    HAL_KEY1_PXIFG &= ~(HAL_KEY1_BIT);
    #endif
    #endif



    #ifdef XX_HAL_KEY_2_PIN
    XX_HAL_KEY_2_INIT();

    #if defined(XX_HAL_KEY_2_IO_STATE) && (XX_HAL_KEY_2_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)
    HAL_KEY2_INP |= HAL_KEY2_BIT;
    #endif

    #if defined(XX_HAL_KEY_2_MODE) && (XX_HAL_KEY_2_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY2_ICTL |= HAL_KEY2_ICTLBIT;
    HAL_KEY2_IEN |= HAL_KEY2_IENBIT;
    HAL_KEY2_PXIFG &= ~(HAL_KEY2_BIT);
    #endif
    #endif



    #ifdef HAL_KEY3_BIT
    XX_HAL_KEY_3_INIT();

    #if(XX_HAL_KEY_3_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)
    HAL_KEY3_INP |= HAL_KEY3_BIT;
    #endif

    #if defined(HAL_KEY3_MODE) && (HAL_KEY3_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY3_ICTL |= HAL_KEY3_ICTLBIT;
    HAL_KEY3_IEN |= HAL_KEY3_IENBIT;
    HAL_KEY3_PXIFG &= ~(HAL_KEY3_BIT);
    #endif
    #endif



    #ifdef HAL_KEY4_BIT
    XX_HAL_KEY_4_INIT();

    #if(XX_HAL_KEY_4_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)
    HAL_KEY4_INP |= HAL_KEY4_BIT;
    #endif

    #if defined(HAL_KEY4_MODE) && (HAL_KEY4_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY4_ICTL |= HAL_KEY4_ICTLBIT;
    HAL_KEY4_IEN |= HAL_KEY4_IENBIT;
    HAL_KEY4_PXIFG &= ~(HAL_KEY4_BIT);
    #endif
    #endif


    #ifdef HAL_KEY5_BIT
    XX_HAL_KEY_5_INIT();

    #if(XX_HAL_KEY_5_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)
    HAL_KEY5_INP |= HAL_KEY5_BIT;
    #endif

    #if defined(HAL_KEY5_MODE) && (HAL_KEY5_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY5_ICTL |= HAL_KEY5_ICTLBIT;
    HAL_KEY5_IEN |= HAL_KEY5_IENBIT;
    HAL_KEY5_PXIFG &= ~(HAL_KEY5_BIT);
    #endif
    #endif


    #ifdef HAL_KEY6_BIT
    XX_HAL_KEY_6_INIT();

    #if(XX_HAL_KEY_6_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)//-- Ƚ̬ģʽ
    HAL_KEY6_INP |= HAL_KEY6_BIT;
    #endif

    #if defined(HAL_KEY6_MODE) && (HAL_KEY6_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY6_ICTL |= HAL_KEY6_ICTLBIT;
    HAL_KEY6_IEN |= HAL_KEY6_IENBIT;
    HAL_KEY6_PXIFG &= ~(HAL_KEY6_BIT);
    #endif
    #endif



    #ifdef HAL_KEY7_BIT
    XX_HAL_KEY_7_INIT();

    #if(XX_HAL_KEY_7_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)
    HAL_KEY7_INP |= HAL_KEY7_BIT;
    #endif

    #if defined(HAL_KEY7_MODE) && (HAL_KEY7_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY7_ICTL |= HAL_KEY7_ICTLBIT;
    HAL_KEY7_IEN |= HAL_KEY7_IENBIT;
    HAL_KEY7_PXIFG &= ~(HAL_KEY7_BIT);
    #endif
    #endif



    #ifdef HAL_KEY8_BIT
    XX_HAL_KEY_1_INIT();

    #if(XX_HAL_KEY_8_IO_STATE == XX_HAL_INPUT_IO_STATE_TRISTATE)
    HAL_KEY8_INP |= HAL_KEY8_BIT;
    #endif

    #if defined(HAL_KEY8_MODE) && (HAL_KEY8_MODE == XX_HAL_KEY_MODE_INTERRUPT)
    /* Interrupt configuration:
    * - Enable interrupt generation at the port
    * - Enable CPU interrupt
    * - Clear any pending interrupt
    */
    HAL_KEY8_ICTL |= HAL_KEY8_ICTLBIT;
    HAL_KEY8_IEN |= HAL_KEY8_IENBIT;
    HAL_KEY8_PXIFG &= ~(HAL_KEY8_BIT);
    #endif
    #endif




    //== pull up and down configuration ==//
#if defined(HAL_PORT_0_INPUT_IO_PUD)
#if (HAL_PORT_0_INPUT_IO_PUD == HAL_INPUT_IO_PULL_UP)
    P2INP &= ~0x20;
#else
    P2INP |= 0x20;
#endif
#endif

#if defined(HAL_PORT_1_INPUT_IO_PUD)
#if(HAL_PORT_1_INPUT_IO_PUD == HAL_INPUT_IO_PULL_UP)
    P2INP &= ~0x40;
#else
    P2INP |= 0x40;
#endif
#endif

#if defined(HAL_PORT_2_INPUT_IO_PUD)
#if(HAL_PORT_2_INPUT_IO_PUD == HAL_INPUT_IO_PULL_UP)
    P2INP &= ~0x80;
#else
    P2INP |= 0x80;
#endif
#endif

    //== interrupt edge configuration ==//
#if defined(HAL_PORT_0_INT_EDGE)
#if (HAL_PORT_0_INT_EDGE == HAL_INT_IO_EDGE_RISING)
    PICTL &= ~0x01;
#else
    PICTL |= 0x01;
#endif
#endif

#if defined(HAL_PORT_1_PIN_0_3_INT_EDGE)
#if (HAL_PORT_1_PIN_0_3_INT_EDGE == HAL_INT_IO_EDGE_RISING)
    PICTL &= ~0x02;
#else
    PICTL |= 0x02;
#endif
#endif

#if defined(HAL_PORT_1_PIN_4_7_INT_EDGE)
#if (HAL_PORT_1_PIN_4_7_INT_EDGE == HAL_INT_IO_EDGE_RISING)
    PICTL &= ~0x04;
#else
    PICTL |= 0x04;
#endif
#endif

#if defined(HAL_PORT_2_INT_EDGE)
#if (HAL_PORT_2_INT_EDGE == HAL_INT_IO_EDGE_RISING)
    PICTL &= ~0x08;
#else
    PICTL |= 0x08;
#endif
#endif
}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


