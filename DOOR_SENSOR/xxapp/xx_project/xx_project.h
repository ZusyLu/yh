/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.         .
***************************************************************************************************/
#ifndef _XX_PROJECT_H
#define _XX_PROJECT_H

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
*                                              全局宏定义
***************************************************************************************************/

#define XX_MAIN_START_1
#define XX_MAIN_START_10
#define XX_SLEEPY_END_DEVICE                                               
//#define XX_NO_SLEEPY                                                                        return false;
//flash
// flash read or write
#define ERASE_PAGE_SIZE(N)                                                                  (N*8*1024)//page per 8kb
#define FLASH_1024_SIZE(N)                                                                  (N*1024) 

#define XX_PROJECT_FLASH_DATA_LEN                                                           400
#define XX_PROJECT_FLASH_TRIAD_ADDR                                                         0X84000   //400k     
#define XX_PROJECT_PRODUCT_ID_ADDR                                                          (XX_PROJECT_FLASH_TRIAD_ADDR + 0X107) //263   
#define XX_PROJECT_DEVICE_NAME_ADDR                                                         (XX_PROJECT_FLASH_TRIAD_ADDR + 0X114)//276   
#define XX_PROJECT_DEVICE_ID_ADDR                                                           (XX_PROJECT_FLASH_TRIAD_ADDR + 0X129)//297   

//Triple
#define XX_PROJECT_PRODUCT_ID_LEN                                                           (8+1)
#define XX_PROJECT_DEVICE_ID_LEN                                                            (0X13+1)
#define XX_PROJECT_DEVICE_NAME_LEN                                                          (0X0C+1)


#define XX_PROJECT_POWER_UP_AND_SCAN_NETWORK   
#define XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_TIME_MS                                        1000
#define XX_PROJECT_SCAN_NETWORK_COMPLETE_NEXT_TIME_MS                                       300

#define XX_PROJECT_MAX_RANDOM_NUMBER                                                        8000
#define XX_PROJECT_SCAN_NETWORK_INTERVAL_TIME_MS                                            (1000*8)

#define XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED                                      0X08
#define XX_PROJECT_CLOSE_SCAN_NETWORK_BLINK_LED                                             1

#define XX_PROJECT_POWER_UP_AND_SCAN_NETWORK_BLINK_LED_TIME_MS                     (1000*2)

#define XX_PROJECT_SCAN_NETWORK_MAX_NUMBER                                         30

//network status change
#define XX_PROJECT_NETWORK_UP_BLINK_LED_FREQUENCY                                  3
#define XX_PROJECT_NETWORK_UP_BLINK_LED_TIME                                       100

#define XX_PROJECT_NETWORK_DOWN_BLINK_LED_FREQUENCY                                5
#define XX_PROJECT_NETWORK_DOWN_BLINK_LED_TIME                                     100

#define XX_PROJECT_TIME_S(N)                                                       (N*1000)
#define XX_PROJECT_TIME_MS(N)                                                       (N)


#define XX_PROJECT_BUTTON_INTERVAL_TIME                                            (15*1000)
#define XX_PROJECT_FACTORY_MIN_TIME                                                3200
#define XX_PROJECT_FACTORY_MAX_TIME                                                8000
#define XX_PROJECT_TRIGGER_TIME_OUT                                                (5*1000)
#define XX_PROJECT_REBOOT_DO_TIME                                                  50

//close ota 
#ifdef XX_OTA_MECHANISM
//#undef  XX_OTA_MECHANISM
#endif

//alpha 
#define XX_PROJECT_ALPHA_MAC_LED                                                   8

#define XX_PROJECT_SOURCE_ENPOINT                                                  1
#define XX_PROJECT_REPORT_DEST_ENPOINT                                             1 
#define XX_PROJECT_GW_SHROT_ADDRESS                                                0
#define XX_PROJECT_UNICASE_SEND_METHOD                                             0
#define XX_PROJECT_GROUP_DEST_ENPOINT                                              0xff

