#include PLATFORM_HEADER
#include "app/framework/util/common.h"
#include "app/framework/include/af.h"
#include "module-cfg-tokens.h"
#include "crc16.h"
#include "serial-rx-task.h"

/*********************************history*************************************
 *
 * This function is called when input i to exit rf test
 * @param [in] none
 * @param [out] none
 * @history
 * Date         : 2018-08-20
 * Author       : Jin
 * Modification : Created function
*/
#define  SERIAL_RX_TIMEOUT    30
#define  SERIAL_BUFFER_LENGTH  100
//define this macro for customer protocol
#define  CUSTOM_PROTOCOL_TYPE
//Software version
#define SOFTWARE_VERSION                                                    0X03
// protocol define
/*-----------------------------------------------
|���ֽ�Э���ʶ��   |   һ�ֽ�	 | N�ֽ�
-------------------------------------------------
|    A5 AA 55	 |    ������	 | ����
-------------------------------------------------*/
#define  PROTOCAL_SERIAL_PORT          0x22
#define  PROTOCAL_BUFFER_MIN_LENGTH    4
#define  PROTOCAL_CMD_INDEX            3
#define  PROTOCAL_PAYLOAD_START_INDEX  4
#define  PROTOCAL_FRAME_ID1          0xA5
#define  PROTOCAL_FRAME_ID2          0xAA
#define  PROTOCAL_FRAME_ID3          0x5A
#define  PROTOCAL_CMD_NODE_TYPE      0x00
#define  PROTOCAL_CMD_CHANNEL        0x01
#define  PROTOCAL_CMD_POWER          0x02
#define  PROTOCAL_CMD_SHORT_ADDR     0x03
#define  PROTOCAL_CMD_PANID          0x04
#define  PROTOCAL_CMD_EPANID         0x05
#define  PROTOCAL_CMD_NWK_KEY        0x06
#define  PROTOCAL_CMD_ALL_SET        0x07
#define  PROTOCAL_CMD_PENETRATE      0x08
#define  PROTOCAL_CMD_REBOOT         0x11
#define  PROTOCAL_CMD_GET_ADDR       0x21
#define  PROTOCAL_CMD_READ_NETWORK   0x22
#define  PROTOCAL_CMD_READ_DATA      0x31
#define  PROTOCAL_CMD_WRITE_DATA     0x32

//Send mode
#define  RF_SEND_MODE_UNICASE     0x00
#define  RF_SEND_MODE_BROADCASE   0x01

extern void yhEmAfInterpanFragmentTestCommand(EmberPanId htPanId,uint8_t *htMacId , \
                                              uint16_t HTclusterId,uint8_t *data,uint16_t HTmessageLen);
void yhSendDataToDebugger(uint8_t* data, uint8_t len);
void yhSendDataToGW(uint8_t* data, uint8_t len,uint8_t cast);
extern yhPenetrateDataType yhPenetrateDataId;

typedef struct {
   int8u length;
   int8u buffer[SERIAL_BUFFER_LENGTH];
} SerialRxDateStruct;
typedef struct {
   int8u frameid1;
   int8u frameid2;
   int8u frameid3;
   int8u cmdCode;
   int8u ackData[40];
} SerialTxDateStruct;
typedef struct{
   int8u  deviceType;
   int8u  channel;
   int8u  radioTxPower;
   int16u shortAddress;
   int16u panId;
   int8u  extendPanId[8];
   int8u  nwkKey[16];
}DeviceCfgInfoStruct;
//
SerialRxDateStruct serialbuffer;
SerialTxDateStruct serialTxBuffer;
DeviceCfgInfoStruct deviceCfgInfo;
tokTypeModuleCfgStruct tokenModuleCfgInfo;
tokTypeModuleUserDataStruct userData;

//
#ifdef CUSTOM_PROTOCOL_TYPE
//customer protocal define
/*---------------------------protocal format define-----------------------------

HEAD	   LEN	      CMD0	   CMD1	   DATA	   FCS
1 Byte	  1 Byte	      1 Byte	  1 Byte		   1 Byte
֡ͷ, 7C	  ���ݳ���     Ԥ��	  ������	   ���ݶ�    У��

--------------------------------------------------------------------------------*/
#define  FULL_BUFFER_MIN_LENGTH                                         5
#define  CMD0_OFFSET                                                    2
#define  CMD1_OFFSET                                                    3
#define  DATA_OFFSET                                                    4

#define  RF_ADDRESS_OFFSET                                              10
//Define CMD1
//MCU-->ZIGBEE
#define CMD1_PUSH_CONTROL_POINT_LIST                                    0x04 //����ָ�����Ƶ�ĵ�ǰֵ()add by JIN
#define RF_CMD1_PUSH_CONTROL_POINT_LIST                                 0X80 //

#define CMD1_PUSH_CONTROL_POINT                                         0x05 //����ָ�����Ƶ�ĵ�ǰֵ
#define RF_CMD1_PUSH_CONTROL_POINT                                      0X83

#define CMD1_CTRL_CONTROL_POINT                                         0x06  // OLED��巢������ȥ�����������Ƶ� Penetrate
#define RF_CMD1_CTRL_CONTROL_POINT                                      0X84  //

#define CMD1_PENELRATE_SEND_DEBUG                                       0x08
#define CMD1_PENELRATE_UNICAST_GW                                       0x09
#define CMD1_PENELRATE_BROADCAST_GW                                     0x0a
#define CMD1_GET_SOFTWARE_VERSION                                       0X0b

#define CMD1_RECALL_SCENE                                               0x07  // OLED��巢�ͳ���ģʽ����
#define RF_CMD1_RECALL_SCENE                                            0X85  //
#define RF_CMD1_OTA_FINISH                                              0X88

#define  CMD1_DIRECT_TX                                                 0x40
#define  RF_CMD1_DIRECT_TX                                              0x40

#define  CMD1_MCU_ACK                                                   0x7F

