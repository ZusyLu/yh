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

#ifdef _XX_PROJECT_H
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
#define xxIasMotionSensorPrint                                                               emberAfAppPrint
#define xxIasMotionSensorPrintln                                                             emberAfAppPrintln
    
#define xxIasMotionSensorPrintBuffer                                                         emberAfAppPrintBuffer
#define xxIasMotionSensorPrintString                                                         "XX_IAS_MOTION_SENSOR:"

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/

#define XX_IAS_MOTION_SENSOR_GLOBAL_DATA_LEN                                                     128

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/
bool Xx_whether_ota;
bool Xx_project_sleepy_end_device_flat = 0;

EmberEventControl xx_project_scan_network_event;
EmberEventControl xx_project_button_interval_time_event;
EmberEventControl xx_project_wait_cfg_cmd_event;

uint8_t Xx_key_for_leave_cunt = 0;
uint8_t Xx_project_scan_network_cunt = 0;
uint8_t Xx_project_will_leave_flat = 0;
uint8_t Xx_project_key_for_config_flat = 0;// no use
uint8_t Xx_project_global_data[XX_IAS_MOTION_SENSOR_GLOBAL_DATA_LEN];
uint8_t Xx_project_mac_addr[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
xxTriple_t Xx_project_triple;
uint8_t Xx_project_zcl_attribute_data[XX_PROJECT_ZCL_ATTRIBUTE_DATA_LEN];
uint8_t Xx_project_press_falt = true;
uint8_t Xx_project_released_falt = true;
uint8_t Xx_project_poll_cunt = 0;
EmberEventControl xxInitEvent;
EmberEventControl xxRebootEventControl;



/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
tokType_xx_scene_t Xx_project_scene_action;
static uint8_t xx_ias_motion_sensor_time_for_config_cunt = 0;


/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
static void clearNetworkTables(void)
{
    uint8_t endpointIndex;
    uint8_t endpoint;

    emberClearBindingTable();
    emberAfClearReportTableCallback();
    for (endpointIndex = 0; endpointIndex < emberAfEndpointCount();
       endpointIndex++) {
        endpoint = emberAfEndpointFromIndex(endpointIndex);
        emberAfResetAttributes(endpoint);
        emberAfGroupsClusterClearGroupTableCallback(endpoint);
        emberAfScenesClusterClearSceneTableCallback(endpoint);
    }
}

static void xxIasMotionSendorWriteMacAddr( void )
{
    unsigned char i,temp[8];
    MEMMOVE(temp, emberGetEui64(), EUI64_SIZE); 

    for(i = 0; i < EUI64_SIZE; i++)
    {
        Xx_project_mac_addr[i] = temp[EUI64_SIZE-1-i];
    }
    XxWriteAttribute(1, ZCL_IDENTIFY_CLUSTER_ID, ZCL_XX_DEVICE_CIE_ADDRESS_ATTRIBUTE_ID, 1, ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE, Xx_project_mac_addr);
}

static void xxIasMotionSendorClearTriggerFlat( void )
{
    Xx_project_press_falt = true;
    Xx_project_released_falt = true; 
}

static void xxIasMotionSendorRebootStartUpEventFunction( void )
{
    emberEventControlSetDelayMS( xxRebootEventControl, XX_PROJECT_REBOOT_DO_TIME );
    emberEventControlSetDelayMS(xxInitEvent,XX_POWER_ON_READ_ADC_TIME_MS);
}

static void xxIasMotionSendorWriteAndReadVersion( void )
{
    //xxIasMotionSensorPrintln("jixian image type = %2x ",EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID);
    //xxIasMotionSensorPrintln("jixian application version = %4x ",CUSTOMER_APPLICATION_VERSION);
    emberProcessCommandString((uint8_t*)"write 1 0 0x4000 1 0x42 \"1.0.2\"", strlen("write 1 0 0x4000 1 0x42 \"1.0.2\""));
    emberProcessCommandString((uint8_t*)"\n", strlen("\n"));
    xxIasMotionSensorPrintln("\r\n xx Ias door Sensor version:");
    emberProcessCommandString((uint8_t*)"read 1 0 0x4000 1", strlen("read 1 0 0x4000 1"));
    emberProcessCommandString((uint8_t*)"\n", strlen("\n")); 
    xxIasMotionSensorPrintln("");
}

//led blink init
void xxIasMotionSensorHalInit( void )
{
#ifdef XX_IRQ_BUTOON_INIT
    xxIrqButtonInit();//irq and time to wake up
#endif
    
#ifdef XX_BLINK_LED_INIT
    XxBlinkLedInit();
#endif

    xxIasMotionSendorRebootStartUpEventFunction();
}

void xxInitEventHandler( void )
{
    emberEventControlSetInactive( xxInitEvent );
    emberAfCorePrintln("######### xx init");
    if ( !halGetBatteryVoltageMilliV() )
    {
        emberEventControlSetDelayMS(xxInitEvent,XX_POWER_ON_READ_ADC_TIME_MS);
    }
    emberEventControlSetDelayMS(emberAfPluginBatteryMonitorReadADCEventControl,XX_POWER_ON_READ_ADC_TIME_MS);
    //xxIrqButtonInit();
}


//init

void XxMainStart1(void)
{
    XxMainInitCbRegister( xxIasMotionSensorHalInit );
    //XxMainTickCbRegister(XxHalPoll);
}

void XxMainStart10(void)
{
    XxMainStackStatusCbRegister(XxZbStackStatusCallback);
    XxMainInitCbRegister(xxDelayEventInit);
    XxMainTickCbRegister(xxDelayEventTick);
}

//timed event handler

void xxProjectScanNetworkFuction( void )
{
    EmberStatus status;
    EmberNetworkStatus networkStatus;
    networkStatus = emberNetworkState();
    static uint8_t xxJoiningCunt = 0;
    emberEventControlSetInactive( xx_project_scan_network_event );
    
    xxIasMotionSensorPrintln("xx networkStatus = %x ",networkStatus );
    switch ( networkStatus )
        {
            case EMBER_NO_NETWORK:
                {
                     //emberLeaveNetwork();
                    //Xx_project_scan_network_cunt++;
                    xxJoiningCunt = 0;
                    if ( Xx_project_scan_network_cunt < XX_PROJECT_SCAN_NETWORK_MAX_NUMBER )
                    {
                        xxBlinkMultiLedBlinkBlink( XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED,\
                                                   XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED_TIME_MS, \
                                                   XX_BLINK_LED_BSP_LED0 );
                        status = emberAfPluginNetworkSteeringStart();
                        if(status == EMBER_SUCCESS)
                        {
                            xxIasMotionSensorPrintln("xx scan networking ...");
                        }
                        else
                        {
                            xxIasMotionSensorPrintln("xx scan networking  error and status = %x ",status);
                        }
                    }
                    else
                    {
                        xxBlinkMultiLedBlinkBlink( XX_PROJECT_CLOSE_SCAN_NETWORK_BLINK_LED,\
                                                   XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED_TIME_MS, \
                                                   XX_BLINK_LED_BSP_LED0 );
                    }


                }
                break;
            
            case EMBER_JOINING_NETWORK:
                {
                    xxJoiningCunt++;
                    if ( xxJoiningCunt >= (XX_PROJECT_SCAN_NETWORK_MAX_NUMBER-1) )
                    {
                        //reboot
                        halReboot();
                    }
                }
                break;

            case EMBER_JOINED_NETWORK:
                {
                    xxJoiningCunt = 0;
                }
                break;

            case EMBER_JOINED_NETWORK_NO_PARENT:
                {
                    xxJoiningCunt = 0;
                    //can set time event for next run
                    emberAfAppPrintln("xx will try rejoin");
                    emberAfStartMoveCallback();
                }
                break;

            default :
                {
                    xxJoiningCunt = 0;
                }
                break;
        }

}

void XxIasMotionSensorScanNetworkBlinkLedRefreshFuction( void )
{
    xxIasMotionSensorPrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    if ( Xx_project_scan_network_cunt < XX_PROJECT_SCAN_NETWORK_MAX_NUMBER &&\
                                                  Xx_project_scan_network_cunt != 0 &&\
                                                 emberNetworkState() != EMBER_JOINED_NETWORK )
    {
        xxBlinkMultiLedBlinkBlink( XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED,\
                                   XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED_TIME_MS, \
                                                                            XX_BLINK_LED_BSP_LED0 );
    }
}

#if 1
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
    xxIasMotionSensorPrintln("Network Steering Completed: %p (0x%X)",
                     (status == EMBER_SUCCESS ? "Join Success" : "FAILED"),
                     status);
    xxIasMotionSensorPrintln("Finishing state: 0x%X", finalState);
    xxIasMotionSensorPrintln("Beacons heard: %d\nJoin Attempts: %d", totalBeacons, joinAttempts);
    if (status == EMBER_SUCCESS)
    {
        emberEventControlSetInactive(xx_project_scan_network_event);
    }
    else 
    {
        Xx_project_scan_network_cunt++;
        emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_SCAN_NETWORK_COMPLETE_NEXT_TIME_MS );
    }

    #if 0
    if ( emberNetworkState() != EMBER_JOINED_NETWORK )
    {
        Xx_project_scan_network_cunt++;
        if ( Xx_project_scan_network_cunt < XX_PROJECT_SCAN_NETWORK_MAX_NUMBER )
        {
             //emberEventControlSetDelayMS( xx_project_scan_network_event, XxjitterTimeDelayMs( (uint32_t)XX_PROJECT_MAX_RANDOM_NUMBER, \
             //                                                                       (uint32_t)XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_TIME_MS ) );
        }
    }
    #endif

}
#endif

