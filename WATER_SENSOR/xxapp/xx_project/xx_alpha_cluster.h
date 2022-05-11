/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.
                                                  .
***************************************************************************************************/
#ifndef _XX_ALPHA_CLUSTER_H
#define _XX_ALPHA_CLUSTER_H

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
*                                              宏定义
***************************************************************************************************/
//cluster 
#define XX_ALPHA_CLUSTER_ID                                              0X0017
//alpha cmd 
#define XX_ALPHA_CLUSTER_REMOVE_SD_COMMAND_ID                            0X02   //for scenes or other command    1 step
#define XX_ALPHA_CLUSTER_REPORT_COMMAND_ID                               0X0A
#define XX_ALPHA_CLUSTER_INITIATE_ASSKE_COMMAND_ID                       0X04  //for scenes command     2 step
#define XX_ALPHA_CLUSTER_CONFIGURE_SD_COMMAND_ID                         0X00  //for app command 2 step
#define XX_ALPHA_CLUSTER_CONFIGURE_GROUP_COMMAND_ID                      0X08
//position
#define XX_ALPHA_CLUSTER_INDEX_POSITION                                  0X03
#define XX_ALPHA_CLUSTER_GROUP_ID_H_POSITION                             7
#define XX_ALPHA_CLUSTER_GROUP_ID_L_POSITION                             6
#define XX_ALPHA_CLUSTER_MAC_ID_POSITION                                 6
#define XX_ALPHA_CLUSTER_MAC_ID_LEN                                      8
#define XX_ALPHA_CLUSTER_ID_H_POSITION                                   18
#define XX_ALPHA_CLUSTER_ID_L_POSITION                                   17
#define XX_ALPHA_CLUSTER_GROUP_CLUSTER_ID_H_POSITION                     9
#define XX_ALPHA_CLUSTER_GROUP_CLUSTER_ID_L_POSITION                     8
#define XX_ALPHA_CLUSTER_GROUP_CLUSTER_CMD_POSITION                      11
#define XX_ALPHA_CLUSTER_CMD_POSITION                                    20
#define XX_ALPHA_CLUSTER_SCENE_ID_POSITION                               8
//mode
#define XX_ALPHA_CLUSTER_TRIGGER_UNICAST_MODE                            0X01
#define XX_ALPHA_CLUSTER_TRIGGER_SCENE_MODE                              0X02
#define XX_ALPHA_CLUSTER_TRIGGER_GROUP_MODE                              0X03
#define XX_ALPHA_CLUSTER_TRIGGER_NULL_MODE                               0xff

//
#define XX_ALPHA_CLUSTER_REPORT_BUTTON_LEN                              3
#define XX_ALPHA_CLUSTER_RECALL_SCENE_PAYLOAD_LEN                       3

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void xxAlphaClusterClearTokenKeyConfigFunction( uint8_t index, tokType_xx_scene_t scene_struct );
extern void xxAlphaClusterRemoveSdCmdHandle( EmberAfClusterCommand* cmd );
extern void xxAlphaClusterInitiateAsskeCmdHandle( EmberAfClusterCommand* cmd );
extern void xxAlphaClusterConfigureSdCmdHandle( EmberAfClusterCommand* cmd );
extern void xxAlphaClusterConfigureGroupCmdHandle( EmberAfClusterCommand* cmd );

//
extern void emAfCliServiceDiscoveryCallback(const EmberAfServiceDiscoveryResult* result);


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