//ZIGBEE-->MCU
#define  CMD1_QUERY_CONTROL_POINT_LIST                                  0x81 //��ȡ���Ƶ��б���Ϣ
#define  CMD1_QUERY_CONTROL_POINT                                       0x82 //��ȡ���Ƶ��ֵ
#define  CMD1_CONTROL_CONTROL_POINT                                     0x83 //�����·�ָ�����ĳ�����Ƶ��״̬
#define  CMD1_UPDATE_PANNEL                                             0x84 //ͬ��OLED�����Ƶ�״̬
#define  CMD1_UPDATE_PANNEL_SCENE                                       0x85 //ͬ��OLED����״̬
#define  CMD1_SOFTWARE_VERSION                                          0X87
#define  CMD1_PENETRATE_TO_GW                                           0X89
#define  CMD1_ZIGBEE_ACK                                                0xFF





//add by xie

#define LEWIN_DATA_OFFSET                                               8

#define LEWIN_FRAME_HEAD1                                               0xF0
#define LEWIN_FRAME_HEAD2                                               0x0F

#define YH_BUFF_MAX_LEN                                                 128
typedef struct{
   int8u  rfLength;
   int8u  header;
   int8u  length;
   int8u  cmd0;
   int8u  cmd1;
   int8u  data[80];
   int8u  fcs;//frame check-sum
}ProtocolDataStruct;

//add by xie
typedef struct{
   int8u  rfLength;
   int8u  header1;
   int8u  header2;
   int8u  version;
   int8u  cmdIDH;
   int8u  cmdIDL;
   int8u  funcCode;
   int8u  lengthH;
   int8u  lengthL;
   int8u  data[80];
   int8u  fcs;//frame crc16
}Lewin3ProtocolDataStruct;

//
int8u yhGlobalBuff[YH_BUFF_MAX_LEN];

Lewin3ProtocolDataStruct  lewinProtocolDataBuffer;
ProtocolDataStruct protocolDataBuffer;
ProtocolDataStruct protocolRxBuffer; //
void customDefineProtocalProcess(void);
#endif
//
EmberEventControl appRebootEventControl;
void halRebootEventHandler(void);
void serialCommandParse(void);

//Function
void appRebootEventHandler(void)
{
   emberEventControlSetInactive(appRebootEventControl);
   halReboot(); //
}
//ACK Function
void setStatusAck(uint8_t cmd,uint8_t status)
{
   serialTxBuffer.frameid1 = PROTOCAL_FRAME_ID1;
   serialTxBuffer.frameid2 = PROTOCAL_FRAME_ID2;
   serialTxBuffer.frameid3 = PROTOCAL_FRAME_ID3;
   serialTxBuffer.cmdCode  = cmd;
   serialTxBuffer.ackData[0] = status;
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&serialTxBuffer.frameid1,5);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
void getAddressAck(void)
{
   EmberNodeId nodeId;
   EmberEUI64 eui64;
   serialTxBuffer.frameid1 = PROTOCAL_FRAME_ID1;
   serialTxBuffer.frameid2 = PROTOCAL_FRAME_ID2;
   serialTxBuffer.frameid3 = PROTOCAL_FRAME_ID3;
   serialTxBuffer.cmdCode  = PROTOCAL_CMD_GET_ADDR;
   nodeId = emberGetNodeId();
   emberAfGetEui64(eui64);
   serialTxBuffer.ackData[0] = (nodeId >> 8) & 0x00ff;
   serialTxBuffer.ackData[1] = (nodeId & 0x00ff);
   serialTxBuffer.ackData[2] = eui64[7];
   serialTxBuffer.ackData[3] = eui64[6];
   serialTxBuffer.ackData[4] = eui64[5];
   serialTxBuffer.ackData[5] = eui64[4];
   serialTxBuffer.ackData[6] = eui64[3];
   serialTxBuffer.ackData[7] = eui64[2];
   serialTxBuffer.ackData[8] = eui64[1];
   serialTxBuffer.ackData[9] = eui64[0];
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&serialTxBuffer.frameid1,14);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}

void readNetworkInfoAck(void)
{
   serialTxBuffer.frameid1 = PROTOCAL_FRAME_ID1;
   serialTxBuffer.frameid2 = PROTOCAL_FRAME_ID2;
   serialTxBuffer.frameid3 = PROTOCAL_FRAME_ID3;
   serialTxBuffer.cmdCode  = PROTOCAL_CMD_READ_NETWORK;
   halCommonGetToken(&tokenModuleCfgInfo.deviceType,TOKEN_MODULE_CFG_INFO);
   MEMMOVE(&serialTxBuffer.ackData[0],&tokenModuleCfgInfo.deviceType,3);
   //Addr and panid
   serialTxBuffer.ackData[3] = (tokenModuleCfgInfo.shortAddress >> 8) & 0x00ff;
   serialTxBuffer.ackData[4] = (tokenModuleCfgInfo.shortAddress & 0x00ff);
    serialTxBuffer.ackData[5] = (tokenModuleCfgInfo.panId >> 8) & 0x00ff;
   serialTxBuffer.ackData[6] = (tokenModuleCfgInfo.panId & 0x00ff);
   MEMMOVE(&serialTxBuffer.ackData[7],&tokenModuleCfgInfo.extendPanId,8);
   MEMMOVE(&serialTxBuffer.ackData[15],&tokenModuleCfgInfo.nwkKey,16);
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&serialTxBuffer.frameid1,35);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}

void readUserDataAck(void)
{
   serialTxBuffer.frameid1 = PROTOCAL_FRAME_ID1;
   serialTxBuffer.frameid2 = PROTOCAL_FRAME_ID2;
   serialTxBuffer.frameid3 = PROTOCAL_FRAME_ID3;
   serialTxBuffer.cmdCode  = PROTOCAL_CMD_READ_DATA;
   halCommonGetToken(&userData,TOKEN_MODULE_USER_DATA);
   MEMMOVE(serialTxBuffer.ackData,userData.usData, 16);
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&serialTxBuffer.frameid1,20);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
void  rfCommandSetup(uint16_t dstAddr,uint8_t sendMode,
                     uint8_t *buffer,uint8_t bufferlen)
{

    if(bufferlen > 80) return;
#if 1
      emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER\
                                 | ZCL_DISABLE_DEFAULT_RESPONSE_MASK), \
                                 0xfc00, \
                                 0x00, \
                                 "s",
                                 buffer,
                                 bufferlen);