void xxProjectButtonIntervalTimeFuction( void )
{
    EmberNetworkStatus state;
    emberEventControlSetInactive( xx_project_button_interval_time_event );
    Xx_project_press_falt = true;
    Xx_project_released_falt = true;
	Xx_key_for_leave_cunt = 0;//clear
}

void xxProjectWaitCfgCmdFuction( void )
{
    EmberStatus status;
    emberEventControlSetInactive( xx_project_wait_cfg_cmd_event );
    //emberEventControlSetDelayMS( xx_project_wait_cfg_cmd_event, XX_PROJECT_SHROT_POLL_TIME );
    Xx_project_poll_cunt++;
    if ( Xx_project_poll_cunt > XX_PROJECT_WAIT_CFG_POLL_CUNT )
    {
        //Xx_project_poll_cunt = 0;
        //emberEventControlSetInactive( xx_project_wait_cfg_cmd_event );
    }
     emberAfRemoveFromCurrentAppTasks(EMBER_AF_FORCE_SHORT_POLL);
}

//event


//leave network
void XxLeaveNetworkFuction(void)
{
    EmberStatus status;
    Xx_project_will_leave_flat = false;
    Xx_project_scan_network_cunt = 0;
    XxIrqButtonHandleClearButtonCountersTimes();
    //emberAfResetAttributes(emberAfCurrentEndpoint());
    clearNetworkTables();
    emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_TIME_S(3) );
    emberLeaveNetwork();
}

bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs)
{   
	return true;
}

bool emberAfPluginIdleSleepOkToIdleCallback(void)
{   
	return true;
}

void XxWriteAttribute(uint8_t endpoint, EmberAfClusterId cluster, EmberAfAttributeId attribute, bool serverAttribute, uint8_t  dataType, uint8_t* data)
{
    //uint8_t i;
  EmberAfStatus status;

  status = emberAfWriteAttribute(endpoint,
                                 cluster,
                                 attribute,
                                 (serverAttribute
                                  ? CLUSTER_MASK_SERVER
                                  : CLUSTER_MASK_CLIENT),
                                 data,
                                 dataType);
  emAfPrintStatus("write", status);
  xxIasMotionSensorPrintln("");
}

void XxCommonWriteTriplefunction( void )
{
#ifdef XX_LIGHT_TRIPLE_TEST
    uint8_t i = 0;
    xxIasMotionSensorPrintln("JI XIAN  TEST");
    xxLightPrintBuffer(xx_mac_addr, 8, TRUE);
    xxIasMotionSensorPrintln("  "); 
    for(i=0;i < XX_LIGHT_TRIPLE_NUMBER;i++)
    {
        if( memcmp(xx_mac_addr,Xx_Light_triple[i].macId,8) )
        {            
            continue;
        }
        else
        {
            xxIasMotionSensorPrintln("xx test Triad:");
//            xxLightPrintBuffer(" string productId is : %s ",&Xx_Light_triple[i].productId[1]);
//            xxLightPrintBuffer(" string deviceId is : %s ",&Xx_Light_triple[i].deviceId[1]);
//            xxLightPrintBuffer(" string deviceName is : %s ",&Xx_Light_triple[i].deviceName[1]);
            Xx_Light_triple[i].productId[0] = XX_LIGHT_PRODUCT_ID_LEN-1;                      
            Xx_Light_triple[i].deviceId[0] = XX_LIGHT_DEVICE_ID_LEN-1;
            Xx_Light_triple[i].deviceName[0] = XX_LIGHT_DEVICE_NAME_LEN-1;
            XxLightWriteAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID, 1, ZCL_CHAR_STRING_ATTRIBUTE_TYPE, Xx_Light_triple[i].productId);          
            XxLightWriteAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_PRODUCT_URL_ATTRIBUTE_ID, 1, ZCL_CHAR_STRING_ATTRIBUTE_TYPE, Xx_Light_triple[i].deviceId);
            XxLightWriteAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_LOCATION_DESCRIPTION_ATTRIBUTE_ID, 1, ZCL_CHAR_STRING_ATTRIBUTE_TYPE, Xx_Light_triple[i].deviceName);
            return;
        }
        
    }
    xxIasMotionSensorPrintln("xx test have not find Triad");
#else
    xxIasMotionSensorPrintln("ji xian  triple");
