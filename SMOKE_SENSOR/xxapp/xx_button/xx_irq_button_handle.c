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

#ifdef _XX_IRQ_BUTTON_HANDLE_H
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
#define xxIrqButtonHandlePrint                                   emberAfAppPrint
#define xxIrqButtonHandlePrintln                                 emberAfAppPrintln

//#define XX_IRQ_BUTTON_HANDLE_KEY_PRESSING_NOT_SLEEP_MODE 
//#define XX_IRQ_BUTTON_HANDLE_KEY_PRESSING_SLEEP_MODE

#define XX_IRQ_BUTTON_HANDLE_BUTTON_PRESSED                      1
#define XX_IRQ_BUTTON_HANDLE_BUTTON_RELEASED                     0

#define XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW                 0
#define XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_HIGH                1

#define XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS             5000
#define XX_IRQ_BUTTON_HANDLE_BUTTON_SHORT_TIME_OUT_MS            ( XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS/4 )

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
enum {
    BUTTON_STATE_LOW          = 0x00,
    BUTTON_STATE_HIGH         = 0x01,
    BUTTON_STATE_UNINIT       = 0xFF,
};

// Button debouncing and long/short differentiation state machine enums
enum {
    BUTTON_PRESSED_AT_STARTUP = 0x00,
    BUTTON_PRESSED_SHORT      = 0x01,
    BUTTON_PRESSED_LONG       = 0x02,
    BUTTON_IDLE               = 0x03,
};

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/
uint8_t xx_irq_button_handle_button = 0;
EmberEventControl xxTestEventControl;

#if ( XX_IRQ_BUTTON_COUNT > 0 )
EmberEventControl xxIrqButtonHandleButtonTimeOut;
#endif

#ifdef XX_IRQ_BSP_BUTTON0
EmberEventControl xxIrqButtonHandleButton0PressedEvent;
EmberEventControl xxIrqButtonHandleButton0ReleasedEvent;
#endif

#ifdef XX_IRQ_BSP_BUTTON1
EmberEventControl xxIrqButtonHandleButton1PressedEvent;
EmberEventControl xxIrqButtonHandleButton1ReleasedEvent;
#endif 

#ifdef XX_IRQ_BSP_BUTTON2
EmberEventControl xxIrqButtonHandleButton2PressedEvent;
EmberEventControl xxIrqButtonHandleButton2ReleasedEvent;
#endif

#ifdef XX_IRQ_BSP_BUTTON3
EmberEventControl xxIrqButtonHandleButton3PressedEvent;
EmberEventControl xxIrqButtonHandleButton3ReleasedEvent;
#endif 

#ifdef XX_IRQ_BSP_BUTTON4
EmberEventControl xxIrqButtonHandleButton4PressedEvent;
EmberEventControl xxIrqButtonHandleButton4ReleasedEvent;
#endif

#ifdef XX_IRQ_BSP_BUTTON5
EmberEventControl xxIrqButtonHandleButton5PressedEvent;
EmberEventControl xxIrqButtonHandleButton5ReleasedEvent;
#endif

uint8_t Xx_irq_button_handle_button0_counter_times = 0;
uint8_t Xx_irq_button_handle_button1_counter_times = 0;
uint8_t Xx_irq_button_handle_button2_counter_times = 0;
uint8_t Xx_irq_button_handle_button3_counter_times = 0;
uint8_t Xx_irq_button_handle_button4_counter_times = 0;
uint8_t Xx_irq_button_handle_button5_counter_times = 0;

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
#ifdef XX_IRQ_BSP_BUTTON0
static uint8_t xx_irq_button0_Polarity = XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW;
#endif

#ifdef XX_IRQ_BSP_BUTTON1
static uint8_t xx_irq_button1_Polarity = XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW;
#endif

#ifdef XX_IRQ_BSP_BUTTON2
static uint8_t xx_irq_button2_Polarity = XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW;
#endif

#ifdef XX_IRQ_BSP_BUTTON3
static uint8_t xx_irq_button3_Polarity = XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW;
#endif

#ifdef XX_IRQ_BSP_BUTTON4
static uint8_t xx_irq_button4_Polarity = XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW;
#endif

#ifdef XX_IRQ_BSP_BUTTON5
static uint8_t xx_irq_button5_Polarity = XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW;
#endif

static uint16_t xx_irq_button_handle_button0_timer = 0;
static uint16_t xx_irq_button_handle_button1_timer = 0;
static uint16_t xx_irq_button_handle_button2_timer = 0;
static uint16_t xx_irq_button_handle_button3_timer = 0;
static uint16_t xx_irq_button_handle_button4_timer = 0;
static uint16_t xx_irq_button_handle_button5_timer = 0;

static uint16_t xx_irq_button_handle_button0_counter = 0;
static uint16_t xx_irq_button_handle_button1_counter = 0;
static uint16_t xx_irq_button_handle_button2_counter = 0;
static uint16_t xx_irq_button_handle_button3_counter = 0;
static uint16_t xx_irq_button_handle_button4_counter = 0;
static uint16_t xx_irq_button_handle_button5_counter = 0;

static uint8_t xx_irq_button_handle_button0_last_state = BUTTON_STATE_UNINIT;
static uint8_t xx_irq_button_handle_button1_last_state = BUTTON_STATE_UNINIT;
static uint8_t xx_irq_button_handle_button2_last_state = BUTTON_STATE_UNINIT;
static uint8_t xx_irq_button_handle_button3_last_state = BUTTON_STATE_UNINIT;
static uint8_t xx_irq_button_handle_button4_last_state = BUTTON_STATE_UNINIT;
static uint8_t xx_irq_button_handle_button5_last_state = BUTTON_STATE_UNINIT;

static uint8_t xx_irq_button_handle_button0_pressed_state = BUTTON_PRESSED_AT_STARTUP;
static uint8_t xx_irq_button_handle_button1_pressed_state = BUTTON_PRESSED_AT_STARTUP;
static uint8_t xx_irq_button_handle_button2_pressed_state = BUTTON_PRESSED_AT_STARTUP;
static uint8_t xx_irq_button_handle_button3_pressed_state = BUTTON_PRESSED_AT_STARTUP;
static uint8_t xx_irq_button_handle_button4_pressed_state = BUTTON_PRESSED_AT_STARTUP;
static uint8_t xx_irq_button_handle_button5_pressed_state = BUTTON_PRESSED_AT_STARTUP;

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/


