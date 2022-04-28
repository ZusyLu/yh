/**************************************************************************************************
  Filename:       xx_example.c
  Revised:        $Date: 2021-9-15 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.1.0(2 stacks)

  Copyright 2021 uascent. All rights reserved.
                                                  .
***************************************************************************************************/
#include "xxapp/xx_main.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _XX_ZCL_PARSE_H
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
#define xxRfZclParsePrint                                     emberAfAppPrint
#define xxRfZclParsePrintln                                   emberAfAppPrintln

#define xxRfZclParsePrintBuffer                               emberAfAppPrintBuffer


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
//uint32_t gpioNumber[GPIO_TEST_NUMBER]={PORTA_PIN(0),PORTA_PIN(3),PORTA_PIN(4),\
//                                         PORTB_PIN(0),PORTB_PIN(1),\
//                                         PORTC_PIN(0),PORTC_PIN(1),PORTC_PIN(2),\
//                                         PORTD_PIN(0),PORTD_PIN(1)};



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
static EmberAfInterpanHeader interpanResponseHeader;

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

static void prepareForResponse(const EmberAfClusterCommand *cmd)
{
  emberAfResponseApsFrame.profileId           = cmd->apsFrame->profileId;
  emberAfResponseApsFrame.clusterId           = cmd->apsFrame->clusterId;
  emberAfResponseApsFrame.sourceEndpoint      = cmd->apsFrame->destinationEndpoint;
  emberAfResponseApsFrame.destinationEndpoint = cmd->apsFrame->sourceEndpoint;

  // Use the default APS options for the response, but also use encryption and
  // retries if the incoming message used them.  The rationale is that the
  // sender of the request cares about some aspects of the delivery, so we as
  // the receiver should make equal effort for the response.
  emberAfResponseApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
  if ((cmd->apsFrame->options & EMBER_APS_OPTION_ENCRYPTION) != 0U) {
    emberAfResponseApsFrame.options |= EMBER_APS_OPTION_ENCRYPTION;
  }
  if ((cmd->apsFrame->options & EMBER_APS_OPTION_RETRY) != 0U) {
    emberAfResponseApsFrame.options |= EMBER_APS_OPTION_RETRY;
  }

  if (cmd->interPanHeader == NULL) {
    emberAfResponseDestination = cmd->source;
    emberAfResponseType &= ~ZCL_UTIL_RESP_INTERPAN;
  } else {
    emberAfResponseType |= ZCL_UTIL_RESP_INTERPAN;
    MEMMOVE(&interpanResponseHeader,
            cmd->interPanHeader,
            sizeof(EmberAfInterpanHeader));
    // Always send responses as unicast
    interpanResponseHeader.messageType = EMBER_AF_INTER_PAN_UNICAST;
  }
}


static bool xxRfZclParseRxZclGlobalCommand( EmberAfClusterCommand *cmd )
{
    xxRfZclParsePrintln("xx this is global cmd ");
    return false;
}

static bool xxRfZclParseRxZclSpecificCommand( EmberAfClusterCommand *cmd )
{
    //EmberAfStatus status;

    // if we are disabled then we can only respond to read or write commands
    // or identify cluster (see device enabled attr of basic cluster)
    #if 0
    if (!emberAfIsDeviceEnabled(cmd->apsFrame->destinationEndpoint)
        && cmd->apsFrame->clusterId != ZCL_IDENTIFY_CLUSTER_ID) {
        xxRfZclParsePrintln("%pd, dropping ep 0x%x clus 0x%2x cmd 0x%x",
            "disable",
            cmd->apsFrame->destinationEndpoint,
            cmd->apsFrame->clusterId,
            cmd->commandId);
            emberAfSendDefaultResponse(cmd, EMBER_ZCL_STATUS_FAILURE);
            //return true;
            return false;
    }
#endif

      // Pass the command to the generated command parser for processing
    EmberAfStatus result = status(false, false, cmd->mfgSpecific);
#if 0
    if ( cmd->direction == (uint8_t)ZCL_DIRECTION_CLIENT_TO_SERVER \
         && emberAfContainsServerWithMfgCode(cmd->apsFrame->destinationEndpoint,\
         cmd->apsFrame->clusterId,\
         cmd->mfgCode) )
#endif
    
    {
        xxRfZclParsePrintln("xx rf zcl parese rx zcl cmd message");
        switch ( cmd->apsFrame->clusterId )
        {
            case ZCL_ON_OFF_CLUSTER_ID:
                {
                    result = xxZclCmdOnOffClusterServerCommandparse( cmd );
                    if (result != EMBER_ZCL_STATUS_SUCCESS) {
                        emberAfSendDefaultResponse(cmd, result);
                    }
                }
                return true;    //break;
                
            case ZCL_SCENES_CLUSTER_ID:
                {
                    return ( xxZclCmdScenesClusterServerCommandparse( cmd ) );               
                }
                break;

            case ZCL_OTA_BOOTLOAD_CLUSTER_ID:
                {
                    xxRfZclParsePrintln("xx ota operate");
                    xxZclCmdotaClusterCommandparse( cmd );
                }
                break;
#ifdef XX_ALPHA_CLUSTER_ID
            case XX_ALPHA_CLUSTER_ID:
                {
                    xxZclCmdAlphaClusterCommandparse( cmd );
                    return true;
                }
                break;
#endif
            
            default:
                   break;
        }
    }
    
    return false;
}

