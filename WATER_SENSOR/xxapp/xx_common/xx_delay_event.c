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

#ifdef _XX_EXAMPLE_H
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
#define xxDelayEventPrint                                   emberAfAppPrint
#define xxDelayEventPrintln                                 emberAfAppPrintln
#define xxDelayEventPrintBuffer                             emberAfAppPrintBuffer



/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/
DelayEvent Xx_delay_event[XX_DELAY_EVENT_MAX_NUMBER];
uint8_t delayIndex = 0xff;


/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/

void xxDelayEventInit(void) 
{
    uint8_t i;  
    for (i = 0; i < XX_DELAY_EVENT_MAX_NUMBER; i++)
    {  
        
        Xx_delay_event[i].type = XX_NO_DELAY_EVENT;
      
    }       
}


uint8_t XxDelayEventFindEvent(uint8_t srartIndex, uint8_t delayType) 
{
    uint8_t i;
    //uint32_t latalyTime = ~0;
    uint8_t latalyIndex = 0xff;

    for (i = srartIndex; i < XX_DELAY_EVENT_MAX_NUMBER; i++) 
    {

        if (Xx_delay_event[i].type == delayType) 
        {
             return i;
        #if 0
            if (Xx_delay_event[i].time < latalyTime) 
            {
                latalyTime = Xx_delay_event[i].time;
                latalyIndex = i;
            }
        #endif
        }
    }

    return latalyIndex;
}

uint8_t XxDelayEventFindFreeLocation(void)
{
    uint8_t i;
    uint8_t index = 0xff;

    for (i = 0; i < XX_DELAY_EVENT_MAX_NUMBER; ++i)
    {
        if (Xx_delay_event[i].type == XX_NO_DELAY_EVENT)
        {
            Xx_delay_event[i].time = 0;
            index = i;
            return index;
        }
    }

    return index;
}


uint8_t XxDelayEventSetEvent(uint8_t event, uint32_t time, uint8_t coverTime)
{
    delayIndex = XxDelayEventFindEvent(0, event);    
    if(delayIndex == 0xff)
    {         
        delayIndex = XxDelayEventFindFreeLocation();
        if(delayIndex == 0xff)//error
        {
            xxDelayEventPrintln("xx set event error ");  
            return 0;
        }
        
        Xx_delay_event[delayIndex].type = event; 
        Xx_delay_event[delayIndex].time = time;     
    }
    else if(coverTime)
    {
        Xx_delay_event[delayIndex].type = event; 
        Xx_delay_event[delayIndex].time = time; 
        
    }

    if( time == 0 )
    {
        Xx_delay_event[delayIndex].type = XX_NO_DELAY_EVENT; 
        Xx_delay_event[delayIndex].time = time; 
    }
    return true;
}

void xxDelayEventTick(uint32_t time) 
{
    #ifdef XX_SLEEPY_END_DEVICE
        if ( !XX_SERIAL_TEST_MODE_FLAG )
        {
            return;
        }
    #endif
    
    static uint32_t lastBlinkTime = 0; 
    uint32_t tensTickToTakeOff;
    uint8_t i;  
    time = halCommonGetInt32uMillisecondTick(); 
    tensTickToTakeOff=(time - lastBlinkTime); 

    if (tensTickToTakeOff > 1) 
    {
        lastBlinkTime = time;  
        for (i = 0; i < XX_DELAY_EVENT_MAX_NUMBER; i++) 
        {             
            if (Xx_delay_event[i].time == 0)
                continue;  
            
            if(tensTickToTakeOff > Xx_delay_event[i].time)
            {  
                Xx_delay_event[i].time = 0; 
            }
            else
            {  
                Xx_delay_event[i].time = Xx_delay_event[i].time-tensTickToTakeOff;
            }

            if (Xx_delay_event[i].time == 0) 
            {  
                if(XX_SERIAL_TEST_MODE_FLAG)
                {
                    xxMfgDelayEventMfgModelFunction(i,Xx_delay_event[i].type);  //mfg model
                }
                else
                {
                    //XxLightDelayEventHandle(i,Xx_delay_event[i].type); 
                }
                             
            }
        }
    }
}










/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