/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
static void xxIrqButtonHandleClearButtonCounters(void)
{
    xx_irq_button_handle_button0_counter = 0; 
    xx_irq_button_handle_button1_counter = 0;
    xx_irq_button_handle_button2_counter = 0; 
    xx_irq_button_handle_button3_counter = 0;
    xx_irq_button_handle_button4_counter = 0; 
    xx_irq_button_handle_button5_counter = 0; 
}

void XxIrqButtonHandleClearButtonCountersTimes( void )
{
    Xx_irq_button_handle_button0_counter_times = 0;
    Xx_irq_button_handle_button1_counter_times = 0;
    Xx_irq_button_handle_button2_counter_times = 0;
    Xx_irq_button_handle_button3_counter_times = 0;
    Xx_irq_button_handle_button4_counter_times = 0;
    Xx_irq_button_handle_button5_counter_times = 0;
}

uint8_t XxIrqButtonHandleReturnButtonPress( void )
{
    if ( Xx_irq_button_handle_button0_counter_times != 0 )
    {
        return 0;
    }
    else if( Xx_irq_button_handle_button1_counter_times != 0 )
    {
        return 1;
    }
    else if( Xx_irq_button_handle_button2_counter_times != 0 )
    {
        return 2;
    }
    else if( Xx_irq_button_handle_button3_counter_times != 0 )
    {
        return 3;
    }
    else if( Xx_irq_button_handle_button4_counter_times != 0 )
    {
        return 4;
    }
    
    return 0;
}

#if ( XX_IRQ_BUTTON_COUNT > 0 )
void xxIrqButtonHandleButtonTimeOutHandler( void )
{
    emberEventControlSetInactive( xxIrqButtonHandleButtonTimeOut );
    xxIrqButtonHandleClearButtonCounters();
}
#endif


// button 0
#ifdef XX_IRQ_BSP_BUTTON0  
void xxIrqButtonHandleButton0HighCallback( void )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    EmberStatus status;
	#ifdef XX_BLINK_LED_INIT
    	//XxBlinkLedInit();
	#endif
    xxBlinkMultiLedBlinkLedOn( XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED_TIME_MS, XX_BLINK_LED_BSP_LED0 );
    XxIasZoneStatusChangeNotificationClearFunction( XX_IAS_MOTION_SENSOR_ZONE_CHANGE_NOTIFICATION_BIT );
	//emberEventControlSetDelayMS(emberAfPluginBatteryMonitorReadADCEventControl,200);
}

void xxIrqButtonHandleButton0LowCallback( void )
{
    EmberStatus status;
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
	#ifdef XX_BLINK_LED_INIT
		//XxBlinkLedInit();
	#endif
    xxBlinkMultiLedBlinkLedOn( XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED_TIME_MS, XX_BLINK_LED_BSP_LED0 );
    xxIrqButtonHandlePrintln("will send ias ");
    Xx_project_press_falt = false;
    XxIasZoneStatusChangeNotificationTriggerFunction( XX_IAS_MOTION_SENSOR_ZONE_CHANGE_NOTIFICATION_BIT );
	//emberEventControlSetDelayMS(emberAfPluginBatteryMonitorReadADCEventControl,200);
}

void xxIrqButtonHandleButton0PressingCallback( void )
{
    emberAfCorePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void xxIrqButtonHandleButton0PressedLongCallback( uint16_t timePressedMs, bool pressedAtReset )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void xxIrqButtonHandleButton0PressedShortCallback( uint16_t timePressedMs )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}



void xxIrqButtonHandleButton0PressedEventHandler( void )
{
    emberEventControlSetInactive( xxIrqButtonHandleButtonTimeOut );
    if ( xx_irq_button_handle_button0_last_state != BUTTON_STATE_LOW )
    {
        xx_irq_button_handle_button0_last_state = BUTTON_STATE_LOW;
        xxIrqButtonHandleButton0LowCallback();
    }

    switch ( xx_irq_button_handle_button0_pressed_state )
    {
        case BUTTON_PRESSED_AT_STARTUP:
        case BUTTON_IDLE:
            {
                xx_irq_button_handle_button0_pressed_state = BUTTON_PRESSED_SHORT;
                emberEventControlSetDelayMS( xxIrqButtonHandleButton0PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS );
                xx_irq_button_handle_button0_timer = halCommonGetInt16uMillisecondTick();
            }
            break;
        case BUTTON_PRESSED_SHORT:
            {
                xx_irq_button_handle_button0_pressed_state = BUTTON_PRESSED_LONG;
            }
            //break;
        case BUTTON_PRESSED_LONG:
            {
                #ifdef XX_IRQ_BUTTON_HANDLE_KEY_PRESSING_NOT_SLEEP_MODE
                    emberEventControlSetDelayMS( xxIrqButtonHandleButton0PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_SHORT_TIME_OUT_MS );
                #else
                    emberEventControlSetInactive( xxIrqButtonHandleButton0PressedEvent );
                #endif
                
                xxIrqButtonHandleButton0PressingCallback();
            }
            break;
        default :
            {

            }
            break;
    }

}

void xxIrqButtonHandleButton0ReleasedEventHandler( void )
{
    uint16_t timePressed = halCommonGetInt16uMillisecondTick() - xx_irq_button_handle_button0_timer;
    emberEventControlSetInactive( xxIrqButtonHandleButton0ReleasedEvent );
    emberEventControlSetInactive( xxIrqButtonHandleButton0PressedEvent );
    
    if ( xx_irq_button_handle_button0_last_state != BUTTON_STATE_HIGH )
    {
        xx_irq_button_handle_button0_last_state = BUTTON_STATE_HIGH;
        xxIrqButtonHandleButton0HighCallback();
    }

    xx_irq_button_handle_button0_counter = timePressed;
    if ( timePressed >= XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS )
    {
        xxIrqButtonHandleButton0PressedLongCallback( timePressed, xx_irq_button_handle_button0_pressed_state == BUTTON_PRESSED_AT_STARTUP );
        xxIrqButtonHandleClearButtonCounters();
    }
    else
    {
        xxIrqButtonHandleButton0PressedShortCallback( xx_irq_button_handle_button0_counter );
        emberEventControlSetActive( xxIrqButtonHandleButtonTimeOut );
    }
    xx_irq_button_handle_button0_pressed_state = BUTTON_IDLE;
}

#endif

