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

#ifdef _XX_BLINK_LED_H
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
// Length restriction for LED pattern
#define XX_BLINK_LED_MAX_BLINK_PATTERN_LENGTH                           20

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef enum {
    LED_ON = 0x00,
    LED_OFF = 0x01,
    LED_BLINKING_ON = 0x02,
    LED_BLINKING_OFF = 0x03,
    LED_BLINK_PATTERN = 0x04,
} gpioBlinkState;

//init
typedef struct {
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} tLedArray;

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/
#ifdef XX_BLINK_LED_BSP_LED0
EmberEventControl xxBlnkLed0Event;
#endif

#ifdef XX_BLINK_LED_BSP_LED1
EmberEventControl xxBlnkLed1Event;
#endif

#ifdef XX_BLINK_LED_BSP_LED2
EmberEventControl xxBlnkLed2Event;
#endif

#ifdef XX_BLINK_LED_BSP_LED3
EmberEventControl xxBlnkLed3Event;
#endif

#ifdef XX_BLINK_LED_BSP_LED4
EmberEventControl xxBlnkLed4Event;
#endif


/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
EmberEventControl * xx_blink_ledEventArray[XX_BLINK_LED_HAL_LED_COUNT] = {
#if XX_BLINK_LED_HAL_LED_COUNT >= 1
    &xxBlnkLed0Event,
#endif
#if XX_BLINK_LED_HAL_LED_COUNT >= 2
    &xxBlnkLed1Event,
#endif
#if XX_BLINK_LED_HAL_LED_COUNT >= 3
    &xxBlnkLed2Event,
#endif
#if XX_BLINK_LED_HAL_LED_COUNT >= 4
    &xxBlnkLed3Event,
#endif
#if XX_BLINK_LED_HAL_LED_COUNT >= 5
    &xxBlnkLed4Event
#endif

};

static const tLedArray led_array[XX_BLINK_LED_BSP_LED_COUNT] = XX_BLINK_LED_BSP_LED_INIT;

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
static void xxBlinkTurnLedOn(uint8_t led);
static void xxBlinkTurnLedOff(uint8_t led);
static uint8_t xxBlinkLedLookup(uint8_t led);
static void xxBlinkHandleLedEvent(uint8_t ledIndex);

static gpioBlinkState xxBlinkLedEventState[XX_BLINK_LED_HAL_LED_COUNT] = { LED_OFF };
static uint8_t xxBlinkLedBlinkCount[XX_BLINK_LED_HAL_LED_COUNT] = { 0x00 };
static uint8_t xxBlinkActiveLed[XX_BLINK_LED_HAL_LED_COUNT] = XX_BLINK_LED_HAL_LED_ENABLE;

static uint16_t xxBlinkLedBlinkTimeMs[XX_BLINK_LED_BSP_LED_COUNT];
static uint16_t xxBlinkBlinkPattern[XX_BLINK_LED_BSP_LED_COUNT][XX_BLINK_LED_MAX_BLINK_PATTERN_LENGTH];
static uint8_t xxBlinkBlinkPatternLength[XX_BLINK_LED_BSP_LED_COUNT];
static uint8_t xxBlinkBlinkPatternIndex[XX_BLINK_LED_BSP_LED_COUNT];
static uint8_t xxBlinkLedSequence;

//init 
void XxBlinkLedInit( void )
{
    int i;
    uint8_t enableLeds[XX_BLINK_LED_BSP_LED_COUNT] = XX_BLINK_LED_HAL_LED_ENABLE;
    #if !defined(_SILICON_LABS_32B_SERIES_2)
         CMU_ClockEnable(cmuClock_HFPER, true);
    #endif //!defined(_SILICON_LABS_32B_SERIES_2)
        CMU_ClockEnable(cmuClock_GPIO, true);
    for ( i = 0; i < XX_BLINK_LED_BSP_LED_COUNT; i++ ) {
        GPIO_PinModeSet(led_array[enableLeds[i]].port,
                        led_array[enableLeds[i]].pin,
                        gpioModePushPull,
                        0);
    }
}

