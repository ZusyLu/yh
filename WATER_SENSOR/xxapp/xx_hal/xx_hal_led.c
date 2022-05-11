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

#ifdef _XX_HAL_LED_H
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
#define xxLedPrintln                                            emberAfAppPrintln
//#define xxLedPrintBuffer                                      emberAfAppPrintBuffer

#define xxLedPrint(pstring, ...)
#define xxLedPrintBuffer(x,y,z)

#ifndef XX_HAL_LED_NUM

#ifdef XX_LIGHT_THREE
#define XX_HAL_LED_NUM                                          0
#else
#define XX_HAL_LED_NUM                                          0
#endif

#endif

//#ifndef XX_HAL_LED_1_BIT
//#define XX_HAL_LED_1_BIT
//#define XX_HAL_LED_1_PIN                                        PORTD_PIN(0)
//#define XX_HAL_LED_1_INIT()                                     halGpioSetConfig(XX_HAL_LED_1_PIN, _GPIO_P_MODEL_MODE0_PUSHPULL);
//#define XX_HAL_LED_1_ON()                                       halGpioSet(XX_HAL_LED_1_PIN)
//#define XX_HAL_LED_1_OFF()                                      halGpioClear(XX_HAL_LED_1_PIN)
//#define XX_HAL_LED_1_STATE()                                    ((xx_hal_led_state&XX_HAL_LED_1) ? 1:0)
//#endif

//#ifndef XX_HAL_LED_2_BIT
//#define XX_HAL_LED_2_BIT
//#define XX_HAL_LED_2_PIN                                      PORTD_PIN(1)
//#define XX_HAL_LED_2_INIT()                                   halGpioSetConfig(XX_HAL_LED_2_PIN, _GPIO_P_MODEL_MODE0_PUSHPULL);
//#define XX_HAL_LED_2_ON()                                     halGpioSet(XX_HAL_LED_2_PIN)
//#define XX_HAL_LED_2_OFF()                                    halGpioClear(XX_HAL_LED_2_PIN)
//#define XX_HAL_LED_2_STATE()                                  ((xx_hal_led_state&XX_HAL_LED_2) ? 1:0)
//#endif

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/
static uint8 xx_hal_led_state;                                  // LED state at last set/clr/blink update

static uint8 xx_hal_led_sleep_state;                            // LED state at last set/clr/blink update
static uint8 xx_hal_led_pre_blink_state;                        // Original State before going to blink mode, bit 0, 1, 2, 3 represent led 0, 1, 2, 3

#ifdef XX_HAL_LED_BLINK
XxHalLedStatus_t XxHalLedStatusControl;
#endif


/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/
uint32 xx_hal_led_event_next_time_millisecond = 0xffffffff;
uint8  xx_hal_led_event_flag = 0;

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
#ifndef XX_EXTERNAL_FUNC_LED_BLINK_UPDATE
void XxExternalFuncLedBlinkUpdate(uint8 leds, uint8 numBlinks, uint8 mode, uint8 end)
{
    
}
#endif

#ifndef XX_EXTERNAL_FUNC_LED_BLINK_END
void XxExternalFuncLedBlinkEnd(uint8 leds, uint8 numBlinks, uint8 mode, uint8 end)
{
    
}
#endif

uint32 xxHalLedGetSystemClockMillisecond(void)
{
    return halCommonGetInt32uMillisecondTick();
}

void xxHalLedEventNextTime(uint32 time)
{
    xx_hal_led_event_next_time_millisecond = xxHalLedGetSystemClockMillisecond() + time;
    //osal_start_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT, time);          /* Schedule event */
}

void xxHalLedEventSet(void)
{
    xx_hal_led_event_flag = 1;
    //osal_stop_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT);
    //osal_set_event (Hal_TaskID, HAL_LED_BLINK_EVENT);
}

/***************************************************************************************************
 * @fn      XxHalLedSet
 *
 * @brief   Tun ON/OFF/TOGGLE given LEDs
 *
 * @param   led - bit mask value of leds to be turned ON/OFF/TOGGLE
 *          mode - BLINK, FLASH, TOGGLE, ON, OFF
 * @return  None
 ***************************************************************************************************/