// button 1
#ifdef XX_IRQ_BSP_BUTTON1
void xxIrqButtonHandleButton1HighCallback( void )
{

}

void xxIrqButtonHandleButton1LowCallback( void )
{
}

void xxIrqButtonHandleButton1PressingCallback( void )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void xxIrqButtonHandleButton1PressedLongCallback( uint16_t timePressedMs, bool pressedAtReset )
{
	if ( Xx_key_for_leave_cunt == 2 ) //leave cunt is 3
	{
		xxIrqButtonHandlePrintln("xx button leave ");
		XxLeaveNetworkFuction();
	}
	else
	{
		xxIrqButtonHandlePrintln("xx button not leave ");
	}
	
	emberEventControlSetInactive(xx_project_button_interval_time_event);
	Xx_key_for_leave_cunt = 0;
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}


void xxIrqButtonHandleButton1PressedShortCallback( uint16_t timePressedMs )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    EmberNetworkStatus state;
	
	Xx_key_for_leave_cunt++;// cunt key press leave
	emberEventControlSetInactive(xx_project_button_interval_time_event);
	if ( Xx_key_for_leave_cunt != 2 )
	{
		emberEventControlSetDelayMS(xx_project_button_interval_time_event, (XX_PROJECT_BUTTON_INTERVAL_TIME/5) );
	}
	else
	{
		emberEventControlSetDelayMS(xx_project_button_interval_time_event, XX_PROJECT_BUTTON_INTERVAL_TIME);
	}

    state = emberAfNetworkState();
    if ( state == EMBER_NO_NETWORK )
    {
        Xx_project_scan_network_cunt = 0;
        //xxBlinkMultiLedBlinkLedOn( XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED_TIME_MS, XX_BLINK_LED_BSP_LED1 );
        emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_TIME_MS(100) );
    }
    else if ( state == EMBER_JOINED_NETWORK )
    {
        emberAfAddToCurrentAppTasks( EMBER_AF_FORCE_SHORT_POLL );
        emberEventControlSetDelayMS( xx_project_wait_cfg_cmd_event, XX_PROJECT_TIME_S(10) );
    }
    else if ( state == EMBER_JOINED_NETWORK_NO_PARENT )
    {   
        xxIrqButtonHandlePrintln(" xx will rejoin ");
        emberAfStartMoveCallback();
    }
}



void xxIrqButtonHandleButton1PressedEventHandler( void )
{
    emberEventControlSetInactive( xxIrqButtonHandleButtonTimeOut );
    if ( xx_irq_button_handle_button1_last_state != BUTTON_STATE_LOW )
    {
        xx_irq_button_handle_button1_last_state = BUTTON_STATE_LOW;
        xxIrqButtonHandleButton1LowCallback();
    }

    switch ( xx_irq_button_handle_button1_pressed_state )
    {
        case BUTTON_PRESSED_AT_STARTUP:
        case BUTTON_IDLE:
            {
                xx_irq_button_handle_button1_pressed_state = BUTTON_PRESSED_SHORT;
                emberEventControlSetDelayMS( xxIrqButtonHandleButton1PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS );
                xx_irq_button_handle_button1_timer = halCommonGetInt16uMillisecondTick();
            }
            break;
        case BUTTON_PRESSED_SHORT:
            {
                xx_irq_button_handle_button1_pressed_state = BUTTON_PRESSED_LONG;
            }
            //break;
        case BUTTON_PRESSED_LONG:
            {
                #ifdef XX_IRQ_BUTTON_HANDLE_KEY_PRESSING_NOT_SLEEP_MODE
                    emberEventControlSetDelayMS( xxIrqButtonHandleButton1PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_SHORT_TIME_OUT_MS );
                #else
                    emberEventControlSetInactive( xxIrqButtonHandleButton1PressedEvent );
                #endif
                xxIrqButtonHandleButton1PressingCallback();
            }
            break;
        default :
            {

            }
            break;
    }
}

void xxIrqButtonHandleButton1ReleasedEventHandler( void )
{
    uint16_t timePressed = halCommonGetInt16uMillisecondTick() - xx_irq_button_handle_button1_timer;
    emberEventControlSetInactive( xxIrqButtonHandleButton1ReleasedEvent );
    emberEventControlSetInactive( xxIrqButtonHandleButton1PressedEvent );
    
    if ( xx_irq_button_handle_button1_last_state != BUTTON_STATE_HIGH )
    {
        xx_irq_button_handle_button1_last_state = BUTTON_STATE_HIGH;
        xxIrqButtonHandleButton1HighCallback();
    }

    xx_irq_button_handle_button1_counter = timePressed;
    if ( timePressed >= XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS )
    {
        xxIrqButtonHandleButton1PressedLongCallback( timePressed, xx_irq_button_handle_button1_pressed_state == BUTTON_PRESSED_AT_STARTUP );
        xxIrqButtonHandleClearButtonCounters();
    }
    else
    {
        xxIrqButtonHandleButton1PressedShortCallback( xx_irq_button_handle_button1_counter );
        emberEventControlSetActive( xxIrqButtonHandleButtonTimeOut );
    }
    xx_irq_button_handle_button1_pressed_state = BUTTON_IDLE;
}

#endif

// button 2
#ifdef XX_IRQ_BSP_BUTTON2
void xxIrqButtonHandleButton2HighCallback( void )
{
    XxIasZoneStatusChangeNotificationTriggerFunction( XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT );
}

void xxIrqButtonHandleButton2LowCallback( void )
{
    XxIasZoneStatusChangeNotificationClearFunction( XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT );
}

void xxIrqButtonHandleButton2PressingCallback( void )
{
    emberAfCorePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void xxIrqButtonHandleButton2PressedLongCallback( uint16_t timePressedMs, bool pressedAtReset )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}


void xxIrqButtonHandleButton2PressedShortCallback( uint16_t timePressedMs )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    //Xx_project_scan_network_cunt = 0;
    //emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_TIME_MS );

}