#if 0
    emberAfPluginEepromRead(XX_PROJECT_PRODUCT_ID_ADDR,\
                            &Xx_project_triple.productId[1],
                            XX_PROJECT_PRODUCT_ID_LEN-1);

    emberAfPluginEepromRead(XX_PROJECT_DEVICE_ID_ADDR,\
                            &Xx_project_triple.deviceId[1],
                            XX_PROJECT_DEVICE_ID_LEN-1);

    emberAfPluginEepromRead(XX_PROJECT_DEVICE_NAME_ADDR,\
                            &Xx_project_triple.deviceName[1],
                            XX_PROJECT_DEVICE_NAME_LEN-1);
#endif
    Xx_project_triple.productId[0] = XX_PROJECT_PRODUCT_ID_LEN-1;                      
    Xx_project_triple.deviceId[0] = XX_PROJECT_DEVICE_ID_LEN-1;
    Xx_project_triple.deviceName[0] = XX_PROJECT_DEVICE_NAME_LEN-1;
    //xxIasMotionSensorPrintln("xx Triad:");
    //xxIasMotionSensorPrintln(" string productId is : %s ",&Xx_project_triple.productId[1]);
    //xxIasMotionSensorPrintln(" string deviceId is : %s ",&Xx_project_triple.deviceId[1]);
   // xxIasMotionSensorPrintln(" string deviceName is : %s ",&Xx_project_triple.deviceName[1]);


    XxWriteAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID, 1, ZCL_CHAR_STRING_ATTRIBUTE_TYPE, Xx_project_triple.productId);          
    XxWriteAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_PRODUCT_URL_ATTRIBUTE_ID, 1, ZCL_CHAR_STRING_ATTRIBUTE_TYPE, Xx_project_triple.deviceId);
    XxWriteAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_LOCATION_DESCRIPTION_ATTRIBUTE_ID, 1, ZCL_CHAR_STRING_ATTRIBUTE_TYPE, Xx_project_triple.deviceName);

#endif
}

void xxRebootEventHandler( void )
{
    EmberNetworkStatus state;
    state = emberAfNetworkState();
	Xx_key_for_leave_cunt = 0;
    emberEventControlSetInactive( xxRebootEventControl );
    //xxIasMotionSendorWriteMacAddr();
    xxIasMotionSendorWriteAndReadVersion();
    //XxCommonWriteTriplefunction();
    xxIasMotionSendorClearTriggerFlat();
    if ( state == EMBER_NO_NETWORK )
    {
        #ifdef XX_PROJECT_POWER_UP_AND_SCAN_NETWORK
            emberAfPluginNetworkSteeringStart();
            emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_TIME_MS );
        #endif
    }
    else if ( state == EMBER_JOINED_NETWORK_NO_PARENT )
    {
        //emberEventControlSetDelayMS( xx_project_wait_cfg_cmd_event, XX_PROJECT_TIME_MS(100) );
        //emberAfStartMoveCallback();
    }
    //emberEventControlSetDelayMS( xxRebootEventControl, XX_PROJECT_REBOOT_DO_TIME );
    //reset cunt
    XxDiagnosticsGetResetCunt();
    XxIasZoneResetUpdateTamperPinFunction();
}

//rw :read is 1; write is 2
// ZCL_IAS_ZONE_CLUSTER_ID ZCL_ZONE_STATUS_ATTRIBUTE_ID


xxRwAttributeData_t XxReadOrWriteFunction( uint8_t rw, uint8_t endpoint, uint16_t cluster, \
                                        uint16_t attribute, EmberAfAttributeType dataType, xxRwAttributeData_t data )
{
    EmberAfStatus afStatus;
    data.rw = rw;
    if ( rw == XX_PROJECT_READ_MODE )
    {
        afStatus = emberAfReadAttribute(endpoint,
                                cluster,
                                attribute,
                                CLUSTER_MASK_SERVER,
                                data.data,
                                sizeof(data.data),
                                &dataType);
        uint16_t  size = (emberAfIsThisDataTypeAStringType(dataType)
                  ? emberAfStringLength(data.data) + 1
                  : emberAfGetDataSize(dataType));
        data.readLen = (uint8_t)size;
        
        if (EMBER_ZCL_STATUS_SUCCESS != afStatus)
        {
            xxIasMotionSensorPrintln("xx read arrtibute error");
            data.rw = XX_PROJECT_READ_WRITE_ERROR;
        }
    }
    else if ( rw == XX_PROJECT_WRITE_MODE )
    {
        afStatus = emberAfWriteServerAttribute(endpoint,
                                             cluster,
                                             attribute,
                                             data.data,
                                             dataType);
        if (EMBER_ZCL_STATUS_SUCCESS != afStatus)
        {
            xxIasMotionSensorPrintln("xx write arrtibute error");
             data.rw = XX_PROJECT_READ_WRITE_ERROR;
        }
    }

        return data;
}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


