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

#ifdef _XX_ZCL_CMD_H
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
#define xxZclCmdPrint                                        emberAfAppPrint
#define xxZclCmdPrintln                                      emberAfAppPrintln

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

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
static EmberAfStatus status(bool wasHandled, bool clusterExists, bool mfgSpecific)
{
    if (wasHandled) {
        return EMBER_ZCL_STATUS_SUCCESS;
    } else if (clusterExists) {
        return EMBER_ZCL_STATUS_UNSUP_COMMAND;
    } else {
        return EMBER_ZCL_STATUS_UNSUPPORTED_CLUSTER;
    }
}

EmberAfStatus xxZclCmdOnOffClusterServerCommandparse( EmberAfClusterCommand *cmd )
{
    bool wasHandled = false;
    if (!cmd->mfgSpecific) 
    {
        switch (cmd->commandId) 
        {
            case ZCL_OFF_COMMAND_ID:
                {
                    // Command is fixed length: 0
                    //wasHandled = xxOnOffClusterOffCallbak( cmd->apsFrame->destinationEndpoint, cmd );
                }
                break;
            
            case ZCL_ON_COMMAND_ID:
                {
                    // Command is fixed length: 0
                    //wasHandled = xxOnOffClusterOnCallbak( cmd->apsFrame->destinationEndpoint, cmd );
                }
                break;
            
            case ZCL_TOGGLE_COMMAND_ID:
                {
                    // Command is fixed length: 0
                    //wasHandled = xxOnOffClusterToggleCallbak( cmd->apsFrame->destinationEndpoint, cmd );
                }
                break;
            
            default:
                {
                    // Unrecognized command ID, error status will apply.
                }
                break;
        }
//led status is negate of light
        #if defined( XX_LIGHT_ONE )
        {
            xxZclCmdPrintln("xx this is XX_LIGHT_ONE device ");
            xxOnOffLedStatusByLightStatus( halGpioRead(XX_HAL_LED_2_PIN), XX_ON_OFF_FIRST_LIGHT );
        }
        #elif defined( XX_LIGHT_TWO )
        {
            xxZclCmdPrintln("xx this is XX_LIGHT_TWO device ");
            xxOnOffLedStatusByLightStatus( halGpioRead(XX_HAL_LED_2_PIN), XX_ON_OFF_FIRST_LIGHT );
        }
        #elif defined( XX_LIGHT_THREE )
        {
            xxZclCmdPrintln("xx this is XX_LIGHT_THREE device ");
            xxOnOffLedStatusByLightStatus( halGpioRead(XX_HAL_LED_2_PIN), XX_ON_OFF_FIRST_LIGHT );
        }
        #endif
        
    }
    
    return status(wasHandled, true, cmd->mfgSpecific);
}


bool xxZclCmdScenesClusterServerCommandparse( EmberAfClusterCommand *cmd )
{
    if (!cmd->mfgSpecific) 
    {
        switch (cmd->commandId) 
        {
            case ZCL_RECALL_SCENE_COMMAND_ID:
                {
                    // Command is fixed length: 0
                    //return ( xxScenesClusterRecallCommandCallBack( cmd ) );
                }
                break;
            
            default:
                {
                    // Unrecognized command ID, error status will apply.
                }
                break;
        }
    }
    
    return false;
}

void xxZclCmdotaClusterCommandparse( EmberAfClusterCommand *cmd )
{
    xxZclCmdPrintln(" cmd->apsFrame->clusterId = %2x cmd->commandId : 0x%X",cmd->apsFrame->clusterId, cmd->commandId);
    switch(cmd->commandId)
    {
        case 0xf0://off
            {
                xxZclCmdPrintln("ota off");
            #if defined(XX_OTA_MECHANISM)
                Xx_whether_ota = false;           
            #endif
            }
            break;

        case 0xf1://on
            {
                xxZclCmdPrintln("ota on");
            #if defined(XX_OTA_MECHANISM)
                Xx_whether_ota = true;
                xxOtaNowFunction();
            #endif
            }
            break;

        case 0x07://on
            {
                xxZclCmdPrintln("ota right now");
            #if defined(XX_OTA_MECHANISM)
                Xx_whether_ota = true;
                xxOtaNowFunction();
            #endif
            }
            break;
    }
}

void xxZclCmdAlphaClusterCommandparse( EmberAfClusterCommand *cmd )
{
    switch (cmd->commandId )
    {
        case XX_ALPHA_CLUSTER_REMOVE_SD_COMMAND_ID:
            {
                xxAlphaClusterRemoveSdCmdHandle( cmd );
            }
            break;

        case XX_ALPHA_CLUSTER_INITIATE_ASSKE_COMMAND_ID:
            {
                xxAlphaClusterInitiateAsskeCmdHandle( cmd );
            }
            break;

        case XX_ALPHA_CLUSTER_CONFIGURE_SD_COMMAND_ID:
            {
                xxAlphaClusterConfigureSdCmdHandle( cmd );
            }
            break;

        case XX_ALPHA_CLUSTER_CONFIGURE_GROUP_COMMAND_ID:
            {
                xxAlphaClusterConfigureGroupCmdHandle( cmd );
            }
            break;
        
        
        default:
            {
            }
            break;
    }
}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