uint8 XxHalLedSet (uint8 leds, uint8 mode)
{
    #if (defined (XX_HAL_LED_BLINK))
    uint8 led;
    XxHalLedControl_t *sts;
    #ifdef XX_HAL_LED_NUM
    uint8 ledNum = XX_HAL_LED_NUM;
    #else
    uint8 ledNum = 0;
    #endif
    uint8 ledMask = 0x01;
    for (uint8 i = 0;i < 8;i++,ledMask <<= 1)
    {
        if (i >= ledNum)
            leds &= ~ledMask;
    }

    switch (mode)
    {
        case XX_HAL_LED_MODE_BLINK:
            /* Default blink, 1 time, D% duty cycle */
            XxHalLedBlink (leds, 1, XX_HAL_LED_DEFAULT_DUTY_CYCLE, XX_HAL_LED_DEFAULT_FLASH_TIME, 0);
            break;

        case XX_HAL_LED_MODE_FLASH:
            /* Default flash, N times, D% duty cycle */
            XxHalLedBlink (leds, XX_HAL_LED_DEFAULT_FLASH_COUNT, XX_HAL_LED_DEFAULT_DUTY_CYCLE, XX_HAL_LED_DEFAULT_FLASH_TIME, 0);
            break;

        case XX_HAL_LED_MODE_ON:
        case XX_HAL_LED_MODE_OFF:
        case XX_HAL_LED_MODE_TOGGLE:
            led = XX_HAL_LED_1;
            leds &= XX_HAL_LED_ALL;
            sts = XxHalLedStatusControl.HalLedControlTable;

            while (leds)
            {
                if (leds & led)
                {
                    if (mode != XX_HAL_LED_MODE_TOGGLE)
                    {
                        sts->mode = mode;                                       /* ON or OFF */
                    }
                    else
                    {
                        sts->mode ^= XX_HAL_LED_MODE_ON;                        /* Toggle */
                    }
                    XxHalLedOnOff (led, sts->mode);
                    leds ^= led;
                }
                led <<= 1;
                sts++;
            }
            break;

        default:
            break;
    }

    #else
    
    XxHalLedOnOff(leds, mode);
    
    #endif

    return ( xx_hal_led_state );
}

/***************************************************************************************************
 * @fn      XxHalLedBlink
 *
 * @brief   Blink the leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *          numBlinks  - number of blinks
 *          percent    - the percentage in each period where the led
 *                       will be on
 *          period     - length of each cycle in milliseconds
 *
 * @return  None
 ***************************************************************************************************/
void XxHalLedBlink (uint8 leds, uint8 numBlinks, uint8 percent, uint16 period, uint8 donotChangeFirstState)
{
    #if defined (XX_HAL_LED_BLINK)
    uint8 led;
    XxHalLedControl_t *sts;

    if (leds && percent && period)
    {
        if (percent < 100)
        {
            led = XX_HAL_LED_1;
            leds &= XX_HAL_LED_ALL;
            sts = XxHalLedStatusControl.HalLedControlTable;

            while (leds)
            {
                if (leds & led)
                {
                    /* Store the current state of the led before going to blinking if not already blinking */
                    if(sts->mode < XX_HAL_LED_MODE_BLINK )
                        xx_hal_led_pre_blink_state |= (led & xx_hal_led_state);

                    if(!donotChangeFirstState)
                        sts->mode  = XX_HAL_LED_MODE_OFF;                       /* Stop previous blink */
                        
                    sts->time  = period;                                        /* Time for one on/off cycle */
                    sts->onPct = percent;                                       /* % of cycle LED is on */
                    sts->todo  = numBlinks;                                     /* Number of blink cycles */
                    if (!numBlinks) 
                        sts->mode |= XX_HAL_LED_MODE_FLASH;                     /* Continuous */
                    sts->next = xxHalLedGetSystemClockMillisecond();            /* Start now */
                    sts->mode |= XX_HAL_LED_MODE_BLINK;                         /* Enable blinking */
                    leds ^= led;
                }
                led <<= 1;
                sts++;
            }
            
            // Cancel any overlapping timer for blink events
            xxHalLedEventSet();
        }
        else
        {
            XxHalLedSet (leds, XX_HAL_LED_MODE_ON);                             /* >= 100%, turn on */
        }
    }
    else
    {
        XxHalLedSet (leds, XX_HAL_LED_MODE_OFF);                                /* No on time, turn off */
    }
    
    #else
    
    percent = (leds & xx_hal_led_state) ? XX_HAL_LED_MODE_OFF : XX_HAL_LED_MODE_ON;
    XxHalLedOnOff (leds, percent);                                              /* Toggle */
    
    #endif
}

