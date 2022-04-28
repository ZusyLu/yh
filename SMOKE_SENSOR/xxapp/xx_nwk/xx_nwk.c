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

#ifdef _XX_NWK_H
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
#define xxNwkPrint                                   emberAfAppPrint
#define xxNwkPrintln                                 emberAfAppPrintln

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/
#define XX_REJOIN_MIN_TIME											10  //FEN ZHONG 
#define XX_REJOIN_MAX_TIME											120 

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
static uint16_t xxRejoinTime = XX_REJOIN_MIN_TIME;

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/
uint8 XxNwkSendRfPacket(uint8 srcEp, uint8 dstEp, uint16 dstAddr, uint8 frameControl, uint16 clusterID, uint8 command, uint8 *buf, uint8 len)
{
    
    EmberApsFrame *apsFrame = emberAfGetCommandApsFrame();
    apsFrame->profileId           = emberAfPrimaryProfileId();
    apsFrame->sourceEndpoint      = srcEp;
    apsFrame->destinationEndpoint = dstEp;

    if (frameControl&ZCL_MANUFACTURER_SPECIFIC_MASK)
    {
        emberAfFillExternalManufacturerSpecificBuffer(frameControl,
                                                      clusterID,
                                                      0x1002, 
                                                      command, 
                                                      "b", 
                                                      buf,
                                                      len);
    }
    else
    {
        emberAfFillExternalBuffer(frameControl,
                                  clusterID,
                                  command,
                                  "b", 
                                  buf,
                                  len);
    }


    return emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

// sendMethod = 0 is Unicast; = 1 is Broadcast; = 3 is Multicast
uint8_t XxNwkMultipleModeSendRfPacket(uint8_t srcEp, uint8_t dstEp, uint16_t dstAddr, uint8_t sendMethod, uint8_t frameControl, uint16_t clusterID, uint8_t command, uint8_t *buf, uint8_t len)
{

    EmberApsFrame *apsFrame = emberAfGetCommandApsFrame();
    apsFrame->profileId           = emberAfPrimaryProfileId();
    apsFrame->sourceEndpoint      = srcEp;
    apsFrame->destinationEndpoint = dstEp;

    if (frameControl&ZCL_MANUFACTURER_SPECIFIC_MASK)
    {
        emberAfFillExternalManufacturerSpecificBuffer(frameControl,
                                                      clusterID,
                                                      0x1002,
                                                      command,
                                                      "b",
                                                      buf,
                                                      len);
    }
    else
    {
        emberAfFillExternalBuffer(frameControl,
                                  clusterID,
                                  command,
                                  "b",
                                  buf,
                                  len);
    }

    if( sendMethod == XX_NWK_UNICASE_SEND_METHOD)
    {
        return emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
    }
    else if(sendMethod == XX_NWK_BROADCAST_SEND_METHOD)
    {
        return emberAfSendCommandBroadcast(0xffff);//dstAddr = 0xffff
    }
    else if(sendMethod == XX_NWK_MULTICAST_SEND_METHOD)
    {
        return emberAfSendCommandMulticast(dstAddr);//dstAddr = group id
    }
}

void XxZbStackStatusCallback(EmberStatus status)
{
    switch(status)
    {        
        EmberNetworkStatus state;
        state = emberAfNetworkState();
        case EMBER_NETWORK_UP:
        {
             xxNwkPrintln("xx EMBER_NETWORK_UP ");
             
             #ifdef XX_HEART_BEAT_ACTIVE_CALL_BACK
                XX_HEART_BEAT_ACTIVE_CALL_BACK
             #endif

			 #ifdef XX_SERIAL_SMOKE_NWK_UP
				uint8 value;
				value=1;
				xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SCAN_NETWORK_RESPONSE_CMD,&value );
			 #endif
				 
             emberEventControlSetInactive( xx_project_scan_network_event );
            Xx_project_scan_network_cunt = XX_PROJECT_SCAN_NETWORK_MAX_NUMBER;
            emberAfAddToCurrentAppTasks(EMBER_AF_FORCE_SHORT_POLL);
            emberEventControlSetDelayMS( xx_project_wait_cfg_cmd_event, XX_PROJECT_SHROT_POLL_TIME );
			xxRejoinTime = XX_REJOIN_MIN_TIME;
        }
        break;

        case EMBER_NETWORK_DOWN:
        {
            #ifdef XX_HEART_BEAT_INACTIVE_CALL_BACK
                XX_HEART_BEAT_INACTIVE_CALL_BACK
            #endif
			
		    #ifdef XX_SERIAL_SMOKE_NWK_UP
          uint8 value;
          value=3;
				 #endif
			
            if ( state == EMBER_NO_NETWORK )
            {
                 xxNwkPrintln("xx EMBER_NETWORK_DOWN ");
				 xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SCAN_NETWORK_RESPONSE_CMD,&value );
                 XxLeaveNetworkFuction();
            }
            else if ( state == EMBER_JOINED_NETWORK_NO_PARENT )
            {
				xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_CMD,&value );
                //emberEventControlSetDelayMS( xx_project_wait_cfg_cmd_event, XX_PROJECT_TIME_MS(100) );
                //emberAfStartMoveCallback();
            }

        }
        break;
    }
}