#else
      emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                                 | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER), \
                                 0xfc00, \
                                 0x00, \
                                 "s",
                                 buffer,
                                 bufferlen);
#endif
    emberAfSetCommandEndpoints(1,1);
    if(sendMode == RF_SEND_MODE_UNICASE)
    {
        emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,dstAddr);
    }
    else if(sendMode == RF_SEND_MODE_BROADCASE)
    {
        emberAfSendCommandBroadcast(0xffff);
    }
}
/**
 * @brief clearSerialBuffer
 *
 * @param [in]  none
 * @param [out] none
 *
 * @return
 * @history
 *   Date          : 2018-08-20
 *   Author       : JIN
 *   Modification : Created function
 */
void clearSerialBuffer(void)
{
   serialbuffer.length = 0;
   serialbuffer.buffer[0] = 0xff;
   serialbuffer.buffer[1] = 0xff;
   serialbuffer.buffer[2] = 0xff;
}
/**
 * @brief serialRxProcess
 *
 * @param [in] int8u Port
 * @param [out] none
 *
 * @return
 *
 * @history
 *   Date          : 2018-08-20
 *   Author       : JIN
 *   Modification : Created function
 */
void serialRxProcess(int8u Port)
{
      static int16u lastTime = 0;
      int16u time;
      int8u ch = 0;
      time = halCommonGetInt16uMillisecondTick();
      if (elapsedTimeInt16u(lastTime, time) > SERIAL_RX_TIMEOUT)
      {
           lastTime = time;
           serialCommandParse();

      }
      else if(emberSerialReadByte(Port, &ch) == EMBER_SUCCESS)
      {
           lastTime = time;
           serialbuffer.buffer[serialbuffer.length] = ch;
           serialbuffer.length ++;
      }

}
void rfDataTransmite(void)
{
    int16u dstAddr;
    int8u  sendMode;
    if(serialbuffer.length < PROTOCAL_BUFFER_MIN_LENGTH)
    {
       serialbuffer.length = 0;
       return;
    }
    else
    {
       dstAddr =  (serialbuffer.buffer[serialbuffer.length-3] << 8) +  serialbuffer.buffer[serialbuffer.length-2] ;
       sendMode = serialbuffer.buffer[serialbuffer.length-1];
       serialbuffer.length = serialbuffer.length -3;//addr and mode byte is not the payload
       rfCommandSetup(dstAddr,sendMode, (int8u *)&(serialbuffer.length),serialbuffer.length + 1);
    }
}
//end
void serialCommandParse(void)
{
    uint8_t  payloadLength;
    uint8_t ii=0;
    if(serialbuffer.length != 0){
        emberAfCorePrintln("serialbuffer.length = %d , serialbuffer.buffer = ",serialbuffer.length);
        for(ii=0;ii<serialbuffer.length;ii++){
            emberAfCorePrint("%x",serialbuffer.buffer[ii]);

        }
        emberAfCorePrintln("");
    }

    if(serialbuffer.length < PROTOCAL_BUFFER_MIN_LENGTH)
    {
       serialbuffer.length = 0;
       serialbuffer.buffer[0] = 0xff;
       serialbuffer.buffer[1] = 0xff;
       serialbuffer.buffer[2] = 0xff;
       return;
    }
    else if((serialbuffer.buffer[0] == PROTOCAL_FRAME_ID1)
            &&(serialbuffer.buffer[1] == PROTOCAL_FRAME_ID2)
            &&(serialbuffer.buffer[2] == PROTOCAL_FRAME_ID3))
    {
        payloadLength = serialbuffer.length - PROTOCAL_BUFFER_MIN_LENGTH;
        switch(serialbuffer.buffer[PROTOCAL_CMD_INDEX])
        {

             case  PROTOCAL_CMD_NODE_TYPE:
                   if(payloadLength == 1)
                   {
                      tokenModuleCfgInfo.deviceType = serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX];
                      setStatusAck(PROTOCAL_CMD_NODE_TYPE,0x00);
                   }
                   else
                   {
                       setStatusAck(PROTOCAL_CMD_NODE_TYPE,0x01);
                   }
                   break;
             case  PROTOCAL_CMD_CHANNEL :
                   if(payloadLength == 1)
                   {
                       tokenModuleCfgInfo.channel = serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX];
                       setStatusAck(PROTOCAL_CMD_CHANNEL,0x00) ;
                   }
                   else
                   {
                       setStatusAck(PROTOCAL_CMD_CHANNEL,0x01) ;
                   }
                  break;
             case  PROTOCAL_CMD_POWER:
                   if(payloadLength == 1)
                   {
                      if(serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX] >= 0x14)
                      {
                         setStatusAck(PROTOCAL_CMD_POWER,0x01);
                      }
                      else
                      {
                         tokenModuleCfgInfo.radioTxPower = serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX];
                         setStatusAck(PROTOCAL_CMD_POWER,0x00);
                      }
                   }
                   else
                   {
                      setStatusAck(PROTOCAL_CMD_POWER,0x01);
                   }
                  break;
             case  PROTOCAL_CMD_SHORT_ADDR:
                   if(payloadLength == 2)
                   {
                      tokenModuleCfgInfo.shortAddress = (serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX] << 8)
                        + serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX + 1];
                      halCommonSetToken(TOKEN_MODULE_CFG_INFO,&tokenModuleCfgInfo);
                      setStatusAck(PROTOCAL_CMD_SHORT_ADDR,0x00);

                   }
                   else
                   {
                       setStatusAck(PROTOCAL_CMD_SHORT_ADDR,0x01);
                   }
                  break;
             case  PROTOCAL_CMD_PANID :
                  if(payloadLength == 2)
                   {
                      tokenModuleCfgInfo.panId = (serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX] << 8)
                        + serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX + 1];
                       setStatusAck(PROTOCAL_CMD_PANID,0x00);
                   }
                   else
                   {
                        setStatusAck(PROTOCAL_CMD_PANID,0x01);
                   }
                  break;
             case  PROTOCAL_CMD_EPANID:
                   if(payloadLength == 8)
                   {
                       MEMMOVE(tokenModuleCfgInfo.extendPanId,
                              &serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX], 8);
                        setStatusAck(PROTOCAL_CMD_EPANID,0x00);

                   }
                   else
                   {
                        setStatusAck(PROTOCAL_CMD_EPANID,0x01);
                   }
                  break;
             case  PROTOCAL_CMD_NWK_KEY :
                   if(payloadLength == 16)
                   {
                       MEMMOVE(tokenModuleCfgInfo.nwkKey,
                              &serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX], 16);
                        setStatusAck(PROTOCAL_CMD_NWK_KEY,0x00);

                   }
                   else
                   {
                         setStatusAck(PROTOCAL_CMD_NWK_KEY,0x01);
                   }
                  break;
             case  PROTOCAL_CMD_ALL_SET:
                    if(payloadLength == 31)
                   {
                      MEMMOVE(&tokenModuleCfgInfo.deviceType,
                              &serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX], 3);
                      tokenModuleCfgInfo.shortAddress = (serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX+3] << 8)
                        + serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX+4];
                      tokenModuleCfgInfo.panId =(serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX+5] << 8)
                        + serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX+6];
                       MEMMOVE(&tokenModuleCfgInfo.extendPanId[0],
                              &serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX+7], 8);
                       MEMMOVE(&tokenModuleCfgInfo.nwkKey[0],
                              &serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX+15], 16);
                       setStatusAck(PROTOCAL_CMD_ALL_SET,0x00);

                   }
                   else
                   {
                        setStatusAck(PROTOCAL_CMD_ALL_SET,0x01);
                   }
                  break;
             case PROTOCAL_CMD_PENETRATE:

               break;
             case PROTOCAL_CMD_REBOOT :
                    if(payloadLength == 0)
                    {
                       halCommonSetToken(TOKEN_MODULE_CFG_INFO,&tokenModuleCfgInfo);
                       emberEventControlSetDelayMS(appRebootEventControl,500);
                       setStatusAck(PROTOCAL_CMD_REBOOT,0x00);
                    }
                    else
                    {
                       setStatusAck(PROTOCAL_CMD_REBOOT,0x01);
                    }
                   break;
             case  PROTOCAL_CMD_GET_ADDR :
                   if(payloadLength == 0)
                   {
                      getAddressAck();
                   }
                   else
                   {
                       setStatusAck(PROTOCAL_CMD_GET_ADDR,0x01); //readUserDataAck
                   }
                    break;
             case PROTOCAL_CMD_READ_NETWORK :
                   if(payloadLength == 0)
                   {
                      readNetworkInfoAck();
                   }
                   else
                   {
                       setStatusAck(PROTOCAL_CMD_READ_NETWORK,0x01);
                   }
                   break;
             case  PROTOCAL_CMD_READ_DATA :
                  if(payloadLength == 0)
                   {
                      readUserDataAck();
                   }
                   else
                   {
                       setStatusAck(PROTOCAL_CMD_READ_DATA,0x01);
                   }
                   break;
             case  PROTOCAL_CMD_WRITE_DATA :
                    if(payloadLength == 16)
                   {
                       setStatusAck(PROTOCAL_CMD_WRITE_DATA,0x00);
                       MEMMOVE(&userData.usData[0],
                              &serialbuffer.buffer[PROTOCAL_PAYLOAD_START_INDEX], 16);
                       halCommonSetToken(TOKEN_MODULE_USER_DATA,&userData);

                   }
                   else
                   {
                       setStatusAck(PROTOCAL_CMD_WRITE_DATA,0x01);
                   }
                   break;
             default:
                   break;
         }
    }
    
    else
    {
#ifdef CUSTOM_PROTOCOL_TYPE
      customDefineProtocalProcess();
#else
       //͸��ģʽ
      rfDataTransmite();
#endif

    }
    serialbuffer.length = 0;
    serialbuffer.buffer[0] = 0xff;
    serialbuffer.buffer[1] = 0xff;
    serialbuffer.buffer[2] = 0xff;
}
uint16_t getDeviceTokenAddr(void)
{
    return (tokenModuleCfgInfo.shortAddress);
}
void modeInitialFromToken(void)
{
      bool reboot_flag = false;
      tokTypeStackNodeData tokenNodeData;
      tokTypeStackKeys tokenKeys;
      //tokTypeStackParentInfo tokenParentInfo; //j
      //uint8_t tokenParentEuiDeafult[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
      EmAfZigbeeProNetwork tokenZigbeeProNetwork;
      //get token value and compare  the current network parameter
      halCommonGetToken(&tokenModuleCfgInfo,TOKEN_MODULE_CFG_INFO) ;
     // if(tokenModuleCfgInfo.shortAddress == 0xffff)return;               //cut by yh
      //only router type
      if((tokenModuleCfgInfo.deviceType != EMBER_ROUTER)
         ||(tokenModuleCfgInfo.shortAddress == 0x0000))
      {
          //tokenModuleCfgInfo.shortAddress = 0x0000;
          //tokenModuleCfgInfo.deviceType = EMBER_COORDINATOR;
          tokenModuleCfgInfo.shortAddress = 0x0001;
          tokenModuleCfgInfo.deviceType = EMBER_ROUTER;

      }
      tokenZigbeeProNetwork.nodeType = tokenModuleCfgInfo.deviceType;
      tokenZigbeeProNetwork.securityProfile = EMBER_AF_SECURITY_PROFILE_Z3;
      for (uint8_t i = 0; i < 2; i++) //double check solve the product test power on connect unstable
      {
          halCommonGetToken(&tokenNodeData, TOKEN_STACK_NODE_DATA);
          if ((tokenNodeData.nodeType == 0) || (tokenNodeData.radioFreqChannel != tokenModuleCfgInfo.channel) ||
              (tokenNodeData.panId != tokenModuleCfgInfo.panId) ||
              (tokenNodeData.zigbeeNodeId != tokenModuleCfgInfo.shortAddress) ||
              (MEMCOMPARE(tokenNodeData.extendedPanId,tokenModuleCfgInfo.extendPanId, 8) != 0))
          {
             tokenNodeData.panId = tokenModuleCfgInfo.panId;
             tokenNodeData.radioTxPower = tokenModuleCfgInfo.radioTxPower;
             emberAfPluginNetworkSteeringGetPowerForRadioChannelCallback(tokenModuleCfgInfo.channel);
             tokenNodeData.radioFreqChannel = tokenModuleCfgInfo.channel;
             tokenNodeData.stackProfile = EMBER_STACK_PROFILE;
             tokenNodeData.nodeType = tokenZigbeeProNetwork.nodeType;
             tokenNodeData.zigbeeNodeId = tokenModuleCfgInfo.shortAddress;
             MEMMOVE(tokenNodeData.extendedPanId, tokenModuleCfgInfo.extendPanId, 8);
             halCommonSetToken(TOKEN_STACK_NODE_DATA,&tokenNodeData);
             reboot_flag = true;
          }
          halCommonGetToken(&tokenKeys, TOKEN_STACK_KEYS);
          if ((tokenKeys.activeKeySeqNum != 0x01) ||
               (MEMCOMPARE(tokenKeys.networkKey,tokenModuleCfgInfo.nwkKey, 16) != 0))
          {
             tokenKeys.activeKeySeqNum = 0x01;
             MEMMOVE(tokenKeys.networkKey, tokenModuleCfgInfo.nwkKey, 16);
             halCommonSetToken(TOKEN_STACK_KEYS,&tokenKeys);
             reboot_flag = true;
          }
          //halCommonGetToken(&tokenParentInfo, TOKEN_STACK_PARENT_INFO);
          //if ((tokenParentInfo.parentNodeId != 0xFFFF) ||
          //    (MEMCOMPARE(tokenParentInfo.parentEui, tokenParentEuiDeafult, 8) != 0))
          //{
          //   tokenParentInfo.parentNodeId = 0xFFFF;
          //   MEMMOVE(tokenParentInfo.parentEui, tokenParentEuiDeafult, 8);
          //   halCommonSetToken(TOKEN_STACK_PARENT_INFO,&tokenParentInfo);
          //   reboot_flag = true;
          //}
       }
       if (reboot_flag == true)
       {
          halReboot(); //jin rootboot for intial the stack
       }
}
#ifdef  CUSTOM_PROTOCOL_TYPE
   //int8u   header;
   //int8u  length;
  // int8u  cmd0;
  // int8u  cmd1;
  // int8u  data[80];
  // int8u  fcs;//frame check-sum