/***************************************************************************************************
 * @fn      XxHalLedFindBlinkEvent
 *
 * @brief   
 *
 * @param   none
 *
 * @return  None
 ***************************************************************************************************/
uint8 XxHalLedBlinkFindEvent(void)
{
//    #if (defined (XX_HAL_LED_BLINK))
//    if(osalFindTimer(Hal_TaskID, HAL_LED_BLINK_EVENT) || osal_find_event(Hal_TaskID, HAL_LED_BLINK_EVENT))
//        return 1;
//    else
//        return 0;
//    #endif /* XX_HAL_LED_BLINK && HAL_LED */

    return 0;
}

/***************************************************************************************************
 * @fn      XxHalLedBlinkAddCount
 *
 * @brief   Add Conuts of Blink the leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *          numBlinks  - number of blinks
 *
 * @return  None
 ***************************************************************************************************/
void XxHalLedBlinkAddCount (uint8 leds, uint8 numBlinks)
{
    #if (defined (XX_HAL_LED_BLINK))
    uint8 led;
    XxHalLedControl_t *sts;

    if (!leds)
        return;

    #ifdef XX_HAL_LED_NUM
    uint8 ledNum = XX_HAL_LED_NUM;
    #else
    uint8 ledNum = 0;
    #endif
    uint8 ledMask=0x01;
    for (uint8 i = 0;i < 8;i++,ledMask <<= 1)
    {
        if (i >= ledNum)
            leds &= ~ledMask;
    }

    if (!leds)
        return;
    
    led = XX_HAL_LED_1;
    leds &= XX_HAL_LED_ALL;
    sts = XxHalLedStatusControl.HalLedControlTable;

    while (leds)
    {
        if (leds & led)
        {
            sts->todo  += numBlinks;                                            /* Number of blink cycles */
            leds ^= led;
        }
        led <<= 1;
        sts++;
    }
    #endif /* XX_HAL_LED_BLINK*/
}

/***************************************************************************************************
 * @fn      XxHalLedBlinkClear
 *
 * @brief   Clear the Blink leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *          numBlinks  - number of blinks
 *
 * @return  None
 ***************************************************************************************************/
void XxHalLedBlinkClear (uint8 leds)
{
    #if (defined (XX_HAL_LED_BLINK))
    uint8 led;
    XxHalLedControl_t *sts;

    if (!leds)
        return;
    
    #ifdef XX_HAL_LED_NUM
    uint8 ledNum = XX_HAL_LED_NUM;
    #else
    uint8 ledNum = 0;
    #endif
    uint8 ledMask = 0x01;
    for (uint8 i = 0;i < 8;i++,ledMask <<= 1)
    {
        if(i >= ledNum)
            leds &= ~ledMask;
    }

    if (!leds)
        return;
    
    led = XX_HAL_LED_1;
    leds &= XX_HAL_LED_ALL;
    sts = XxHalLedStatusControl.HalLedControlTable;

    while (leds)
    {
        if (leds & led)
        {
            sts->todo = 0;                                                      /* Number of blink cycles */
            sts->mode = XX_HAL_LED_MODE_OFF;
            leds ^= led;
        }
        
        led <<= 1;
        sts++;
    }
    #endif /* XX_HAL_LED_BLINK */
}