uint8_t XxReportSpecificAttributeEx(uint16_t   u16ClusterID,  uint8_t  u8AttrNums,   uint16_t   *u16AttrID,\
                                    uint8_t u8SrcEndPoint,    uint8_t  u8DestEndPoint,    uint16_t   ManufacturerCode)

{

    uint8_t Data[50];
    uint8_t ZCL_Data[90];
    uint16_t ZclBufferLen = 0;
    EmberStatus status = EMBER_SUCCESS;
    globalApsFrame.clusterId = u16ClusterID;
    globalApsFrame.destinationEndpoint = u8DestEndPoint;
    globalApsFrame.sourceEndpoint = u8SrcEndPoint;
    globalApsFrame.profileId = 0x0104;//HA
    uint16_t mfgSpecificId = ManufacturerCode;
    uint8_t disableDefaultResponse = 1;
    EmberAfAttributeType type;
    uint8_t index = 0;
    uint8_t i;

//zcl common header
    ZCL_Data[index++] = (ZCL_GLOBAL_COMMAND
                         | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                         | (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE
                            ? ZCL_MANUFACTURER_SPECIFIC_MASK
                            : 0)
                         | (disableDefaultResponse
                            ? ZCL_DISABLE_DEFAULT_RESPONSE_MASK
                            : 0));

    if (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE)

    {
        ZCL_Data[index++] = (uint8_t)mfgSpecificId;
        ZCL_Data[index++] = (uint8_t)(mfgSpecificId >> 8);
    }

    ZCL_Data[index++] = emberAfNextSequence();
    ZCL_Data[index++] = ZCL_REPORT_ATTRIBUTES_COMMAND_ID;
    ZclBufferLen = index;

    for( i=0; i<u8AttrNums; i++ )
    {
        status =    emAfReadAttribute(u8SrcEndPoint,
                                      (EmberAfClusterId)u16ClusterID,
                                      (EmberAfAttributeId)u16AttrID[i],
                                      CLUSTER_MASK_SERVER,
                                      mfgSpecificId, //emAfReadAttribute emberAfReadAttribute
                                      Data,
                                      sizeof(Data),
                                      &type);
        if (status != EMBER_ZCL_STATUS_SUCCESS)

        {
            xxNwkPrintln("ERR:APP_eUserReportSpecificAttributeEx %x", status);
            return  status;
        }

        ZCL_Data[ZclBufferLen] = LOW_BYTE(u16AttrID[i]);
        ZclBufferLen += 1;
        ZCL_Data[ZclBufferLen] = HIGH_BYTE(u16AttrID[i]);
        ZclBufferLen +=1;
        ZCL_Data[ZclBufferLen] = type;
        ZclBufferLen +=1;
        uint16_t  size = (emberAfIsThisDataTypeAStringType(type)
                          ? emberAfStringLength(Data) + 1
                          : emberAfGetDataSize(type));
        MEMMOVE( ZCL_Data+ZclBufferLen, Data, size);
        ZclBufferLen += size;
        MEMSET(Data, 0, 50);
    }

    uint8_t mode ;
    mode = EMBER_OUTGOING_DIRECT;
    status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                0x0000,
                                &globalApsFrame,
                                ZclBufferLen,
                                ZCL_Data);

    xxNwkPrintln("Report>>%s u16ClusterID: %04x, SeqNum: %d, status: %d\n",__FUNCTION__, u16ClusterID,ZCL_Data[3], status);
    return status;
}

bool emberAfPluginEndDeviceSupportLostParentConnectivityCallback( void )
{
	uint8 value=0;
	if ( xxRejoinTime > XX_REJOIN_MAX_TIME )
	{
		xxRejoinTime = XX_REJOIN_MAX_TIME;
	}
	else
	{
		xxRejoinTime = xxRejoinTime<<1;
	}
	value = 3;
	xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_CMD,&value );
	emberEventControlSetDelayMinutes( xx_project_scan_network_event, xxRejoinTime );
	
	return false;
}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


