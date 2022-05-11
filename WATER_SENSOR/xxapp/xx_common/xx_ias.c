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

#ifdef _XX_IAS_H
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
#define xxIasPrint                                                               emberAfAppPrint
#define xxIasPrintln                                                             emberAfAppPrintln

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
xxRwAttributeData_t Xx_rw_attribute_data;

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
static uint8_t xx_ias_status_payload_buff[XX_IAS_STATUS_PAYLOAD_LEN];
/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
uint8_t * XxIasGetIasStatusPayloadFunction( void )
{
    return xx_ias_status_payload_buff;
}

uint8_t *XxBuildIasStatusChangeNotificationPayloadFunction( uint8_t endPoint ,\
                                                            uint16_t zoneStatus, \
                                                            uint16_t timeSinceStatusOccurredS )
{
    EmberStatus sendStatus;
    uint8_t extendedStatus;
    EmberAfStatus status;
    uint8_t ZoneId;
    uint8_t i = 0;
    extendedStatus = 0;
    uint8_t *iasPayload = (uint8*)XxMemAlloc(XX_IAS_STATUS_PAYLOAD_LEN);
    if ( !iasPayload )
    {
        xxIasPrintln(" xx mem alloc error ");
        return NULL;
    }

    emberAfWriteServerAttribute(endPoint,
                          ZCL_IAS_ZONE_CLUSTER_ID,
                          ZCL_ZONE_STATUS_ATTRIBUTE_ID,
                          (uint8_t*)&zoneStatus,
                          ZCL_INT16U_ATTRIBUTE_TYPE);

    if ( !emberAfIasZoneClusterAmIEnrolled(endPoint) ) {
        xxIasPrintln(" xx not enrolled ");
    }

    extendedStatus = 0x00;
    iasPayload[i++] = zoneStatus;
    iasPayload[i++] = zoneStatus>>8;
    iasPayload[i++] = extendedStatus;
    status = emberAfReadServerAttribute( endPoint,\
                                          ZCL_IAS_ZONE_CLUSTER_ID,\
                                          ZCL_ZONE_ID_ATTRIBUTE_ID,\
                                          (unsigned char*)&ZoneId,\
                                          1);  // uint8_t size
    iasPayload[i++] = ZoneId;
    iasPayload[i++] = timeSinceStatusOccurredS;
    iasPayload[i++] = timeSinceStatusOccurredS>>8;

    return iasPayload;
}

void XxIasZoneStatusChangeNotificationFunction( uint8_t endpoint, uint16_t newStatus, uint16_t timeSinceStatusOccurredS )
{
    uint8_t destEndPoint;
    uint8_t srcEndPoint;
    srcEndPoint = 1;
    destEndPoint = endpoint;

    uint8_t *buff = XxBuildIasStatusChangeNotificationPayloadFunction( destEndPoint ,\
                                                                        newStatus, \
                                                                        timeSinceStatusOccurredS );

   XxNwkSendRfPacket( srcEndPoint,\
                      destEndPoint, \
                      XX_PROJECT_GW_NODE_ID,\
                      ZCL_FRAME_CONTROL_SERVER_TO_CLIENT|ZCL_CLUSTER_SPECIFIC_COMMAND|ZCL_DISABLE_DEFAULT_RESPONSE_MASK,\
                      ZCL_IAS_ZONE_CLUSTER_ID,\
                      ZCL_ZONE_STATUS_CHANGE_NOTIFICATION_COMMAND_ID,
                      buff,\
                      XX_IAS_STATUS_PAYLOAD_LEN );

   XxMemFree(buff);//
}