/***************************************************************************************************
 * @fn      XxHalLedBlinkGetCount
 *
 * @brief   Get Conuts of Blink leds
 *
 * @param   leds       - bit mask value of leds to be blinked
 *
 * @return  numBlinks  - number of blinks
 ***************************************************************************************************/
uint8 XxHalLedBlinkGetCount(uint8 leds)
{
    #if (defined (XX_HAL_LED_BLINK))
    uint8 led;
    XxHalLedControl_t *sts;

    if (!leds)
      return 0;
    
    led = XX_HAL_LED_1;
    leds &= XX_HAL_LED_ALL;
    sts = XxHalLedStatusControl.HalLedControlTable;

    while (leds)
    {
        if (leds & led)
        {
            return sts->todo;                                                   /* Number of blink cycles */
        }
        
        led <<= 1;
        sts++;
    }

    #endif /* XX_HAL_LED_BLINK*/

    return 0xff;
}

uint8 XxHalLedBlinkGetLeds(void)
{
    #if (defined (XX_HAL_LED_BLINK))
    uint8 blinkLeds = 0,i;
    XxHalLedControl_t *sts;
    
    sts = XxHalLedStatusControl.HalLedControlTable;

    for (i = 0;i < XX_HAL_LED_DEFAULT_MAX_LEDS;i++)
    {
        if(sts->mode & XX_HAL_LED_MODE_BLINK)
            blinkLeds |= BV(i);
        sts++;
    }

    #endif /* XX_HAL_LED_BLINK */

    return blinkLeds;
}


/***************************************************************************************************
 * @fn      HalLedUpdate
 *
 * @brief   Update leds to work with blink
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void xxHalLedUpdate (void)
{
    uint8 led;
    uint8 leds;
    uint32 time;
    uint16 next;
    uint16 wait;
    uint8 pct = 0;
    XxHalLedControl_t *sts;

    next = 0;
    led  = XX_HAL_LED_1;
    leds = XX_HAL_LED_ALL;
    sts = XxHalLedStatusControl.HalLedControlTable;

    /* Check if sleep is active or not */
    if (!XxHalLedStatusControl.sleepActive)
    {
        while (leds)
        {
            if (leds & led)
            {
                if (sts->mode & XX_HAL_LED_MODE_BLINK)
                {
                    time = xxHalLedGetSystemClockMillisecond();
                    
                    if (time >= sts->next)
                    {
                        if (sts->mode & XX_HAL_LED_MODE_ON)
                        {
                            pct = 100 - sts->onPct;                             /* Percentage of cycle for off */
                            sts->mode &= ~XX_HAL_LED_MODE_ON;                   /* Say it's not on */
                            XxHalLedOnOff (led, XX_HAL_LED_MODE_OFF);           /* Turn it off */
                            XxExternalFuncLedBlinkUpdate(led, sts->todo, XX_HAL_LED_MODE_OFF, 0);
                            if ( !(sts->mode & XX_HAL_LED_MODE_FLASH) )
                            {
                                if ( sts->todo != 0xff )
                                    sts->todo--;                                    /* Not continuous, reduce count */
                            }
                        }            
                        else if ( (!sts->todo) && !(sts->mode & XX_HAL_LED_MODE_FLASH) )
                        {
                            sts->mode ^= XX_HAL_LED_MODE_BLINK;                 // No more blinks
                        }
                        else
                        {
                            pct = sts->onPct;                                   // Percentage of cycle for on
                            sts->mode |= XX_HAL_LED_MODE_ON;                    // Say it's on
                            XxHalLedOnOff( led, XX_HAL_LED_MODE_ON );           // Turn it on
                            XxExternalFuncLedBlinkUpdate(led, sts->todo, XX_HAL_LED_MODE_ON, 0);
                        }
                        
                        if (sts->mode & XX_HAL_LED_MODE_BLINK)
                        {
                            wait = (((uint32)pct * (uint32)sts->time) / 100);
                            sts->next = time + wait;
                        }
                        else
                        {
                            /* no more blink, no more wait */
                            wait = 0;
                            
                            /* After blinking, set the LED back to the state before it blinks */
                            XxHalLedSet (led, ((xx_hal_led_pre_blink_state & led)!=0)?XX_HAL_LED_MODE_ON:XX_HAL_LED_MODE_OFF);
                            XxExternalFuncLedBlinkEnd(led, sts->todo, ((xx_hal_led_pre_blink_state & led)!=0)?XX_HAL_LED_MODE_ON:XX_HAL_LED_MODE_OFF, 1);
                            
                            /* Clear the saved bit */
                            xx_hal_led_pre_blink_state &= (led ^ 0xFF);
                        }
                    }
                    else
                    {
                        wait = sts->next - time;                                /* Time left */
                    }

                    if (!next || ( wait && (wait < next) ))
                    {
                        next = wait;
                    }
                }
                
                leds ^= led;
            }
            
            led <<= 1;
            sts++;
        }

        if (next)
        {
            xxHalLedEventNextTime(next);
        }
        else
        {
            xx_hal_led_event_flag = 0;
            xx_hal_led_event_next_time_millisecond = 0;
        }
    }
}