uint8_t checkSum(uint8_t *data,uint8_t length)
{
    uint8_t i,checkSum = 0;

    for(i = 0;i < length;i++)
      checkSum += *(data+i);
    return checkSum;
}
//zigbee-->mcu ACK
void serialSendAck(uint8_t cmd,uint8_t ackCode)
{
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = 0x02;
   protocolDataBuffer.cmd0 = 0x01;
   protocolDataBuffer.cmd1 = CMD1_ZIGBEE_ACK;
   protocolDataBuffer.data[0] = cmd;
   protocolDataBuffer.data[1] =  ackCode;
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
   //fcs for end data
   protocolDataBuffer.data[2] =  protocolDataBuffer.fcs;
   //
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&protocolDataBuffer.header,
                        protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}


//zigbee-->mcu ACK  by xie
void lewinserialSendAck(uint8_t cmd,uint8_t ackCode)
{
   lewinProtocolDataBuffer.header1 = LEWIN_FRAME_HEAD1;
   lewinProtocolDataBuffer.header2 = LEWIN_FRAME_HEAD2;
   lewinProtocolDataBuffer.version = LEWIN_PRO_VERSION;
   lewinProtocolDataBuffer.cmdIDH = 0;
   lewinProtocolDataBuffer.cmdIDL = 0;
   lewinProtocolDataBuffer.funcCode = CMD1_ZIGBEE_ACK;
   lewinProtocolDataBuffer.lengthH = 0;
   lewinProtocolDataBuffer.lengthL = 1;
   lewinProtocolDataBuffer.data[0] = ackCode;
   
  // protocolDataBuffer.cmd1 = CMD1_ZIGBEE_ACK;
  // protocolDataBuffer.data[0] = cmd;
   //protocolDataBuffer.data[1] =  ackCode;
   lewinProtocolDataBuffer.fcs = getCrc16(&lewinProtocolDataBuffer.header1,0,
                                     (lewinProtocolDataBuffer.lengthH<<8| lewinProtocolDataBuffer.lengthL)+ LEWIN_DATA_OFFSET);
   //fcs for end data
   //protocolDataBuffer.data[2] =  protocolDataBuffer.fcs;
   //
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&lewinProtocolDataBuffer.header1,
                        10);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
