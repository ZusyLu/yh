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

#ifdef _XX_HEART_BEAT_H
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
#define xxHeartBeatPrintln                                  emberAfAppPrintln
#define xxHeartBeatPrintBuffer                              emberAfAppPrintBuffer

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
EmberEventControl xx_project_heart_beat_event;

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
static uint8_t xx_beart_beat_time = 0;
/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
uint8_t XxHeartBeatGetHeartBeatTime( void )
{
    return xx_beart_beat_time;
}

uint8_t XxHeartBeatSetHeartBeatTime( uint8_t *time )
{
    return ( xx_beart_beat_time = *time );
}

void XxHeartBeatActiveOrReActiveCallback( void )
{
    EmberAfStatus status;
    uint8_t data[20]={0};
    uint8_t dataType = ZCL_INT8U_ATTRIBUTE_TYPE;
    emberEventControlSetInactive( xx_project_heart_beat_event );
    
    status = emberAfReadAttribute(XX_PROJECT_ENDPIONT,
                                ZCL_IDENTIFY_CLUSTER_ID,
                                XX_ZCL_ATTRIBUTE_ID_HEARTBEAT_TIME,
                                CLUSTER_MASK_SERVER,
                                data,
                                sizeof(data),
                                &dataType);
    xxHeartBeatPrintln("xx read time is %d minute", data[0]);
    XxHeartBeatSetHeartBeatTime( &data[0] );
    emberEventControlSetDelayMinutes( xx_project_heart_beat_event, XX_HEART_BEAT_TIMING_REPORT_TIME );//emberEventControlSetDelayMS
    if (status != EMBER_ZCL_STATUS_SUCCESS)
    {
        xxHeartBeatPrintln(" xx read heart beat time is fail %x ", status);
    }
}

void XxHeartBeatInActiveCallback( void )
{
    emberEventControlSetInactive( xx_project_heart_beat_event );
}


void xxProjectHeartBeatEventHandler( void )
{
    emberEventControlSetInactive( xx_project_heart_beat_event );
    if ( emberAfNetworkState() == EMBER_JOINED_NETWORK )
    {
        uint16_t readA[1] = { ZCL_XX_DEVICE_CIE_ADDRESS_ATTRIBUTE_ID };
        //XxReportSpecificAttributeEx( ZCL_IDENTIFY_CLUSTER_ID, 1,readA,1,1,0x0000);
    }
    #ifdef XX_HEART_BEAT_ACTIVE_CALL_BACK
        XX_HEART_BEAT_ACTIVE_CALL_BACK
    #endif
}



/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