/***************************************************************************************************
 * @fn      HalGetLedState
 *
 * @brief   Dim LED2 - Dim (set level) of LED2
 *
 * @param   none
 *
 * @return  led state
 ***************************************************************************************************/
uint8 XxHalLedGetState (void)
{
    return xx_hal_led_state;
}

/***************************************************************************************************
 * @fn      XxHalLedEnterSleep
 *
 * @brief   Store current LEDs state before sleep
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void XxHalLedEnterSleep( void )
{
    #ifdef XX_HAL_LED_BLINK
    /* Sleep ON */
    XxHalLedStatusControl.sleepActive = TRUE;
    #endif /* XX_HAL_LED_BLINK */

    /* Save the state of each led */
    xx_hal_led_sleep_state = 0;
    #if defined(XX_HAL_LED_1_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_1_STATE();
    #endif
    
    #if defined(XX_HAL_LED_2_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_2_STATE() << 1;
    #endif
    
    #if defined(XX_HAL_LED_3_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_3_STATE() << 2;
    #endif
    
    #if defined(XX_HAL_LED_4_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_4_STATE() << 3;
    #endif
    
    #if defined(XX_HAL_LED_5_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_5_STATE() << 4;
    #endif
    
    #if defined(XX_HAL_LED_6_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_6_STATE() << 5;
    #endif
    
    #if defined(XX_HAL_LED_7_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_7_STATE() << 6;
    #endif
    
    #if defined(XX_HAL_LED_8_BIT)
    xx_hal_led_sleep_state |= XX_HAL_LED_8_STATE() << 7;
    #endif

    /* TURN OFF all LEDs to save power */
    XxHalLedOnOff (XX_HAL_LED_ALL, XX_HAL_LED_MODE_OFF);
}

/***************************************************************************************************
 * @fn      XxHalLedExitSleep
 *
 * @brief   Restore current LEDs state after sleep
 *
 * @param   none
 *
 * @return  none
 ***************************************************************************************************/
void XxHalLedExitSleep( void )
{
    /* Load back the saved state */
    XxHalLedOnOff(xx_hal_led_sleep_state, XX_HAL_LED_MODE_ON);

    /* Restart - This takes care BLINKING LEDS */
    xxHalLedUpdate();

    #ifdef XX_HAL_LED_BLINK
    /* Sleep OFF */
    XxHalLedStatusControl.sleepActive = FALSE;
    #endif /* XX_HAL_LED_BLINK */
}

/***************************************************************************************************
 * @fn      XxHalLedOnOff
 *
 * @brief   Turns specified LED ON or OFF
 *
 * @param   leds - LED bit mask
 *          mode - LED_ON,LED_OFF,
 *
 * @return  none
 ***************************************************************************************************/