void XxBlinkLedSetLed( uint8_t led )
{
#if defined (XX_BLINK_LED_BSP_LED_POLARITY) && (XX_BLINK_LED_BSP_LED_POLARITY == 0)
  GPIO_PinOutClear(led_array[led].port, led_array[led].pin);
#else
  GPIO_PinOutSet(led_array[led].port, led_array[led].pin);
#endif
}

void XxBlinkLedClearLed( uint8_t led )
{
#if defined (XX_BLINK_LED_BSP_LED_POLARITY) && (XX_BLINK_LED_BSP_LED_POLARITY == 0)
  GPIO_PinOutSet(led_array[led].port, led_array[led].pin);
#else
  GPIO_PinOutClear(led_array[led].port, led_array[led].pin);
#endif
}

void XxBlinkLedToggleLed( uint8_t led )
{
  GPIO_PinOutToggle(led_array[led].port, led_array[led].pin);
}


static void xxBlinkHandleLedEvent(uint8_t ledIndex)
{
  // Verify that this event never tries to interact with an LED that has no
  // allocated array entries

  if (ledIndex < XX_BLINK_LED_HAL_LED_COUNT) {
    switch (xxBlinkLedEventState[ledIndex]) {
      case LED_ON:
        // was on.  this must be time to turn it off.
        xxBlinkTurnLedOff(xxBlinkActiveLed[ledIndex]);
        emberEventControlSetInactive(*(xx_blink_ledEventArray[ledIndex]));
        //XxIasMotionSensorScanNetworkBlinkLedRefreshFuction();// reflash led blink
        break;

      case LED_OFF:
        // was on.  this must be time to turn it off.
        xxBlinkTurnLedOn(xxBlinkActiveLed[ledIndex]);
        emberEventControlSetInactive(*(xx_blink_ledEventArray[ledIndex]));
        break;

      case LED_BLINKING_ON:
        xxBlinkTurnLedOff(xxBlinkActiveLed[ledIndex]);
        if (xxBlinkLedBlinkCount[ledIndex] > 0) {
            if (xxBlinkLedBlinkCount[ledIndex] != 255) { // blink forever if count is 255
                xxBlinkLedBlinkCount[ledIndex]--;
            }

            if (xxBlinkLedBlinkCount[ledIndex] > 0) {
                xxBlinkLedEventState[ledIndex] = LED_BLINKING_OFF;
                emberEventControlSetDelayMS(*(xx_blink_ledEventArray[ledIndex]),
                                            xxBlinkLedBlinkTimeMs[ledIndex]);
            } else {
                xxBlinkLedEventState[ledIndex] = LED_OFF;
                emberEventControlSetInactive(*(xx_blink_ledEventArray[ledIndex]));
            }
        } else {
            xxBlinkLedEventState[ledIndex] = LED_BLINKING_OFF;
            emberEventControlSetDelayMS(*(xx_blink_ledEventArray[ledIndex]),
                                      xxBlinkLedBlinkTimeMs[ledIndex]);
        }
        break;
        
      case LED_BLINKING_OFF:
        xxBlinkTurnLedOn(xxBlinkActiveLed[ledIndex]);
        xxBlinkLedEventState[ledIndex] = LED_BLINKING_ON;
        emberEventControlSetDelayMS(*(xx_blink_ledEventArray[ledIndex]),
                                    xxBlinkLedBlinkTimeMs[ledIndex]);
        break;
      case LED_BLINK_PATTERN:
        if (xxBlinkLedBlinkCount[ledIndex] == 0) {
          xxBlinkTurnLedOff(xxBlinkActiveLed[ledIndex]);

          xxBlinkLedEventState[ledIndex] = LED_OFF;
          emberEventControlSetInactive(*(xx_blink_ledEventArray[ledIndex]));
          break;
        }

        if (xxBlinkBlinkPatternIndex[ledIndex] % 2 == 1) {
          xxBlinkTurnLedOff(xxBlinkActiveLed[ledIndex]);
        } else {
          xxBlinkTurnLedOn(xxBlinkActiveLed[ledIndex]);
        }

        emberEventControlSetDelayMS(*(xx_blink_ledEventArray[ledIndex]),
                                    xxBlinkBlinkPattern[ledIndex][xxBlinkBlinkPatternIndex[
                                                             ledIndex]]);

        xxBlinkBlinkPatternIndex[ledIndex]++;

        if (xxBlinkBlinkPatternIndex[ledIndex] >= xxBlinkBlinkPatternLength[ledIndex]) {
          xxBlinkBlinkPatternIndex[ledIndex] = 0;
          if (xxBlinkLedBlinkCount[ledIndex] != 255) { // blink forever if count is 255
            xxBlinkLedBlinkCount[ledIndex]--;
          }
        }
        break;
      default:
        break;
    }
  } else {
    assert(false);
  }
}