void xxIrqButtonHandleButton2PressedEventHandler( void )
{
    emberEventControlSetInactive( xxIrqButtonHandleButtonTimeOut );
    if ( xx_irq_button_handle_button2_last_state != BUTTON_STATE_LOW )
    {
        xx_irq_button_handle_button2_last_state = BUTTON_STATE_LOW;
        xxIrqButtonHandleButton2LowCallback();
    }

    switch ( xx_irq_button_handle_button2_pressed_state )
    {
        case BUTTON_PRESSED_AT_STARTUP:
        case BUTTON_IDLE:
            {
                xx_irq_button_handle_button2_pressed_state = BUTTON_PRESSED_SHORT;
                emberEventControlSetDelayMS( xxIrqButtonHandleButton2PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS );
                xx_irq_button_handle_button2_timer = halCommonGetInt16uMillisecondTick();
            }
            break;
        case BUTTON_PRESSED_SHORT:
            {
                xx_irq_button_handle_button2_pressed_state = BUTTON_PRESSED_LONG;
            }
            //break;
        case BUTTON_PRESSED_LONG:
            {
                #ifdef XX_IRQ_BUTTON_HANDLE_KEY_PRESSING_NOT_SLEEP_MODE
                    emberEventControlSetDelayMS( xxIrqButtonHandleButton2PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_SHORT_TIME_OUT_MS );
                #else
                    emberEventControlSetInactive( xxIrqButtonHandleButton2PressedEvent );
                #endif

                xxIrqButtonHandleButton2PressingCallback();
            }
            break;
        default :
            {

            }
            break;
    }
}

void xxIrqButtonHandleButton2ReleasedEventHandler( void )
{
    uint16_t timePressed = halCommonGetInt16uMillisecondTick() - xx_irq_button_handle_button2_timer;
    emberEventControlSetInactive( xxIrqButtonHandleButton2ReleasedEvent );
    emberEventControlSetInactive( xxIrqButtonHandleButton2PressedEvent );
    
    if ( xx_irq_button_handle_button2_last_state != BUTTON_STATE_HIGH )
    {
        xx_irq_button_handle_button2_last_state = BUTTON_STATE_HIGH;
        xxIrqButtonHandleButton2HighCallback();
    }

    xx_irq_button_handle_button2_counter = timePressed;
    if ( timePressed >= XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS )
    {
        xxIrqButtonHandleButton2PressedLongCallback( timePressed, xx_irq_button_handle_button2_pressed_state == BUTTON_PRESSED_AT_STARTUP );
        xxIrqButtonHandleClearButtonCounters();
    }
    else
    {
        xxIrqButtonHandleButton2PressedShortCallback( xx_irq_button_handle_button2_counter );
        emberEventControlSetActive( xxIrqButtonHandleButtonTimeOut );
    }
    xx_irq_button_handle_button2_pressed_state = BUTTON_IDLE;
}

#endif

// button 3
#ifdef XX_IRQ_BSP_BUTTON3
void xxIrqButtonHandleButton3HighCallback( void )
{

}

void xxIrqButtonHandleButton3LowCallback( void )
{

}

void xxIrqButtonHandleButton3PressingCallback( void )
{

}

void xxIrqButtonHandleButton3PressedLongCallback( uint16_t timePressedMs, bool pressedAtReset )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    static uint32_t lastTime = 0 ;
    uint32_t time = 0;
    uint32_t intervalTime = 0;
    uint8_t buttonTemp = 0xff;
    EmberNetworkStatus state;
    tokType_xx_scene_t scene_struct;
    time = halCommonGetInt32uMillisecondTick();
    intervalTime = time - lastTime;
    lastTime = time;
    emberEventControlSetInactive(xx_project_button_interval_time_event);
    emberEventControlSetDelayMS(xx_project_button_interval_time_event, XX_PROJECT_BUTTON_INTERVAL_TIME);
    buttonTemp = Xx_irq_button_handle_button3_counter_times;
    XxIrqButtonHandleClearButtonCountersTimes();
    Xx_irq_button_handle_button3_counter_times = ++buttonTemp;

     if( intervalTime > XX_PROJECT_FACTORY_MIN_TIME \
         && intervalTime < XX_PROJECT_FACTORY_MAX_TIME \
         && Xx_project_will_leave_flat == true )
     {
         XxLeaveNetworkFuction();
         
         return;
         //halMultiLedBlinkBlink(4, XX_FAST_FLASHING_TIME, LED0);
     }
     xxIrqButtonHandlePrintln("Xx_irq_button_handle_button3_counter_times = %d ", Xx_irq_button_handle_button3_counter_times );
     state = emberAfNetworkState();
     xxIrqButtonHandlePrintln("ji xian button network status = %x ",  state);
     if ( state == EMBER_JOINED_NETWORK || state == EMBER_JOINED_NETWORK_NO_PARENT )
     {
         // report button number
         Xx_project_global_data[0] = XX_IRQ_BSP_BUTTON1;
         Xx_project_global_data[1] = 0x00;
         Xx_project_global_data[2] = 0x00;
         EmberStatus status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                            XX_PROJECT_REPORT_DEST_ENPOINT,\
                                                            XX_PROJECT_GW_SHROT_ADDRESS, \
                                                            XX_PROJECT_UNICASE_SEND_METHOD,
                                                            ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                            XX_ALPHA_CLUSTER_ID, \
                                                            XX_ALPHA_CLUSTER_REPORT_COMMAND_ID,\
                                                            Xx_project_global_data, \
                                                            XX_ALPHA_CLUSTER_REPORT_BUTTON_LEN);
         if (status != EMBER_SUCCESS) {
             xxIrqButtonHandlePrintln("ji xian send error : 0x%x",  status);
         }