static bool xxRfZclParseRxZclMessage( EmberAfClusterCommand *cmd )
{
    uint8 index = emberAfIndexFromEndpoint(cmd->apsFrame->destinationEndpoint);
    if ( index == 0xFF )
    {
        xxRfZclParsePrintln(" xx invalid endpoiont ");
        return false;
    }
    else if ( emberAfNetworkIndexFromEndpointIndex(index) != cmd->networkIndex )
    {
        xxRfZclParsePrintln(" xx invalid network ");
        return false;
    }
    else if (emberAfProfileIdFromIndex(index) != cmd->apsFrame->profileId
             && (cmd->apsFrame->profileId != EMBER_WILDCARD_PROFILE_ID
                 || (EMBER_MAXIMUM_STANDARD_PROFILE_ID
                     < emberAfProfileIdFromIndex(index))))
    {
        xxRfZclParsePrintln(" xx invalid profiled id ");
        return false;
    }
    else if ((cmd->type == EMBER_INCOMING_MULTICAST
              || cmd->type == EMBER_INCOMING_MULTICAST_LOOPBACK)
              && !emberAfGroupsClusterEndpointInGroupCallback(cmd->apsFrame->destinationEndpoint,
                                                             cmd->apsFrame->groupId)) 
    {
        xxRfZclParsePrintln(" xx invalid multicast ");
        return false;
    }
    else 
    {
        xxRfZclParsePrintln("xx cmd->clusterSpecific = %x ",cmd->clusterSpecific);
        return ( cmd->clusterSpecific ? xxRfZclParseRxZclSpecificCommand( cmd )
                                      : xxRfZclParseRxZclGlobalCommand( cmd ) );
    }
                                                             
}


bool emberAfPreCommandReceivedCallback( EmberAfClusterCommand* cmd )
{
    bool xxStatus;
    xxRfZclParsePrintln("xx cmd->apsFrame->destinationEndpoint = %x ",cmd->apsFrame->destinationEndpoint);

    if (cmd->apsFrame->destinationEndpoint == EMBER_BROADCAST_ENDPOINT) {
        uint8_t i;
        for (i = 0; i < emberAfEndpointCount(); i++) {
            uint8_t endpoint = emberAfEndpointFromIndex(i);
#if 0
            if (!emberAfEndpointIndexIsEnabled(i)
                || !emberAfContainsClusterWithMfgCode(endpoint, cmd->apsFrame->clusterId, cmd->mfgCode)) {
                continue;
            }
#endif
            // Since the APS frame is cleared after each sending,
            // we must reinitialize it.  It is cleared to prevent
            // data from leaking out and being sent inadvertently.
            prepareForResponse(cmd);

            // Change the destination endpoint of the incoming command and the source
            // source endpoint of the response so they both reflect the endpoint the
            // message is actually being passed to in this iteration of the loop.
            cmd->apsFrame->destinationEndpoint      = endpoint;
            emberAfResponseApsFrame.sourceEndpoint = endpoint;
            xxStatus = xxRfZclParseRxZclMessage(cmd);
            if ( xxStatus )
            {
                xxRfZclParsePrintln("RX Only xx processing");
            }
            else
            {
                xxRfZclParsePrintln("RX not xx processing");
            }
            
        }
        return xxStatus;
    } 
    else 
    {
        xxStatus = xxRfZclParseRxZclMessage(cmd);
        if ( xxStatus )
        {
            xxRfZclParsePrintln("RX Only xx processing");
        }
        else
        {
            xxRfZclParsePrintln("RX not xx processing");
        }
        return xxStatus;
    }
}



/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