void xxBlinkMultiLedBlinkLedOn(uint8_t timeMs, uint8_t led)
{
    uint8_t ledIndex;

    ledIndex = xxBlinkLedLookup(led);
    xxBlinkTurnLedOn(xxBlinkActiveLed[ledIndex]);
    xxBlinkLedEventState[ledIndex] = LED_ON;

    if (timeMs > 0) {
    emberEventControlSetDelayMS(*(xx_blink_ledEventArray[ledIndex]),
                                ((uint16_t) timeMs) );
    } else {
    emberEventControlSetInactive(*(xx_blink_ledEventArray[ledIndex]));
    }
}

void xxBlinkMultiLedBlinkLedOff(uint8_t timeMs, uint8_t led)
{
    uint8_t ledIndex;

    ledIndex = xxBlinkLedLookup(led);
    xxBlinkTurnLedOff(xxBlinkActiveLed[ledIndex]);
    xxBlinkLedEventState[ledIndex] = LED_OFF;

    if (timeMs > 0) {
    emberEventControlSetDelayQS(*(xx_blink_ledEventArray[ledIndex]),
                                ((uint16_t) timeMs) * 4);
    } else {
    emberEventControlSetInactive(*(xx_blink_ledEventArray[ledIndex]));
    }
}

void xxBlinkMultiLedBlinkBlink(uint8_t  count,
                           uint16_t blinkTimeMs,
                           uint8_t  led)
{
  uint8_t ledIndex;

    ledIndex = xxBlinkLedLookup(led);
    xxBlinkLedBlinkTimeMs[ledIndex] = blinkTimeMs;
    xxBlinkTurnLedOff(xxBlinkActiveLed[ledIndex]);
    xxBlinkLedEventState[ledIndex] = LED_BLINKING_OFF;
    emberEventControlSetDelayMS(*(xx_blink_ledEventArray[ledIndex]),
                              xxBlinkLedBlinkTimeMs[ledIndex]);
    xxBlinkLedBlinkCount[ledIndex] = count;
}

void xxBlinkMultiLedBlinkPattern(uint8_t  count,
                             uint8_t  length,
                             uint16_t *pattern,
                             uint8_t  led)
{
    uint8_t i, ledIndex;

    ledIndex = xxBlinkLedLookup(led);

    if (length < 2) {
    return;
    }

    xxBlinkTurnLedOn(xxBlinkActiveLed[ledIndex]);

    xxBlinkLedEventState[ledIndex] = LED_BLINK_PATTERN;

    if (length > XX_BLINK_LED_MAX_BLINK_PATTERN_LENGTH) {
    length = XX_BLINK_LED_MAX_BLINK_PATTERN_LENGTH;
    }

    xxBlinkBlinkPatternLength[ledIndex] = length;
    xxBlinkLedBlinkCount[ledIndex] = count;

    for (i = 0; i < xxBlinkBlinkPatternLength[ledIndex]; i++) {
    xxBlinkBlinkPattern[ledIndex][i] = pattern[i];
    }

    emberEventControlSetDelayMS(*(xx_blink_ledEventArray[ledIndex]),
                              xxBlinkBlinkPattern[ledIndex][0]);

    xxBlinkBlinkPatternIndex[ledIndex] = 1;
}