#if 1
         halCommonGetIndexedToken(&scene_struct, TOKEN_XX_SCENE, XX_IRQ_BSP_BUTTON0);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.button         =%x ",scene_struct.triggerIndex);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.scenesid       =%x ",scene_struct.scenesid);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.groupid        =%2x ",scene_struct.groupid);

         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.triggerMode;   =%x ",scene_struct.triggerMode);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.destEnpiont;   =%x ",scene_struct.destEnpiont);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.zclCmd;        =%x ",scene_struct.zclCmd);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.clusterId;     =%2x ",scene_struct.clusterId);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.destShortId;   =%2x ",scene_struct.destShortId);

         switch( scene_struct.triggerMode )
         {
             case XX_ALPHA_CLUSTER_TRIGGER_SCENE_MODE:
                  {
                      // trigger app mesgsage
                      Xx_project_global_data[0] = scene_struct.groupid>>8;
                      Xx_project_global_data[1] = scene_struct.groupid;
                      Xx_project_global_data[2] = scene_struct.scenesid;
                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             XX_PROJECT_RECALL_SCENE_DEST_ENPOINT,\
                                                             XX_PROJECT_BROADCAST_ADDRESS, \
                                                             XX_NWK_BROADCAST_SEND_METHOD,
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             ZCL_SCENES_CLUSTER_ID, \
                                                             ZCL_RECALL_SCENE_COMMAND_ID,\
                                                             Xx_project_global_data, \
                                                             XX_ALPHA_CLUSTER_RECALL_SCENE_PAYLOAD_LEN);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_SCENE_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_UNICAST_MODE:
                  {
                      if( scene_struct.destShortId == 0xffff )
                      {
                          emberAfFindNodeId(&scene_struct.destMacId[0],emAfCliServiceDiscoveryCallback);
                          return;
                      }

                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             scene_struct.destEnpiont,\
                                                             scene_struct.destShortId, \
                                                             XX_NWK_UNICASE_SEND_METHOD,\
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             scene_struct.clusterId, \
                                                             scene_struct.zclCmd,\
                                                             NULL, \
                                                             0);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_UNICAST_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_GROUP_MODE:
                  {
                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             XX_PROJECT_GROUP_DEST_ENPOINT,\
                                                             scene_struct.groupid, \
                                                             XX_NWK_MULTICAST_SEND_METHOD,\
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             scene_struct.clusterId, \
                                                             scene_struct.zclCmd,\
                                                             NULL, \
                                                             0);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_UNICAST_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_NULL_MODE:
                  {
                      xxIrqButtonHandlePrintln("xx this is XX_TRIGGER_GROUP_MODE");
                      return;
                  }
                  break;

             default:
                    {
                        xxIrqButtonHandlePrintln("error and xx_scene_struct.triggerMode is :",scene_struct.triggerMode);
                    }
                    break;
         }
#endif
     }

    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void xxIrqButtonHandleButton3PressedShortCallback( uint16_t timePressedMs )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    //Xx_project_scan_network_cunt = 0;
    //emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_TIME_MS );
}

void xxIrqButtonHandleButton3PressedEventHandler( void )
{
    emberEventControlSetInactive( xxIrqButtonHandleButtonTimeOut );
    if ( xx_irq_button_handle_button3_last_state != BUTTON_STATE_LOW )
    {
        xx_irq_button_handle_button3_last_state = BUTTON_STATE_LOW;
        xxIrqButtonHandleButton3LowCallback();
    }

    switch ( xx_irq_button_handle_button3_pressed_state )
    {
        case BUTTON_PRESSED_AT_STARTUP:
        case BUTTON_IDLE:
            {
                xx_irq_button_handle_button3_pressed_state = BUTTON_PRESSED_SHORT;
                emberEventControlSetDelayMS( xxIrqButtonHandleButton3PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS );
                xx_irq_button_handle_button3_timer = halCommonGetInt16uMillisecondTick();
            }
            break;
        case BUTTON_PRESSED_SHORT:
            {
                xx_irq_button_handle_button3_pressed_state = BUTTON_PRESSED_LONG;
            }
            //break;
        case BUTTON_PRESSED_LONG:
            {
                #ifdef XX_IRQ_BUTTON_HANDLE_KEY_PRESSING_NOT_SLEEP_MODE
                    emberEventControlSetDelayMS( xxIrqButtonHandleButton3PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_SHORT_TIME_OUT_MS );
                #else
                    emberEventControlSetInactive( xxIrqButtonHandleButton3PressedEvent );
                #endif
                xxIrqButtonHandleButton3PressingCallback();
            }
            break;
        default :
            {

            }
            break;
    }
}

void xxIrqButtonHandleButton3ReleasedEventHandler( void )
{
    uint16_t timePressed = halCommonGetInt16uMillisecondTick() - xx_irq_button_handle_button3_timer;
    emberEventControlSetInactive( xxIrqButtonHandleButton3ReleasedEvent );
    emberEventControlSetInactive( xxIrqButtonHandleButton3PressedEvent );
    
    if ( xx_irq_button_handle_button3_last_state != BUTTON_STATE_HIGH )
    {
        xx_irq_button_handle_button3_last_state = BUTTON_STATE_HIGH;
        xxIrqButtonHandleButton3HighCallback();
    }

    xx_irq_button_handle_button3_counter = timePressed;
    if ( timePressed >= XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS )
    {
        xxIrqButtonHandleButton3PressedLongCallback( timePressed, xx_irq_button_handle_button3_pressed_state == BUTTON_PRESSED_AT_STARTUP );
        xxIrqButtonHandleClearButtonCounters();
    }
    else
    {
        xxIrqButtonHandleButton3PressedShortCallback( xx_irq_button_handle_button3_counter );
        emberEventControlSetActive( xxIrqButtonHandleButtonTimeOut );
    }
    xx_irq_button_handle_button3_pressed_state = BUTTON_IDLE;
}

#endif

// button 4
#ifdef XX_IRQ_BSP_BUTTON4
void xxIrqButtonHandleButton4HighCallback( void )
{

}

void xxIrqButtonHandleButton4LowCallback( void )
{

}

void xxIrqButtonHandleButton4PressingCallback( void )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void xxIrqButtonHandleButton4PressedLongCallback( uint16_t timePressedMs, bool pressedAtReset )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    static uint32_t lastTime = 0 ;
    uint32_t time = 0;
    uint32_t intervalTime = 0;
    uint8_t buttonTemp = 0xff;
    EmberNetworkStatus state;
    tokType_xx_scene_t scene_struct;
    time = halCommonGetInt32uMillisecondTick();
    intervalTime = time - lastTime;
    lastTime = time;
    emberEventControlSetInactive(xx_project_button_interval_time_event);
    emberEventControlSetDelayMS(xx_project_button_interval_time_event, XX_PROJECT_BUTTON_INTERVAL_TIME);
    buttonTemp = Xx_irq_button_handle_button4_counter_times;
    XxIrqButtonHandleClearButtonCountersTimes();
    Xx_irq_button_handle_button4_counter_times = ++buttonTemp;

     if( intervalTime > XX_PROJECT_FACTORY_MIN_TIME \
         && intervalTime < XX_PROJECT_FACTORY_MAX_TIME \
         && Xx_project_will_leave_flat == true )
     {
         XxLeaveNetworkFuction();
         
         return;
         //halMultiLedBlinkBlink(4, XX_FAST_FLASHING_TIME, LED0);
     }
     xxIrqButtonHandlePrintln("Xx_irq_button_handle_button4_counter_times = %d ", Xx_irq_button_handle_button4_counter_times );
     state = emberAfNetworkState();
     xxIrqButtonHandlePrintln("ji xian button network status = %x ",  state);
     if ( state == EMBER_JOINED_NETWORK || state == EMBER_JOINED_NETWORK_NO_PARENT )
     {
         // report button number
         Xx_project_global_data[0] = XX_IRQ_BSP_BUTTON1;
         Xx_project_global_data[1] = 0x00;
         Xx_project_global_data[2] = 0x00;
         EmberStatus status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                            XX_PROJECT_REPORT_DEST_ENPOINT,\
                                                            XX_PROJECT_GW_SHROT_ADDRESS, \
                                                            XX_PROJECT_UNICASE_SEND_METHOD,
                                                            ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                            XX_ALPHA_CLUSTER_ID, \
                                                            XX_ALPHA_CLUSTER_REPORT_COMMAND_ID,\
                                                            Xx_project_global_data, \
                                                            XX_ALPHA_CLUSTER_REPORT_BUTTON_LEN);
         if (status != EMBER_SUCCESS) {
             xxIrqButtonHandlePrintln("ji xian send error : 0x%x",  status);
         }