void XxIasZoneStatusChangeNotificationClearFunction( uint8_t trigerBit )
{
    uint16_t newStatus=0;
    uint8_t  endpoint;
    uint8_t  timeSinceOccurredSeconds;
    EmberNetworkStatus state;
    state = emberAfNetworkState();
    newStatus = 0;
    endpoint  = 1;
    timeSinceOccurredSeconds = 1;
    xxRwAttributeData_t iasStatus = XxReadOrWriteFunction( XX_PROJECT_READ_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                                    ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                                    ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    xxIasPrintln("iasStatus[0] = %x iasStatus[1] = %x iasStatus.len=%d ",iasStatus.data[0], iasStatus.data[1],iasStatus.readLen);
    //newStatus = iasStatus.data[0] | (iasStatus.data[1]<<8);
    newStatus = iasStatus.data[0];
    newStatus &= ~trigerBit;
    if ( halGetBatteryVoltageMilliV() < XX_PROJECT_LOW_POWERMV )
    {
        newStatus |= XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }
    else if ( !halGetBatteryVoltageMilliV() )
    {
        emberEventControlSetDelayMS(emberAfPluginBatteryMonitorReadADCEventControl,200);
    }
    else 
    {
        newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }

    #ifdef XX_IAS_READ_TAMPER_PIN()
        if ( !XX_IAS_READ_TAMPER_PIN() )
        {
            newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
        else
        {
            newStatus |= XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
    #endif 

    iasStatus.data[0] = newStatus;
    iasStatus.data[1] = newStatus>>8;
    XxReadOrWriteFunction( XX_PROJECT_WRITE_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    if ( state == EMBER_JOINED_NETWORK )
    {
        #if 0
            emberAfPluginIasZoneServerUpdateZoneStatus(endpoint,
                                                     newStatus,
                                                     timeSinceOccurredSeconds);
        #else
            XxIasZoneStatusChangeNotificationFunction( endpoint,\
                                                       newStatus, \
                                                       timeSinceOccurredSeconds );
        #endif
    }
    else if ( state == EMBER_JOINED_NETWORK_NO_PARENT )
    {   
        xxIasPrintln(" xx will rejoin ");
        emberAfStartMoveCallback();
    }
}

void XxIasZoneStatusChangeNotificationTriggerFunction( uint8_t trigerBit )
{
    
    uint16_t newStatus=0;
    uint8_t  endpoint;
    uint8_t  timeSinceOccurredSeconds;
    static uint8_t i=0 ,j= 0;
    xxRwAttributeData_t iasStatus;
    EmberNetworkStatus state;
    state = emberAfNetworkState();
    iasStatus = XxReadOrWriteFunction( XX_PROJECT_READ_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                                            ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                                            ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    //newStatus = iasStatus.data[0] | (iasStatus.data[1]<<8);
    newStatus = iasStatus.data[0];
    newStatus |= trigerBit;
    //newStatus = 1;
    endpoint  = 1;
    timeSinceOccurredSeconds = 1;
    if ( halGetBatteryVoltageMilliV() < XX_PROJECT_LOW_POWERMV )
    {
        newStatus |= XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }
    else if ( !halGetBatteryVoltageMilliV() )
    {
        emberEventControlSetDelayMS(emberAfPluginBatteryMonitorReadADCEventControl,200);
    }
    else
    {
       newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }
    
    #ifdef XX_IAS_READ_TAMPER_PIN()
        if ( !XX_IAS_READ_TAMPER_PIN() )
        {
            newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
        else
        {
            newStatus |= XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
    #endif
    
    iasStatus.data[0] = newStatus;
    iasStatus.data[1] = newStatus>>8;
    XxReadOrWriteFunction( XX_PROJECT_WRITE_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    if ( state == EMBER_JOINED_NETWORK )
    {
        #if 0
            emberAfPluginIasZoneServerUpdateZoneStatus(endpoint,
                                                     newStatus,
                                                     timeSinceOccurredSeconds);
        #else 
            XxIasZoneStatusChangeNotificationFunction( endpoint,\
                                                       newStatus, \
                                                       timeSinceOccurredSeconds );
        #endif
    }
    else if ( state == EMBER_JOINED_NETWORK_NO_PARENT )
    {   
        xxIasPrintln(" xx will rejoin ");
        emberAfStartMoveCallback();
    }

}

//TAMPER
void XxIasZoneResetUpdateTamperPinFunction( void )
{
    
    uint16_t newStatus=0;
    uint8_t  endpoint;
    uint8_t  timeSinceOccurredSeconds;
    xxRwAttributeData_t iasStatus;
    iasStatus = XxReadOrWriteFunction( XX_PROJECT_READ_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                                            ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                                            ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    //newStatus = iasStatus.data[0] | (iasStatus.data[1]<<8);
    newStatus = iasStatus.data[0];
    //newStatus = 1;
    endpoint  = 1;
    timeSinceOccurredSeconds = 1;
    
    #ifdef XX_IAS_READ_TAMPER_PIN()
        if ( !XX_IAS_READ_TAMPER_PIN() )
        {
            newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
        else
        {
            newStatus |= XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
    #endif
    
    iasStatus.data[0] = newStatus;
    iasStatus.data[1] = newStatus>>8;
    XxReadOrWriteFunction( XX_PROJECT_WRITE_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