void xxBlinkMultiLedBlinkSetActivityLeds(uint8_t led)
{
    uint8_t i;

    for (i = 0; i < xxBlinkLedSequence; i++) {
        if (xxBlinkActiveLed[i] == led) {
            return;
        }
    }
    xxBlinkActiveLed[xxBlinkLedSequence] = (uint8_t)led;
    xxBlinkLedSequence++;

    if (xxBlinkLedSequence == XX_BLINK_LED_BSP_LED_COUNT) {
        xxBlinkLedSequence = 0;
    }
}

// *****************************************************************************
// function to set the GPIO and maintain the state during sleep.
// Port is 0 for port a, 1 for port b, and 2 for port c.
void xxBlinkLedBlinkSleepySetGpio(uint8_t port, uint8_t pin)
{
  GPIO_PinOutSet((GPIO_Port_TypeDef)port, pin);
}

// *****************************************************************************
// function to clear the GPIO and maintain the state during sleep.
// Port is 0 for port a, 1 for port b, and 2 for port c.
void xxBlinkLedBlinkSleepyClearGpio(uint8_t port, uint8_t pin)
{
  GPIO_PinOutClear((GPIO_Port_TypeDef)port, pin);
}

// *****************************************************************************
// Helper function to lookup which led to be acted on
static uint8_t xxBlinkLedLookup(uint8_t led)
{
  uint8_t i, ledIndex = 0;

  for (i = 0; i < XX_BLINK_LED_BSP_LED_COUNT; i++) {
    if (led == xxBlinkActiveLed[i]) {
      ledIndex = i;
      break;
    }
  }
  return ledIndex;
}

// *****************************************************************************
// Drive the LED for a GPIO high and update sleepy state
static void xxBlinkTurnLedOn(uint8_t led)
{
#ifdef LED_ACTIVE_HIGH
  XxBlinkLedSetLed( led );
#else
  XxBlinkLedClearLed( led );
#endif // LED_ACTIVE_HIGH
}

// *****************************************************************************
// Drive the LED for a GPIO low and update sleepy state
static void xxBlinkTurnLedOff(uint8_t led)
{
#ifdef LED_ACTIVE_HIGH
  XxBlinkLedClearLed( led );
#else
  XxBlinkLedSetLed( led );
#endif // LED_ACTIVE_HIGH
}


void xxBlinkLedBlinkLedOn(uint8_t timeMs)
{
    xxBlinkMultiLedBlinkLedOn(timeMs, xxBlinkActiveLed[0]);
}

void xxBlinkLedBlinkLedOff(uint8_t timeMs)
{
    xxBlinkMultiLedBlinkLedOff(timeMs, xxBlinkActiveLed[0]);
}

void xxBlinkLedBlinkBlink(uint8_t count, uint16_t blinkTimeMs)
{
    xxBlinkMultiLedBlinkBlink(count, blinkTimeMs, xxBlinkActiveLed[0]);
}

void xxBlinkLedBlinkPattern(uint8_t count, uint8_t length, uint16_t *pattern)
{
    xxBlinkMultiLedBlinkPattern(count, length, pattern, xxBlinkActiveLed[0]);
}

void xxBlinkLedBlinkSetActivityLed(uint8_t led)
{
    xxBlinkActiveLed[0] = (uint8_t)led;
}

#ifdef XX_BLINK_LED_BSP_LED0
void xxBlnkLed0EventHandle( void )
{
    xxBlinkHandleLedEvent(0);
}
#endif
    
#ifdef XX_BLINK_LED_BSP_LED1
void xxBlnkLed1EventHandle( void )
{
    xxBlinkHandleLedEvent(1);
}
#endif
    
#ifdef XX_BLINK_LED_BSP_LED2
void xxBlnkLed2EventHandle( void )
{
    xxBlinkHandleLedEvent(2);
}
#endif
    
#ifdef XX_BLINK_LED_BSP_LED3
void xxBlnkLed3EventHandle( void )
{
    xxBlinkHandleLedEvent(3);
}
#endif
    
#ifdef XX_BLINK_LED_BSP_LED4
void xxBlnkLed4EventHandle( void )
{
    xxBlinkHandleLedEvent(4);
}
#endif



/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