#if 1
         halCommonGetIndexedToken(&scene_struct, TOKEN_XX_SCENE, XX_IRQ_BSP_BUTTON0);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.button         =%x ",scene_struct.triggerIndex);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.scenesid       =%x ",scene_struct.scenesid);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.groupid        =%2x ",scene_struct.groupid);

         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.triggerMode;   =%x ",scene_struct.triggerMode);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.destEnpiont;   =%x ",scene_struct.destEnpiont);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.zclCmd;        =%x ",scene_struct.zclCmd);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.clusterId;     =%2x ",scene_struct.clusterId);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.destShortId;   =%2x ",scene_struct.destShortId);

         switch( scene_struct.triggerMode )
         {
             case XX_ALPHA_CLUSTER_TRIGGER_SCENE_MODE:
                  {
                      // trigger app mesgsage
                      Xx_project_global_data[0] = scene_struct.groupid>>8;
                      Xx_project_global_data[1] = scene_struct.groupid;
                      Xx_project_global_data[2] = scene_struct.scenesid;
                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             XX_PROJECT_RECALL_SCENE_DEST_ENPOINT,\
                                                             XX_PROJECT_BROADCAST_ADDRESS, \
                                                             XX_NWK_BROADCAST_SEND_METHOD,
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             ZCL_SCENES_CLUSTER_ID, \
                                                             ZCL_RECALL_SCENE_COMMAND_ID,\
                                                             Xx_project_global_data, \
                                                             XX_ALPHA_CLUSTER_RECALL_SCENE_PAYLOAD_LEN);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_SCENE_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_UNICAST_MODE:
                  {
                      if( scene_struct.destShortId == 0xffff )
                      {
                          emberAfFindNodeId(&scene_struct.destMacId[0],emAfCliServiceDiscoveryCallback);
                          return;
                      }

                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             scene_struct.destEnpiont,\
                                                             scene_struct.destShortId, \
                                                             XX_NWK_UNICASE_SEND_METHOD,\
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             scene_struct.clusterId, \
                                                             scene_struct.zclCmd,\
                                                             NULL, \
                                                             0);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_UNICAST_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_GROUP_MODE:
                  {
                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             XX_PROJECT_GROUP_DEST_ENPOINT,\
                                                             scene_struct.groupid, \
                                                             XX_NWK_MULTICAST_SEND_METHOD,\
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             scene_struct.clusterId, \
                                                             scene_struct.zclCmd,\
                                                             NULL, \
                                                             0);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_UNICAST_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_NULL_MODE:
                  {
                      xxIrqButtonHandlePrintln("xx this is XX_TRIGGER_GROUP_MODE");
                      return;
                  }
                  break;

             default:
                    {
                        xxIrqButtonHandlePrintln("error and xx_scene_struct.triggerMode is :",scene_struct.triggerMode);
                    }
                    break;
         }
#endif
     }

    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void xxIrqButtonHandleButton4PressedShortCallback( uint16_t timePressedMs )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    //Xx_project_scan_network_cunt = 0;
    //emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_TIME_MS );
}


void xxIrqButtonHandleButton4PressedEventHandler( void )
{
    emberEventControlSetInactive( xxIrqButtonHandleButtonTimeOut );
    if ( xx_irq_button_handle_button4_last_state != BUTTON_STATE_LOW )
    {
        xx_irq_button_handle_button4_last_state = BUTTON_STATE_LOW;
        xxIrqButtonHandleButton4LowCallback();
    }

    switch ( xx_irq_button_handle_button4_pressed_state )
    {
        case BUTTON_PRESSED_AT_STARTUP:
        case BUTTON_IDLE:
            {
                xx_irq_button_handle_button4_pressed_state = BUTTON_PRESSED_SHORT;
                emberEventControlSetDelayMS( xxIrqButtonHandleButton4PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS );
                xx_irq_button_handle_button4_timer = halCommonGetInt16uMillisecondTick();
            }
            break;
        case BUTTON_PRESSED_SHORT:
            {
                xx_irq_button_handle_button4_pressed_state = BUTTON_PRESSED_LONG;
            }
            //break;
        case BUTTON_PRESSED_LONG:
            {
                emberEventControlSetInactive( xxIrqButtonHandleButton4PressedEvent );
                xxIrqButtonHandleButton4PressingCallback();
            }
            break;
        default :
            {

            }
            break;
    }
}

void xxIrqButtonHandleButton4ReleasedEventHandler( void )
{
    uint16_t timePressed = halCommonGetInt16uMillisecondTick() - xx_irq_button_handle_button4_timer;
    emberEventControlSetInactive( xxIrqButtonHandleButton4ReleasedEvent );
    emberEventControlSetInactive( xxIrqButtonHandleButton4PressedEvent );
    
    if ( xx_irq_button_handle_button4_last_state != BUTTON_STATE_HIGH )
    {
        xx_irq_button_handle_button4_last_state = BUTTON_STATE_HIGH;
        xxIrqButtonHandleButton4HighCallback();
    }

    xx_irq_button_handle_button4_counter = timePressed;
    if ( timePressed >= XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS )
    {
        xxIrqButtonHandleButton4PressedLongCallback( timePressed, xx_irq_button_handle_button4_pressed_state == BUTTON_PRESSED_AT_STARTUP );
        xxIrqButtonHandleClearButtonCounters();
    }
    else
    {
        xxIrqButtonHandleButton4PressedShortCallback( xx_irq_button_handle_button4_counter );
        emberEventControlSetActive( xxIrqButtonHandleButtonTimeOut );
    }
    xx_irq_button_handle_button4_pressed_state = BUTTON_IDLE;
}