void XxHalLedOnOff (uint8 leds, uint8 mode)
{
    #if defined(XX_HAL_LED_1_BIT) && (XX_HAL_LED_NUM >= 1)
    if (leds & XX_HAL_LED_1)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_1_ON();
        }
        else
        {
            XX_HAL_LED_1_OFF();
        }
    }
    #endif

    #if defined(XX_HAL_LED_2_BIT) && (XX_HAL_LED_NUM >= 2)
    if (leds & XX_HAL_LED_2)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_2_ON();
        }
        else
        {
            XX_HAL_LED_2_OFF();
        }
    }
    #endif

    #if defined(XX_HAL_LED_3_BIT) && (XX_HAL_LED_NUM >= 3)
    if (leds & XX_HAL_LED_3)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_3_ON();
        }
        else
        {
            XX_HAL_LED_3_OFF();
        }
    }
    #endif

    #if defined(XX_HAL_LED_4_BIT) && (XX_HAL_LED_NUM >= 4)
    if (leds & XX_HAL_LED_4)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_4_ON();
        }
        else
        {
            XX_HAL_LED_4_OFF();
        }
    }
    #endif

    #if defined(XX_HAL_LED_5_BIT) && (XX_HAL_LED_NUM >= 5)
    if (leds & XX_HAL_LED_5)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_5_ON();
        }
        else
        {
            XX_HAL_LED_5_OFF();
        }
    }
    #endif

    #if defined(XX_HAL_LED_6_BIT) && (XX_HAL_LED_NUM >= 6)
    if (leds & XX_HAL_LED_6)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_6_ON();
        }
        else
        {
            XX_HAL_LED_6_OFF();
        }
    }
    #endif

    #if defined(XX_HAL_LED_7_BIT) && (XX_HAL_LED_NUM >= 7)
    if (leds & XX_HAL_LED_7)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_7_ON();
        }
        else
        {
            XX_HAL_LED_7_OFF();
        }
    }
    #endif

    #if defined(XX_HAL_LED_8_BIT) && (XX_HAL_LED_NUM >= 8)
    if (leds & XX_HAL_LED_8)
    {
        if (mode == XX_HAL_LED_MODE_ON)
        {
            XX_HAL_LED_8_ON();
        }
        else
        {
            XX_HAL_LED_8_OFF();
        }
    }
    #endif

    /* Remember current state */
    if (mode)
    {
        xx_hal_led_state |= leds;
    }
    else
    {
        xx_hal_led_state &= (leds ^ 0xFF);
    }
}

void XxHalLedPoll(uint32_t sysTimeMillisecond)
{
    if (xx_hal_led_event_flag || 
        (sysTimeMillisecond >= xx_hal_led_event_next_time_millisecond))
    {
        xx_hal_led_event_flag = 0;                  //-- one time
        xx_hal_led_event_next_time_millisecond = 0xffffffff;
        
        xxHalLedUpdate();
    }
}

void XxHalLedInit(void)
{
    #if defined(XX_HAL_LED_1_BIT)
    XX_HAL_LED_1_INIT();
    #endif

    #if defined(XX_HAL_LED_2_BIT)
    XX_HAL_LED_2_INIT();
    #endif

    #if defined(XX_HAL_LED_3_BIT)
    XX_HAL_LED_3_INIT();
    #endif

    #if defined(XX_HAL_LED_4_BIT)
    XX_HAL_LED_4_INIT();
    #endif

    #if defined(XX_HAL_LED_5_BIT)
    XX_HAL_LED_5_INIT();
    #endif

    #if defined(XX_HAL_LED_6_BIT)
    XX_HAL_LED_6_INIT();
    #endif

    #if defined(XX_HAL_LED_7_BIT)
    XX_HAL_LED_7_INIT();
    #endif

    #if defined(XX_HAL_LED_8_BIT)
    XX_HAL_LED_8_INIT();
    #endif

    /* Initialize all LEDs to OFF */
    XxHalLedSet (XX_HAL_LED_ALL, XX_HAL_LED_MODE_OFF);

    #ifdef XX_HAL_LED_BLINK
    /* Initialize sleepActive to FALSE */
    XxHalLedStatusControl.sleepActive = FALSE;
    #endif
}

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


