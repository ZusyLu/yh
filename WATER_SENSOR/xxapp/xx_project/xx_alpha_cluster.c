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

#ifdef _XX_ALPHA_CLUSTER_H
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
#define XX_ALPHA_CLUSTER_EIGHT_BETY_ALL_FF                                         { 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF }

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

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
static uint8_t xx_alpha_cluster_mac_all_ff[XX_ALPHA_CLUSTER_MAC_ID_LEN] = XX_ALPHA_CLUSTER_EIGHT_BETY_ALL_FF;

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
void xxAlphaClusterClearTokenKeyConfigFunction( uint8_t index, tokType_xx_scene_t scene_struct )
{
    uint8_t *prt;
    memset(&scene_struct.triggerIndex, 0xff, sizeof(scene_struct) );
    halCommonSetIndexedToken( TOKEN_XX_SCENE, index, &scene_struct);
    halCommonGetIndexedToken( &scene_struct, TOKEN_XX_SCENE, index );
    prt = &scene_struct.triggerIndex;
    emberAfCorePrintln("xx AlphaClusterClearToken scene_struct is :");
    for( uint8_t i = 0; i < sizeof(scene_struct); i++ )
    {
        emberAfCorePrint("%x ",prt[i] );
    }
    emberAfCorePrintln("");
}


void xxAlphaClusterRemoveSdCmdHandle( EmberAfClusterCommand* cmd )
{
    if( cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION] < XX_IRQ_BSP_BUTTON_CUNT) //0~2 is zcl head
    {
        xxAlphaClusterClearTokenKeyConfigFunction( cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION], Xx_project_scene_action );// clear index token
    }
}

void xxAlphaClusterInitiateAsskeCmdHandle( EmberAfClusterCommand* cmd )
{
    if( (uint16_t)( cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_L_POSITION] | cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_H_POSITION]<<8 ) != 0xffff ) //0~2 is zcl head
    {
        tokType_xx_scene_t scene_struct;
        scene_struct.triggerMode    = XX_ALPHA_CLUSTER_TRIGGER_SCENE_MODE;
        scene_struct.triggerIndex   = cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION];
        scene_struct.groupid        = (uint16_t)( cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_H_POSITION] | cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_H_POSITION]<<8 );
        scene_struct.scenesid       = cmd->buffer[XX_ALPHA_CLUSTER_SCENE_ID_POSITION];
        halCommonSetIndexedToken( TOKEN_XX_SCENE, cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION], &scene_struct);
    }
}

void xxAlphaClusterConfigureSdCmdHandle( EmberAfClusterCommand* cmd )
{
    if( memcmp( &cmd->buffer[XX_ALPHA_CLUSTER_MAC_ID_POSITION], xx_alpha_cluster_mac_all_ff, XX_ALPHA_CLUSTER_MAC_ID_LEN ) ) //0~2 is zcl head
    {
        tokType_xx_scene_t scene_struct;
        memset(&scene_struct.triggerIndex, 0xff, sizeof(scene_struct) );
        scene_struct.triggerMode       = XX_ALPHA_CLUSTER_TRIGGER_UNICAST_MODE;
        scene_struct.triggerIndex      = cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION];
        memmove(scene_struct.destMacId, &cmd->buffer[XX_ALPHA_CLUSTER_MAC_ID_POSITION], XX_ALPHA_CLUSTER_MAC_ID_LEN );
        scene_struct.destEnpiont       = cmd->buffer[XX_ALPHA_CLUSTER_MAC_ID_POSITION + XX_ALPHA_CLUSTER_MAC_ID_LEN];
        scene_struct.clusterId         = (uint16_t)( cmd->buffer[XX_ALPHA_CLUSTER_ID_H_POSITION]<<8 | cmd->buffer[XX_ALPHA_CLUSTER_ID_L_POSITION] );
        scene_struct.zclCmd            = cmd->buffer[XX_ALPHA_CLUSTER_CMD_POSITION];
        //xx_scene_struct.destShortId = 0xffff;
        halCommonSetIndexedToken( TOKEN_XX_SCENE, cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION], &scene_struct);

        halCommonGetIndexedToken(&scene_struct, TOKEN_XX_SCENE, cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION]);
        emberAfCorePrintln("xx_scene_struct.triggerIndex   =%x ",scene_struct.triggerIndex);
        emberAfCorePrintln("xx_scene_struct.scenesid       =%x ",scene_struct.scenesid);
        emberAfCorePrintln("xx_scene_struct.groupid        =%2x ",scene_struct.groupid);

        emberAfCorePrintln("xx_scene_struct.triggerMode    =%x ",scene_struct.triggerMode);
        emberAfCorePrintln("xx_scene_struct.destEnpiont    =%x ",scene_struct.destEnpiont);
        emberAfCorePrintln("xx_scene_struct.zclCmd         =%x ",scene_struct.zclCmd);
        emberAfCorePrintln("xx_scene_struct.clusterId      =%2x ",scene_struct.clusterId);
        emberAfCorePrintln("xx_scene_struct.destShortId    =%2x ",scene_struct.destShortId);
        emberAfCorePrintln("xx_scene_struct.destMacId      = ");
        for( uint8_t j = 0; j < 8; j++)
            emberAfCorePrint("%x ",scene_struct.destMacId[j]);

        emberAfCorePrintln("");

        emberAfFindNodeId(&scene_struct.destMacId[0],emAfCliServiceDiscoveryCallback);
        //emberAfSendImmediateDefaultResponse(status);
    }
}

void xxAlphaClusterConfigureGroupCmdHandle( EmberAfClusterCommand* cmd )
{
    if( (uint16_t)( cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_L_POSITION] | cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_H_POSITION]<<8 ) != 0xffff ) //0~2 is zcl head
    {
        tokType_xx_scene_t scene_struct;
        scene_struct.triggerMode    = XX_ALPHA_CLUSTER_TRIGGER_GROUP_MODE;
        scene_struct.triggerIndex   = cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION];
        scene_struct.groupid        = (uint16_t)( cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_L_POSITION] | cmd->buffer[XX_ALPHA_CLUSTER_GROUP_ID_H_POSITION]<<8 );
        scene_struct.clusterId      = (uint16_t)( cmd->buffer[XX_ALPHA_CLUSTER_GROUP_CLUSTER_ID_L_POSITION] | cmd->buffer[XX_ALPHA_CLUSTER_GROUP_CLUSTER_ID_H_POSITION]<<8 );
        scene_struct.zclCmd            = cmd->buffer[XX_ALPHA_CLUSTER_GROUP_CLUSTER_CMD_POSITION];
        halCommonSetIndexedToken( TOKEN_XX_SCENE, cmd->buffer[XX_ALPHA_CLUSTER_INDEX_POSITION], &scene_struct);
        //emberAfSendImmediateDefaultResponse(status);
    }
}







/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