#define XX_PROJECT_RECALL_SCENE_DEST_ENPOINT                                       0xff
#define XX_PROJECT_BROADCAST_ADDRESS                                               0xffff
#define XX_PROJECT_LEAVE_NET_WORK_COUNTER_TIMES                                    8
#define XX_PROJECT_WAITING_FOR_CONFIGURATION_KEY_CUNT                              3
#define XX_PROJECT_WAITING_FOR_CONFIGURATION_LED_FLASHINGK_COUNTER_TIMES           6
#define XX_PROJECT_CFG_POLL_NUMBER_CUUNTER                                         10



#define XX_PROJECT_HEART_BEAT_EVENT_REPORT_DEFAULT_TIME                            0X0A


#define XX_PROJECT_ZCL_ATTRIBUTE_DATA_LEN                                          70

#define XX_PROJECT_LOW_POWERMV                                                     2300

#define XX_PROJECT_GW_NODE_ID                                                      0X0000

#define XX_POWER_ON_READ_ADC_TIME_MS                                               500
#define XX_PROJECT_SHROT_POLL_TIME                                                (10*1000)
#define XX_PROJECT_WAIT_CFG_POLL_CUNT                                             60

#define XX_PROJECT_ENDPIONT                                                        1

#define XX_PROJECT_READ_MODE                                                       1
#define XX_PROJECT_WRITE_MODE                                                      2
#define XX_PROJECT_READ_WRITE_ERROR                                                0xff

//led blink
//no parent led blink
#define XX_PROJECT_NO_PARENT_LED_BLINK_CUNT                                         3
#define XX_PROJECT_NO_PARENT_LED_BLINK_TIME                                         100
#define XX_PROJECT_NO_PARENT_REJOIN_NWK_TIME                                        500
#define XX_PROJECT_NO_PARENT_REJOIN_NWK                                             emberEventControlSetDelayMS( xx_project_scan_network_event, XX_PROJECT_NO_PARENT_REJOIN_NWK_TIME )
#define XX_PROJECT_NO_PARENT_LED_BLINK                                              xxBlinkMultiLedBlinkBlink( XX_PROJECT_NO_PARENT_LED_BLINK_CUNT,\
                                                                                                               XX_PROJECT_NO_PARENT_LED_BLINK_TIME,\
                                                                                                               XX_BLINK_LED_BSP_LED0 )


/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/


/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef struct
{
    uint8_t productId[XX_PROJECT_PRODUCT_ID_LEN];
    uint8_t deviceId[XX_PROJECT_DEVICE_ID_LEN];
    uint8_t deviceName[XX_PROJECT_DEVICE_NAME_LEN];
    
}xxTriple_t;

/***************************************************************************************************
*                                             FUNCTION
*                                             全局变量
***************************************************************************************************/

extern uint8_t Xx_project_scan_network_cunt;
extern EmberEventControl xx_project_scan_network_event;
extern EmberEventControl xx_project_button_interval_time_event;
extern EmberEventControl xx_project_wait_cfg_cmd_event;

extern tokType_xx_scene_t Xx_project_scene_action;
extern uint8_t Xx_project_will_leave_flat;
extern uint8_t Xx_project_key_for_config_flat;
extern uint8_t Xx_project_global_data[];
extern bool Xx_project_sleepy_end_device_flat;
extern xxTriple_t Xx_project_triple;
extern uint8_t Xx_project_zcl_attribute_data[];
extern bool Xx_whether_ota;
extern uint8_t Xx_project_press_falt;
extern uint8_t Xx_project_released_falt;
extern uint8_t Xx_project_poll_cunt;
extern uint8_t Xx_key_for_leave_cunt;
/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void XxMainStart1(void);
extern void xxProjectDisableJTAGPort( void );
extern void XxLeaveNetworkFuction(void);
extern void XxIasMotionSensorScanNetworkBlinkLedRefreshFuction( void );
extern void XxWriteAttribute(uint8_t endpoint, EmberAfClusterId cluster, EmberAfAttributeId attribute, bool serverAttribute, uint8_t  dataType, uint8_t* data);
extern void XxCommonWriteTriplefunction( void );

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