#endif

//button 5
#ifdef XX_IRQ_BSP_BUTTON5
void xxIrqButtonHandleButton5HighCallback( void )
{

}

void xxIrqButtonHandleButton5LowCallback( void )
{

}

void xxIrqButtonHandleButton5PressingCallback( void )
{

}

void xxIrqButtonHandleButton5PressedLongCallback( uint16_t timePressedMs, bool pressedAtReset )
{
    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    static uint32_t lastTime = 0 ;
    uint32_t time = 0;
    uint32_t intervalTime = 0;
    uint8_t buttonTemp = 0xff;
    EmberNetworkStatus state;
    tokType_xx_scene_t scene_struct;
    time = halCommonGetInt32uMillisecondTick();
    intervalTime = time - lastTime;
    lastTime = time;
    emberEventControlSetInactive(xx_project_button_interval_time_event);
    emberEventControlSetDelayMS(xx_project_button_interval_time_event, XX_PROJECT_BUTTON_INTERVAL_TIME);
    buttonTemp = Xx_irq_button_handle_button5_counter_times;
    XxIrqButtonHandleClearButtonCountersTimes();
    Xx_irq_button_handle_button5_counter_times = ++buttonTemp;

     if( intervalTime > XX_PROJECT_FACTORY_MIN_TIME \
         && intervalTime < XX_PROJECT_FACTORY_MAX_TIME \
         && Xx_project_will_leave_flat == true )
     {
         XxLeaveNetworkFuction();
         
         return;
         //halMultiLedBlinkBlink(4, XX_FAST_FLASHING_TIME, LED0);
     }
     xxIrqButtonHandlePrintln("Xx_irq_button_handle_button5_counter_times = %d ", Xx_irq_button_handle_button5_counter_times );
     state = emberAfNetworkState();
     xxIrqButtonHandlePrintln("ji xian button network status = %x ",  state);
     if ( state == EMBER_JOINED_NETWORK || state == EMBER_JOINED_NETWORK_NO_PARENT )
     {
         // report button number
         Xx_project_global_data[0] = XX_IRQ_BSP_BUTTON5;
         Xx_project_global_data[1] = 0x00;
         Xx_project_global_data[2] = 0x00;
         EmberStatus status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                            XX_PROJECT_REPORT_DEST_ENPOINT,\
                                                            XX_PROJECT_GW_SHROT_ADDRESS, \
                                                            XX_PROJECT_UNICASE_SEND_METHOD,
                                                            ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                            XX_ALPHA_CLUSTER_ID, \
                                                            XX_ALPHA_CLUSTER_REPORT_COMMAND_ID,\
                                                            Xx_project_global_data, \
                                                            XX_ALPHA_CLUSTER_REPORT_BUTTON_LEN);
         if (status != EMBER_SUCCESS) {
             xxIrqButtonHandlePrintln("ji xian send error : 0x%x",  status);
         }
#if 1
         halCommonGetIndexedToken(&scene_struct, TOKEN_XX_SCENE, XX_IRQ_BSP_BUTTON0);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.button         =%x ",scene_struct.triggerIndex);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.scenesid       =%x ",scene_struct.scenesid);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.groupid        =%2x ",scene_struct.groupid);

         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.triggerMode;   =%x ",scene_struct.triggerMode);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.destEnpiont;   =%x ",scene_struct.destEnpiont);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.zclCmd;        =%x ",scene_struct.zclCmd);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.clusterId;     =%2x ",scene_struct.clusterId);
         xxIrqButtonHandlePrintln("TOKEN_XX_SCENE.destShortId;   =%2x ",scene_struct.destShortId);

         switch( scene_struct.triggerMode )
         {
             case XX_ALPHA_CLUSTER_TRIGGER_SCENE_MODE:
                  {
                      // trigger app mesgsage
                      Xx_project_global_data[0] = scene_struct.groupid>>8;
                      Xx_project_global_data[1] = scene_struct.groupid;
                      Xx_project_global_data[2] = scene_struct.scenesid;
                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             XX_PROJECT_RECALL_SCENE_DEST_ENPOINT,\
                                                             XX_PROJECT_BROADCAST_ADDRESS, \
                                                             XX_NWK_BROADCAST_SEND_METHOD,
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             ZCL_SCENES_CLUSTER_ID, \
                                                             ZCL_RECALL_SCENE_COMMAND_ID,\
                                                             Xx_project_global_data, \
                                                             XX_ALPHA_CLUSTER_RECALL_SCENE_PAYLOAD_LEN);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_SCENE_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_UNICAST_MODE:
                  {
                      if( scene_struct.destShortId == 0xffff )
                      {
                          emberAfFindNodeId(&scene_struct.destMacId[0],emAfCliServiceDiscoveryCallback);
                          return;
                      }

                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             scene_struct.destEnpiont,\
                                                             scene_struct.destShortId, \
                                                             XX_NWK_UNICASE_SEND_METHOD,\
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             scene_struct.clusterId, \
                                                             scene_struct.zclCmd,\
                                                             NULL, \
                                                             0);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_UNICAST_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_GROUP_MODE:
                  {
                      status = XxNwkMultipleModeSendRfPacket(XX_PROJECT_SOURCE_ENPOINT, \
                                                             XX_PROJECT_GROUP_DEST_ENPOINT,\
                                                             scene_struct.groupid, \
                                                             XX_NWK_MULTICAST_SEND_METHOD,\
                                                             ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_DISABLE_DEFAULT_RESPONSE_MASK, \
                                                             scene_struct.clusterId, \
                                                             scene_struct.zclCmd,\
                                                             NULL, \
                                                             0);
                      if (status != EMBER_SUCCESS)
                      {
                          xxIrqButtonHandlePrintln("ji xian send XX_TRIGGER_UNICAST_MODE error : 0x%x",  status);
                      }
                  }
                  break;

             case XX_ALPHA_CLUSTER_TRIGGER_NULL_MODE:
                  {
                      xxIrqButtonHandlePrintln("xx this is XX_TRIGGER_GROUP_MODE");
                      return;
                  }
                  break;

             default:
                    {
                        xxIrqButtonHandlePrintln("error and xx_scene_struct.triggerMode is :",scene_struct.triggerMode);
                    }
                    break;
         }
