/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                                               .
***************************************************************************************************/
#ifndef _XX_IAS_H
#define _XX_IAS_H

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
#define XX_IAS_STATUS_PAYLOAD_LEN                                   6
#define XX_IAS_READ_TAMPER_PIN()                                    GPIO_PinInGet(XX_IRQ_BSP_BUTTON2_PORT, XX_IRQ_BSP_BUTTON2_PIN)

#define XX_IAS_MOTION_SENSOR_ZONE_CHANGE_NOTIFICATION_BIT                                    (1<<0)
#define XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT                                                 (1<<2)
#define XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT                                              (1<<3)

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef struct 
{
    uint8_t rw;//r w mode
    uint8_t data[30];//read or write data
    uint8_t readLen;//read ledn
}xxRwAttributeData_t;

extern xxRwAttributeData_t Xx_rw_attribute_data;

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void XxIasZoneStatusChangeNotificationFunction( uint8_t endpoint, uint16_t newStatus, uint16_t timeSinceStatusOccurredQs );
extern void XxIasZoneStatusChangeNotificationClearFunction( uint8_t trigerBit );
extern void XxIasZoneStatusChangeNotificationTriggerFunction( uint8_t trigerBit );

extern xxRwAttributeData_t XxReadOrWriteFunction( uint8_t rw, uint8_t endpoint, uint16_t cluster, \
                                        uint16_t attribute, EmberAfAttributeType dataType, xxRwAttributeData_t data );
extern void XxIasZoneResetUpdateTamperPinFunction( void );


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