//���MAC Addr+ NWK Addr����������֡
void fillAddressBuffer(uint8_t *buffer)
{
   EmberNodeId nodeId;
   EmberEUI64 eui64;
   nodeId = emberGetNodeId();
   emberAfGetEui64(eui64);
   *(buffer+0) = eui64[0];
   *(buffer+1) = eui64[1];
   *(buffer+2) = eui64[2];
   *(buffer+3) = eui64[3];
   *(buffer+4) = eui64[4];
   *(buffer+5) = eui64[5];
   *(buffer+6) = eui64[6];
   *(buffer+7) = eui64[7];
   *(buffer+8) = (nodeId & 0x00ff);
   *(buffer+9) = (nodeId >> 8) & 0x00ff;
}
//�Ӵ���֡�л�ȡ���Ƶ��б�ĸ���
uint8_t getControlPointListNumber(void)
{
   uint8_t cpListNumber;
   cpListNumber = protocolRxBuffer.data[0];
   return cpListNumber;
}
uint8_t getControlPointListCount(void)
{
   uint8_t cpListCount;
   cpListCount = protocolRxBuffer.data[1];//add byte
   return cpListCount;
}
//��ȡ����֡���ݳ���
uint8_t getDataLengthFromBuffer(void)
{
   uint8_t length;
   length = protocolRxBuffer.length;
   return length;
}
//zigbee �� MCU��ȡ���Ƶ��б�
void serialCommandGetControlPointList(void)
{
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = 0x00;
   protocolDataBuffer.cmd0 = 0x01;
   protocolDataBuffer.cmd1 = CMD1_QUERY_CONTROL_POINT_LIST;
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
   //fcs for end data
   protocolDataBuffer.data[0] =  protocolDataBuffer.fcs;
   //
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&protocolDataBuffer.header,
                        protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
//zigbee��ȡMCU���Ƶ��ֵ
void serialCommandGetControlPointValue(uint16_t cpCodeNumber)
{
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = 0x02;
   protocolDataBuffer.cmd0 = 0x01;
   protocolDataBuffer.cmd1 = CMD1_QUERY_CONTROL_POINT;
   protocolDataBuffer.data[0] = cpCodeNumber;
   protocolDataBuffer.data[1] =  (cpCodeNumber >> 8) & 0x00ff;
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
   //fcs for end data
   protocolDataBuffer.data[2] =  protocolDataBuffer.fcs;
   //
   emberSerialWriteData(PROTOCAL_SERIAL_PORT,&protocolDataBuffer.header,
                        protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH);
   emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
//RF�������Ϳ��Ƶ��б�
void rfSendCommandPushControlPointList(void)
{
   uint8_t cpListNumber;
   uint8_t cpListCount;
   uint8_t cpListIndex;
   if(protocolRxBuffer.length != 2 + 2*protocolRxBuffer.data[1])//length is right?
   {
     serialSendAck(serialbuffer.buffer[3],0x01);
     return;//list legth is 1 + 1(n) +2n
   }
   serialSendAck(serialbuffer.buffer[3],0x00);
   cpListNumber = getControlPointListNumber();
    cpListCount = getControlPointListCount();
   protocolDataBuffer.header = FRAME_HEADER;
  // protocolDataBuffer.length = 11 + 4*cpListNumber;
   protocolDataBuffer.length = 12 + 2*cpListCount;
   protocolDataBuffer.cmd0 = FRAME_CMD0;
   protocolDataBuffer.cmd1 = RF_CMD1_PUSH_CONTROL_POINT_LIST;
   //data[0]~data[10] is address
   fillAddressBuffer(&protocolDataBuffer.data[0]); //RF_ADDRESS_OFFSET = 10
   protocolDataBuffer.data[10] = protocolRxBuffer.data[0]; // cpListNo
   protocolDataBuffer.data[11] = protocolRxBuffer.data[1]; // cpListNumber
   for(cpListIndex = 0;cpListIndex < cpListCount;cpListIndex++)// cpList code and value
   {
      protocolDataBuffer.data[12+2*cpListIndex] =  protocolRxBuffer.data[2 + 2*cpListIndex];
      protocolDataBuffer.data[13+2*cpListIndex] =  protocolRxBuffer.data[3 + 2*cpListIndex];
      //protocolDataBuffer.data[14+3*cpListIndex] =  protocolRxBuffer.data[3 + 4*cpListIndex];
     // protocolDataBuffer.data[15+3*cpListIndex] =  protocolRxBuffer.data[3 + 4*cpListIndex];

   }
   cpListIndex = cpListIndex-1;
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
    protocolDataBuffer.rfLength = protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   //protocolDataBuffer.data[15+4*cpListIndex] = protocolDataBuffer.fcs;
   protocolDataBuffer.data[14+2*cpListIndex] = protocolDataBuffer.fcs;
   //
   rfCommandSetup(0x0000,0,&protocolDataBuffer.rfLength,protocolDataBuffer.rfLength+1);
}
//RF�������Ϳ��Ƶ���Ϣ����
void rfSendCommandPushControlPoint(void)
{
   uint8_t cpListCount;
   uint8_t cpListIndex;
   cpListCount = protocolRxBuffer.data[0];
   if(protocolRxBuffer.length != 1 + 4*cpListCount)
   {
     serialSendAck(serialbuffer.buffer[3],0x01);
     return;//legth is 3
   }
   serialSendAck(serialbuffer.buffer[3],0x00);
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = 1 + 4*cpListCount;
   protocolDataBuffer.cmd0 = FRAME_CMD0;
   protocolDataBuffer.cmd1 = RF_CMD1_PUSH_CONTROL_POINT;
   //data[0]~data[10] is address
  // fillAddressBuffer(&protocolDataBuffer.data[0]);
   protocolDataBuffer.data[0] = protocolRxBuffer.data[0];
   for(cpListIndex = 0;cpListIndex < cpListCount;cpListIndex++)// cpList code and value
   {
      protocolDataBuffer.data[1+4*cpListIndex] =  protocolRxBuffer.data[1 + 4*cpListIndex];
      protocolDataBuffer.data[2+4*cpListIndex] =  protocolRxBuffer.data[2 + 4*cpListIndex];
      protocolDataBuffer.data[3+4*cpListIndex] =  protocolRxBuffer.data[3 + 4*cpListIndex];
      protocolDataBuffer.data[4+4*cpListIndex] =  protocolRxBuffer.data[4 + 4*cpListIndex];
   }
   cpListIndex = cpListIndex-1;
  // protocolDataBuffer.data[11] =  protocolRxBuffer.data[1];
  // protocolDataBuffer.data[12] =  protocolRxBuffer.data[2];
  // protocolDataBuffer.data[13] =  protocolRxBuffer.data[3];
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
    protocolDataBuffer.rfLength = protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   protocolDataBuffer.data[5+4*cpListIndex] = protocolDataBuffer.fcs;
   //
   rfCommandSetup(0x0000,1,&protocolDataBuffer.rfLength,protocolDataBuffer.rfLength+1);
}
//RF���Ϳ��ƿ��Ƶ�����
void rfSendCommandCtrlControlPoint(void)
{
   if(protocolRxBuffer.length != 4)
   {
     serialSendAck(serialbuffer.buffer[3],0x01);
     return;//legth is 3
   }
   serialSendAck(serialbuffer.buffer[3],0x00);
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = 0x0E;
   protocolDataBuffer.cmd0 = FRAME_CMD0;
   protocolDataBuffer.cmd1 = RF_CMD1_CTRL_CONTROL_POINT;
   //data[0]~data[10] is address
   fillAddressBuffer(&protocolDataBuffer.data[0]);
   protocolDataBuffer.data[10] = protocolRxBuffer.data[0];
   protocolDataBuffer.data[11] =  protocolRxBuffer.data[1];
   protocolDataBuffer.data[12] =  protocolRxBuffer.data[2];
   protocolDataBuffer.data[13] =  protocolRxBuffer.data[3];
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
   protocolDataBuffer.rfLength = protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   protocolDataBuffer.data[14] = protocolDataBuffer.fcs;
   //
   rfCommandSetup(0x0000,0,&protocolDataBuffer.rfLength,protocolDataBuffer.rfLength+1);
}
//RF���͵��ó�������
void rfSendCommandRecallScene(void)
{
   if(protocolRxBuffer.length != 1)
   {
     serialSendAck(serialbuffer.buffer[3],0x01);
     return;//legth is 1
   }
   serialSendAck(serialbuffer.buffer[3],0x00);
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = 0x0B;
   protocolDataBuffer.cmd0 = FRAME_CMD0;
   protocolDataBuffer.cmd1 = RF_CMD1_RECALL_SCENE;
   //data[0]~data[10] is address
   fillAddressBuffer(&protocolDataBuffer.data[0]);
   protocolDataBuffer.data[10] = protocolRxBuffer.data[0];
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
    protocolDataBuffer.rfLength = protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   protocolDataBuffer.data[11] = protocolDataBuffer.fcs;
   //
   rfCommandSetup(0x0000,0,&protocolDataBuffer.rfLength,protocolDataBuffer.rfLength+1);
}
//RF͸����������(���͵�����ǰ10���ֽ����豸��ַ��ϢMAC Addr+NWK Addr)
void rfSendCommandDirectTx(void)
{
   uint8_t dataIndex = 0;
   serialSendAck(serialbuffer.buffer[3],0x00);
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = getDataLengthFromBuffer() + 10;//10 = addr length
   protocolDataBuffer.cmd0 = FRAME_CMD0;
   protocolDataBuffer.cmd1 = RF_CMD1_DIRECT_TX;
   //data[0]~data[10] is address
   fillAddressBuffer(&protocolDataBuffer.data[0]);
   for(dataIndex = 0;dataIndex < protocolRxBuffer.length;dataIndex++)
   {
      protocolDataBuffer.data[10 + dataIndex] = protocolRxBuffer.data[dataIndex];
   }
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
    protocolDataBuffer.rfLength = protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   protocolDataBuffer.data[10 + dataIndex] = protocolDataBuffer.fcs;
   //
   rfCommandSetup(0x0000,0,&protocolDataBuffer.rfLength,protocolDataBuffer.rfLength+1);
}
//RF����OTA����������
void rfSendCommandOtaFinish(uint8_t status)
{
   //status = 0x00 sucess 0x01 fail
   serialSendAck(serialbuffer.buffer[3],0x00);
   protocolDataBuffer.header = FRAME_HEADER;
   protocolDataBuffer.length = 11;//10 = addr length  1 = status
   protocolDataBuffer.cmd0 = FRAME_CMD0;
   protocolDataBuffer.cmd1 = RF_CMD1_OTA_FINISH;
   //data[0]~data[10] is address
   fillAddressBuffer(&protocolDataBuffer.data[0]);
   protocolDataBuffer.data[10] = status;
   protocolDataBuffer.fcs = checkSum(&protocolDataBuffer.header,
                                     protocolDataBuffer.length + DATA_OFFSET);
    protocolDataBuffer.rfLength = protocolDataBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   protocolDataBuffer.data[11] = protocolDataBuffer.fcs;
   //
   rfCommandSetup(0x0000,0,&protocolDataBuffer.rfLength,protocolDataBuffer.rfLength+1);
}
//
void customDefineProtocalProcess(void)
{
    unsigned int crc = 0;
    uint8_t i=0;
    //length
    if((serialbuffer.length < FULL_BUFFER_MIN_LENGTH)
       ||(serialbuffer.length != serialbuffer.buffer[1] + FULL_BUFFER_MIN_LENGTH)
       ||(serialbuffer.buffer[0] != FRAME_HEADER))
    {

       return;
    }
    //check sum  error
   // crc = (serialbuffer.buffer[serialbuffer.length-2]<<8)|(serialbuffer.buffer[serialbuffer.length-1]);
   // if(getCrc16(serialbuffer.buffer,0,serialbuffer.length-2) != crc)
    else if(checkSum(&serialbuffer.buffer[0],serialbuffer.length-1)
            != serialbuffer.buffer[serialbuffer.length-1] )
    {
       serialSendAck(serialbuffer.buffer[3],0x01);
    }
    else
    {
       MEMMOVE(&protocolRxBuffer.header,&serialbuffer.buffer[0],serialbuffer.length);
        switch(protocolRxBuffer.cmd1)
        {
             case  CMD1_PUSH_CONTROL_POINT_LIST:
                   rfSendCommandPushControlPointList();
                   break;
             case  CMD1_PUSH_CONTROL_POINT:
                   rfSendCommandPushControlPoint();
                   break;
             case  CMD1_CTRL_CONTROL_POINT:
                   rfSendCommandCtrlControlPoint();
                   break;
             //case  CMD1_RECALL_SCENE:
             //      rfSendCommandRecallScene();
             //      break;
             case  CMD1_MCU_ACK:
                   break;
            case  CMD1_DIRECT_TX:
                   rfSendCommandDirectTx();
                   break;
            case  CMD1_PENELRATE_SEND_DEBUG://(EmberPanId)yhPenetrateDataId.panid

                  yhSendDataToDebugger(&serialbuffer.buffer[4],serialbuffer.length-5);

                  break;
            case  CMD1_PENELRATE_UNICAST_GW:
              serialbuffer.buffer[3] = CMD1_PENETRATE_TO_GW;
              serialbuffer.buffer[(serialbuffer.length-1)] = checkSum(serialbuffer.buffer,(serialbuffer.length-1));
              yhEmAfCliRawCommand(serialbuffer.buffer,serialbuffer.length,0xfc01,0x0000);
                  //rfCommandSetup(0x0000,0,serialbuffer.buffer,serialbuffer.length);
                  //yhSendDataToGW(serialbuffer.buffer, serialbuffer.length,0);
                  break;

            case  CMD1_PENELRATE_BROADCAST_GW:

              serialbuffer.buffer[3] = CMD1_PENETRATE_TO_GW;
              serialbuffer.buffer[(serialbuffer.length-1)] = checkSum(serialbuffer.buffer,(serialbuffer.length-1));
              yhEmAfCliRawCommand(serialbuffer.buffer,serialbuffer.length,0xfc01,0xffff);
                  break;

            case  CMD1_GET_SOFTWARE_VERSION:
                serialbuffer.buffer[1] = 0x01;
                serialbuffer.buffer[3] = CMD1_SOFTWARE_VERSION;
                serialbuffer.buffer[4] = SOFTWARE_VERSION;
                serialbuffer.buffer[5] = checkSum(serialbuffer.buffer,5);
                emberSerialWriteData(PROTOCAL_SERIAL_PORT,serialbuffer.buffer,6);
                emberSerialWaitSend(PROTOCAL_SERIAL_PORT);

                break;

             default:
                   break;
        }
        
      
      //add by xie
      /*  MEMMOVE(&lewinProtocolDataBuffer.header1,&serialbuffer.buffer[0],serialbuffer.length);
        switch(lewinProtocolDataBuffer.funcCode)
        {
             case  CMD1_PUSH_CONTROL_POINT_LIST:
                   rfSendCommandPushControlPointList();
                   break;
             case  CMD1_PUSH_CONTROL_POINT:
                   rfSendCommandPushControlPoint();
                   break;
             case  CMD1_CTRL_CONTROL_POINT:
                   rfSendCommandCtrlControlPoint();
                   break;
             //case  CMD1_RECALL_SCENE:
             //      rfSendCommandRecallScene();
             //      break;
             case  CMD1_MCU_ACK:
                   break;
            case  CMD1_DIRECT_TX:
                   rfSendCommandDirectTx();
                   break;
             default:
                   break;
        }*/
    }

}
#endif

void yhSendDataToDebugger(uint8_t* data, uint8_t len)
{
  uint8_t buff[150];
  uint8_t i;
  uint16_t crcTemp;
  if(len>140)
  {
      return;
  }

  buff[0] = 0xf0;
  buff[1] = 0x0f;
  buff[2] = 0x01;
  buff[3] = 0x00;
  buff[4] = 0x00;
  buff[5] = 0x06;
  buff[6] = 0x00;
  buff[7] = len+12;
  memset(&buff[8],0x00,10);
  emberAfGetEui64(&buff[8]);
  buff[18] = 0x00;
  buff[19] = len;
  memcpy(&buff[20], data, len);
  crcTemp = getCRC(buff, len+20);
  //emberAfCorePrintln("crcTemp = %2x ",crcTemp);
  buff[20+len] = (uint8_t)(crcTemp>>8);
  buff[20+len+1] = (uint8_t)crcTemp;

  yhEmAfInterpanFragmentTestCommand( 0x6868,\
                                        yhPenetrateDataId.longid, \
                                        0xfc99,\
                                        buff,\
                                        20+len+1+1);
}


void yhSendDataToGW(uint8_t* data, uint8_t len,uint8_t cast)
{
  rfCommandSetup(0x0000,cast,data,len);
}

void yhSerialSendProtocolFuction( int8u head, int8u len, int8u cmd0, int8u cmd1, int8u *data )
{
    if(len > (YH_BUFF_MAX_LEN - 5) || len == 0)
    {
        return;
    }
    yhGlobalBuff[0] = head;
    yhGlobalBuff[1] = len;
    yhGlobalBuff[2] = cmd0;
    yhGlobalBuff[3] = cmd1;
    memmove(&yhGlobalBuff[4], data, len);
    yhGlobalBuff[ 4 + len ] = checkSum(yhGlobalBuff, len+4 );
    emberSerialWriteData(PROTOCAL_SERIAL_PORT,yhGlobalBuff,len+4+1);
    emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
    memset(yhGlobalBuff, 0xff, YH_BUFF_MAX_LEN);
}

void yhDebugDataAndSendFunction( int8u version, int16u cmd, int8u functionCode, int16u dataLen, int8u *destLongMac, int16u panId, int8u *data  )
{
    uint16_t crcTemp;
    memset(yhGlobalBuff, 0xff, YH_BUFF_MAX_LEN);
    yhGlobalBuff[0] = LEWIN_FRAME_HEAD1;
    yhGlobalBuff[1] = LEWIN_FRAME_HEAD2;
    yhGlobalBuff[2] = version;
    yhGlobalBuff[3] = cmd>>8;
    yhGlobalBuff[4] = cmd;
    yhGlobalBuff[5] = functionCode;
    yhGlobalBuff[6] = dataLen>>8;
    yhGlobalBuff[7] = dataLen;
    memmove( &yhGlobalBuff[8], destLongMac, 8 );
    yhGlobalBuff[16] = panId>>8;
    yhGlobalBuff[17] = panId;
    memmove( &yhGlobalBuff[18], data, dataLen-10 );
    crcTemp = getCRC( yhGlobalBuff, dataLen+8 );
    yhGlobalBuff[8+dataLen] = crcTemp>>8;
    yhGlobalBuff[8+dataLen+1] = crcTemp;

    yhEmAfInterpanFragmentTestCommand( 0x6868,\
                                       destLongMac, \
                                       0xfc99,\
                                       yhGlobalBuff,\
                                       8+dataLen+1+1);
}