#endif
     }

    xxIrqButtonHandlePrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}


void xxIrqButtonHandleButton5PressedShortCallback( uint16_t timePressedMs )
{
    
}



void xxIrqButtonHandleButton5PressedEventHandler( void )
{
    emberEventControlSetInactive( xxIrqButtonHandleButtonTimeOut );
    if ( xx_irq_button_handle_button5_last_state != BUTTON_STATE_LOW )
    {
        xx_irq_button_handle_button5_last_state = BUTTON_STATE_LOW;
        xxIrqButtonHandleButton5LowCallback();
    }

    switch ( xx_irq_button_handle_button5_pressed_state )
    {
        case BUTTON_PRESSED_AT_STARTUP:
        case BUTTON_IDLE:
            {
                xx_irq_button_handle_button5_pressed_state = BUTTON_PRESSED_SHORT;
                emberEventControlSetDelayMS( xxIrqButtonHandleButton5PressedEvent, XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS );
                xx_irq_button_handle_button5_timer = halCommonGetInt16uMillisecondTick();
            }
            break;
        case BUTTON_PRESSED_SHORT:
            {
                xx_irq_button_handle_button5_pressed_state = BUTTON_PRESSED_LONG;
            }
            //break;
        case BUTTON_PRESSED_LONG:
            {
                emberEventControlSetInactive( xxIrqButtonHandleButton5PressedEvent );
                xxIrqButtonHandleButton5PressingCallback();
            }
            break;
        default :
            {

            }
            break;
    }
}

void xxIrqButtonHandleButton5ReleasedEventHandler( void )
{
    uint16_t timePressed = halCommonGetInt16uMillisecondTick() - xx_irq_button_handle_button5_timer;
    emberEventControlSetInactive( xxIrqButtonHandleButton5ReleasedEvent );
    emberEventControlSetInactive( xxIrqButtonHandleButton5PressedEvent );
    
    if ( xx_irq_button_handle_button5_last_state != BUTTON_STATE_HIGH )
    {
        xx_irq_button_handle_button5_last_state = BUTTON_STATE_HIGH;
        xxIrqButtonHandleButton5HighCallback();
    }

    xx_irq_button_handle_button5_counter = timePressed;
    if ( timePressed >= XX_IRQ_BUTTON_HANDLE_BUTTON_LONG_TIME_OUT_MS )
    {
        xxIrqButtonHandleButton5PressedLongCallback( timePressed, xx_irq_button_handle_button5_pressed_state == BUTTON_PRESSED_AT_STARTUP );
        xxIrqButtonHandleClearButtonCounters();
    }
    else
    {
        xxIrqButtonHandleButton5PressedShortCallback( xx_irq_button_handle_button5_counter );
        emberEventControlSetActive( xxIrqButtonHandleButtonTimeOut );
    }
    xx_irq_button_handle_button5_pressed_state = BUTTON_IDLE;
}

#endif

//event time out clear
void xxTestEventHandler( void )
{
    emberEventControlSetInactive( xxTestEventControl );
    xxIrqButtonHandlePrintln("xx_irq_button_handle_button = %d", xx_irq_button_handle_button);
}


void xxIrqButtonHandleFunction(uint8_t button, uint8_t state)
{

#ifdef XX_IRQ_BSP_BUTTON0
    if ( button == XX_IRQ_BSP_BUTTON0 )
    {
        if ( state == XX_IRQ_BUTTON_HANDLE_BUTTON_PRESSED )
        {
            if ( xx_irq_button0_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton0PressedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton0ReleasedEvent );
            }
            
        }
        else
        {
            if ( xx_irq_button0_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton0ReleasedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton0PressedEvent );
            }
        }
    }

#endif

#ifdef XX_IRQ_BSP_BUTTON1
    if ( button == XX_IRQ_BSP_BUTTON1 )
    {
        if ( state == XX_IRQ_BUTTON_HANDLE_BUTTON_PRESSED )
        {
            if ( xx_irq_button1_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton1PressedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton1ReleasedEvent );
            }
            
        }
        else
        {
            if ( xx_irq_button1_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton1ReleasedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton1PressedEvent );
            }
        }
    }

#endif

#ifdef XX_IRQ_BSP_BUTTON2
    if ( button == XX_IRQ_BSP_BUTTON2 )
    {
        if ( state == XX_IRQ_BUTTON_HANDLE_BUTTON_PRESSED )
        {
            if ( xx_irq_button2_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton2PressedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton2ReleasedEvent );
            }
            
        }
        else
        {
            if ( xx_irq_button2_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton2ReleasedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton2PressedEvent );
            }
        }
    }

#endif

#ifdef XX_IRQ_BSP_BUTTON3
    if ( button == XX_IRQ_BSP_BUTTON3 )
    {
        if ( state == XX_IRQ_BUTTON_HANDLE_BUTTON_PRESSED )
        {
            if ( xx_irq_button3_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton3PressedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton3ReleasedEvent );
            }
            
        }
        else
        {
            if ( xx_irq_button3_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton3ReleasedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton3PressedEvent );
            }
        }
    }

#endif

#ifdef XX_IRQ_BSP_BUTTON4
    if ( button == XX_IRQ_BSP_BUTTON4 )
    {
        if ( state == XX_IRQ_BUTTON_HANDLE_BUTTON_PRESSED )
        {
            if ( xx_irq_button4_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton4PressedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton4ReleasedEvent );
            }
            
        }
        else
        {
            if ( xx_irq_button4_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton4ReleasedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton4PressedEvent );
            }
        }
    }

#endif

#ifdef XX_IRQ_BSP_BUTTON5
    if ( button == XX_IRQ_BSP_BUTTON5 )
    {
        if ( state == XX_IRQ_BUTTON_HANDLE_BUTTON_PRESSED )
        {
            if ( xx_irq_button5_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton5PressedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton5ReleasedEvent );
            }
            
        }
        else
        {
            if ( xx_irq_button5_Polarity == XX_IRQ_BUTTON_HANDLE_POLARITY_ACTIVE_LOW )
            {
                emberEventControlSetActive( xxIrqButtonHandleButton5ReleasedEvent );
            }
            else
            {
                  emberEventControlSetActive( xxIrqButtonHandleButton5PressedEvent );
            }
        }
    }

#endif
}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


