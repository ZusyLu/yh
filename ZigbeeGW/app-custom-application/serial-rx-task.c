#include PLATFORM_HEADER
#include "app/framework/util/common.h"
#include "app/framework/include/af.h"
#include "gw-cfg-tokens.h"
#include "serial-rx-task.h"
#include "serial-rx-queue.h"
#include "rf-tx-command.h"
#include "serial-ota-task.h"
#include "protocol-parsing.h"

//#include "eeprom.h"
/*------------------------------------------------
 ��C�ļ��Ĺ��ܰ������¼������֣�
1����������Э�鲿�ֹ���ʵ��
2������HOST�˴������ݵĻ��ζ��н��մ���
3����������HOST�˴������ͨ��RF���Ͷ�Ӧ��ָ��豸��
__________________________________________________*/

#define  OWN_SERIAL_RX_TASK   //

//#define  SERIAL_NACK

#define  SERIAL_RX_TIMEOUT     20

#define  RF_TX_TIMEOUT         100
//define this macro for customer protocol
#define  CUSTOM_PROTOCOL_TYPE
// network configuration protocol define
// ��������Э��
/*-----------------------------------------------
|���ֽ�Э���ʶ��   |   һ�ֽ�	 | N�ֽ�
-------------------------------------------------
|    A5 AA 55	 |    ������	 | ����
-------------------------------------------------*/

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
#define  PROTOCAL_CMD_REBOOT         0x11
#define  PROTOCAL_CMD_GET_ADDR       0x21
#define  PROTOCAL_CMD_READ_NETWORK   0x22
#define  PROTOCAL_CMD_READ_DATA      0x31
#define  PROTOCAL_CMD_WRITE_DATA     0x32
//add by yh 20210720
#define  YH_CREATE_30NETWORK        0XA1
#define  YH_OPEN_30NETWORK          0XA2
#define  YH_CLOSE_30NETWORK         0XA3
#define  YH_LEAVE_30NETWORK         0XA4
#define  YH_DATA_30NETWORK          0XA5
#define  YH_READ_YH_VERSION         0XA6

//YH APP 3.0
#define  YH_CREATE_30_NETWORK       0X00
#define  YH_OPEN_NETWORK            0X01
#define  YH_CLOSE_NETWORK           0X02
#define  YH_RESET                   0x03
#define  YH_LEAVE_NETWORK           0x04
#define  YH_READ_30_NETWORK         0x05

#define   YH_CONFIG_CMD_LEN         0X03
#define   YH_LEAVE_CMD_LEN          0X04
#define   YH_WINDOW_CONTROL         0X08
#define   YH_WINDOW_PERCENTAGE_CONTROL         0X09
#define   YH_WINDOW_VALUE_CONTROL              0X0a

//window cmd

#define    YH_WINDOW_TILT_PERCENTAGE          0X08
#define    YH_WINDOW_LIFT_VALUE               0X04
#define    YH_WINDOW_TILT_VALUE               0X07

#define    YH_DOOR_LOCK_CONTROL               0xf0

//yh
#define   YH_ZCL_HEAD                         0X51
#define   YH_ZCL_TAIL                         0x15



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

typedef union{

  int8u   data[4];
  int32u  linkKey;

}yhNetwork30LinkKeyType;

typedef union{
  int8u data[2];
  int16u clusterId;
}yhClusterIDType;

uint8_t yh_send[]="send 0x0000 1 1";
uint8_t yh_obj[5] = "ffff";
uint8_t yh_hex_obj[2] = 0;
//
SerialRxDateStruct  serialbuffer;
SerialTxDateStruct  serialTxBuffer;
DeviceCfgInfoStruct deviceCfgInfo;
tokTypeModuleCfgStruct tokenModuleCfgInfo;
tokTypeModuleUserDataStruct userData;
tokTypeModuleOtaInfoStruct otaInfo;
//tokTypeModuleOtaDownLoadInfoStruct otaDownloadInfo;
//
EmberEventControl appRebootEventControl;
void halRebootEventHandler(void);
void serialCommandParse(void);
void customDefineProtocalProcess(void);
extern bool emberProcessCommandString(uint8_t *input, uint8_t sizeOrPort);
//
ProtocolDataStruct protocolRxBuffer; //
ProtocolDataStruct protocolRingBuffer;
uint16_t lastSerialRxTime = 0;
uint32_t packIndex = 0;
uint32_t fileSize = 0;
uint16_t otaImageType;
uint32_t otaVersion;
//yh
yhNetwork30LinkKeyType yhNetwork30LinkKey;
yhNodeIDType yhNodeId;
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
//��ȡ���������ַ����
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
//��ȡ���������0x22
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
//��ȡ�洢������
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

//��ȡOTAͷ�ļ���������
void readOtaInfoAck(void)
{
     
     halCommonGetToken(&otaInfo,TOKEN_MODULE_OTA_INFO);
    // MEMMOVE(serialTxBuffer.ackData,userData.usData, 16);
    // emberSerialWriteData(PROTOCAL_SERIAL_PORT,&serialTxBuffer.frameid1,20);
    // emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
//������ڻ�����

void clearSerialBuffer(void)
{
     serialbuffer.length = 0;
     serialbuffer.buffer[0] = 0xff;
     serialbuffer.buffer[1] = 0xff;
     serialbuffer.buffer[2] = 0xff;
}
//���ڽ������ݴ���
void serialRxProcess(uint8_t Port)
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
//������������Э��Ľ��մ���
void serialCommandParse(void)
{
      uint8_t  payloadLength;
      uint8_t ii=0;
      if(serialbuffer.length != 0){
          emberAfCorePrintln("recevie data:");
          emberAfCorePrintln("serialbuffer.length = %d , serialbuffer.buffer = ",serialbuffer.length);
          for(ii=0;ii<serialbuffer.length;ii++){
              emberAfCorePrint("%x",serialbuffer.buffer[ii]);

          }
          emberAfCorePrintln("");
      }


      if(serialbuffer.length < 2) // PROTOCAL_BUFFER_MIN_LENGTH
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
                        tokenModuleCfgInfo.deviceType = 0x01; 
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
                        setStatusAck(PROTOCAL_CMD_SHORT_ADDR,0x00);                        
                        tokenModuleCfgInfo.shortAddress = 0x0000; 

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
               case PROTOCAL_CMD_REBOOT :
                    if(payloadLength == 0)
                    {
                       tokenModuleCfgInfo.deviceType = 0x01; 
                       tokenModuleCfgInfo.shortAddress = 0x0000; 
                       halCommonSetToken(TOKEN_MODULE_CFG_INFO,&tokenModuleCfgInfo);
                       emberEventControlSetDelayMS(appRebootEventControl,100);
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

               case YH_CREATE_30NETWORK:
                    {
                        emberAfPluginNetworkCreatorStart(true);
                        setStatusAck(YH_CREATE_30NETWORK,0x00);
                    }
                    break;

               case YH_OPEN_30NETWORK:
                    {
                        emberAfPluginNetworkCreatorSecurityOpenNetwork();
                        setStatusAck(YH_OPEN_30NETWORK,0x00);
                    }
                    break;

               case YH_CLOSE_30NETWORK:
                    {
                        emberAfPluginNetworkCreatorSecurityCloseNetwork();
                        setStatusAck(YH_CLOSE_30NETWORK,0x00);
                    }
                    break;

               case YH_LEAVE_30NETWORK:
                    {
                        setStatusAck(YH_LEAVE_30NETWORK,0x00);
                        emberLeaveNetwork();
                    }
                    break;

               case YH_DATA_30NETWORK:
                    {
                        yhNetwork30LinkKey.linkKey = getOutgoingApsFrameCounter();
                        MEMCOPY(&serialbuffer.buffer[serialbuffer.length],yhNetwork30LinkKey.data,4);

                        emberSerialWriteData(PROTOCAL_SERIAL_PORT,serialbuffer.buffer,serialbuffer.length+4);
                        emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
                    }
                    break;

               case YH_READ_YH_VERSION:
                    {
                        serialbuffer.buffer[serialbuffer.length] = GW_VERSION;
                        emberSerialWriteData(PROTOCAL_SERIAL_PORT,serialbuffer.buffer,5);
                        emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
                    }
                    break;
               default:
                     break;
           }
      }
      else
      {
        // emberSerialPrintf(1,"\r\customDefineProtocalProcess,  %x,%x,%x,%x,%x",serialbuffer.buffer[0] ,
         //                  serialbuffer.buffer[1] ,serialbuffer.buffer[2],serialbuffer.buffer[3],serialbuffer.buffer[4] );
         //cut by yh
          //emberSerialPrintf(1,"\r\customDefineProtocalProcess,  %x,%x,%x,%x,%x",\
          //serialbuffer.buffer[0] , serialbuffer.buffer[1] ,serialbuffer.buffer[2],serialbuffer.buffer[3],serialbuffer.buffer[4] );
          customDefineProtocalProcess();
          ParseProtocolFramingFunction();
#if 0
          if(    serialbuffer.length == YH_LEAVE_CMD_LEN \
              || serialbuffer.length == YH_WINDOW_CONTROL \
              || serialbuffer.length == YH_WINDOW_PERCENTAGE_CONTROL\
              || serialbuffer.length == YH_WINDOW_VALUE_CONTROL){

              yhDealWith(serialbuffer.buffer,serialbuffer.length);

          }else if(serialbuffer.buffer[0] == 0x51){

              yhDealWith(serialbuffer.buffer,serialbuffer.length);
          }
#endif
      }
      serialbuffer.length = 0;
      serialbuffer.buffer[0] = 0xff;
      serialbuffer.buffer[1] = 0xff;
      serialbuffer.buffer[2] = 0xff;
}
//������Token�����г�ʼ������
void modeInitialFromToken(void)
{
      bool reboot_flag = false;
      tokTypeStackNodeData tokenNodeData;
      tokTypeStackKeys tokenKeys;
//     tokTypeStackParentInfo tokenParentInfo; //j
//     uint8_t tokenParentEuiDeafult[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
      EmAfZigbeeProNetwork tokenZigbeeProNetwork;
      //get token value and compare  the current network parameter
      halCommonGetToken(&tokenModuleCfgInfo,TOKEN_MODULE_CFG_INFO) ;
      //only coordinator type
      if((tokenModuleCfgInfo.deviceType != EMBER_COORDINATOR)
         ||(tokenModuleCfgInfo.shortAddress != 0x0000))
      {
          tokenModuleCfgInfo.shortAddress = 0x0000;
          tokenModuleCfgInfo.deviceType = EMBER_COORDINATOR;

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

/*---------------------------protocal format define-----------------------------
// host and zb gateway protocol define
// ����Э��
HEAD	   LEN	      CMD0	CMD1	  DATA	   FCS
1 Byte	  1 Byte	      1 Byte	1 Byte		   1 Byte
֡ͷ,7C	  ���ݳ���     Ԥ��	������	  ���ݶ�    У��

--------------------------------------------------------------------------------*/
//У��ͼ���
uint8_t checkSum(uint8_t *data,uint8_t length)
{
       uint8_t i,checkSum = 0;

       for(i = 0;i < length;i++)
        checkSum += *(data+i);
       return checkSum;
}
//zigbee--->host mcu ACK
void serialSendAck(uint8_t cmd,uint8_t ackCode)
{
    //ackCode = 00 success  0x01 fail
#ifndef SERIAL_NACK
     uint8_t buffer[7];
     buffer[0] = FRAME_HEADER; //protocolDataBuffer.header = FRAME_HEADER;
     buffer[1] = 0x02;// protocolDataBuffer.length = 0x02;
     buffer[2] = 0x01;//protocolDataBuffer.cmd0 = 0x01;
     buffer[3] = GW_ZIGBEE_ACK;//protocolDataBuffer.cmd1 = GW_ZIGBEE_ACK;
     buffer[4] = cmd;//protocolDataBuffer.data[0] = cmd;
     buffer[5] = ackCode;//protocolDataBuffer.data[1] =  ackCode;
     buffer[6] = checkSum(buffer,6);//protocolDataBuffer.fcs
     //
     emberSerialWriteData(PROTOCAL_SERIAL_PORT,buffer,7);
     emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
#endif
}
//zigbee--->host mcu �����������OTA����
void serialSendImageRequest(uint8_t ctrlStatus)
{
    //ctrlStatus = 00 stop  0x01 start
     uint8_t buffer[12];
     buffer[0] = FRAME_HEADER; //protocolDataBuffer.header = FRAME_HEADER;
     buffer[1] = 0x07;// protocolDataBuffer.length = 0x07;
     buffer[2] = 0x01;//protocolDataBuffer.cmd0 = 0x01;
     buffer[3] = GW_IMAGE_REQUEST;//protocolDataBuffer.cmd1 = GW_IMAGE_REQUEST;
     buffer[4] = ctrlStatus;//protocolDataBuffer.data[0] = ctrlStatus;
     buffer[5] = validImageType[0];//protocolDataBuffer.data[1] =  image type;
     buffer[6] = validImageType[1];//protocolDataBuffer.data[2] =  image type;
     buffer[7] = imageVersion[0];//protocolDataBuffer.data[3] = image version;
     buffer[8] = imageVersion[1];//protocolDataBuffer.data[4] = image version;
     buffer[9] = imageVersion[2];//protocolDataBuffer.data[5] = image version;
     buffer[10] = imageVersion[3];//protocolDataBuffer.data[6] = image version;
     buffer[11] = checkSum(buffer,11);//protocolDataBuffer.fcs
     //
     emberSerialWriteData(PROTOCAL_SERIAL_PORT,buffer,12);
     emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
//zigbee--->host mcu ����һ֡OTA����
void serialSendImagePacketRequest(uint32_t offset,uint8_t length)
{
    //ctrlStatus = 00 stop  0x01 start
     uint8_t buffer[10];
     buffer[0] = FRAME_HEADER; //protocolDataBuffer.header = FRAME_HEADER;
     buffer[1] = 0x05;// protocolDataBuffer.length = 0x05;
     buffer[2] = 0x01;//protocolDataBuffer.cmd0 = 0x01;
     buffer[3] = GW_IMAGE_PACKET_REQUEST;//protocolDataBuffer.cmd1 = GW_IMAGE_REQUEST; 87
     buffer[4] = offset & 0x000000ff;//protocolDataBuffer.data[0] = offset;
     buffer[5] = (offset >>8 & 0x000000ff);//protocolDataBuffer.data[1] =  image type;
     buffer[6] = (offset >>16 & 0x000000ff);//protocolDataBuffer.data[2] =  image type;
     buffer[7] = (offset >> 24 & 0x000000ff);//protocolDataBuffer.data[3] = image version;
     buffer[8] = length;//protocolDataBuffer.data[4] = image version;
     buffer[9] = checkSum(buffer,9);//protocolDataBuffer.fcs
     //
     emberSerialWriteData(PROTOCAL_SERIAL_PORT,buffer,10);
     emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
}
// �ڽ��չ���Э�鴮�Ĵ���
// ��Ҫ����RF���������֡��ֱ�ӷ��뻷�ζ����л���
void customDefineProtocalProcess(void)
{
    //length
    int offset = 0;
    uint8_t res=0;
    
    if((serialbuffer.length < FULL_BUFFER_MIN_LENGTH)
       ||(serialbuffer.length != serialbuffer.buffer[1] + FULL_BUFFER_MIN_LENGTH)
       ||(serialbuffer.buffer[0] != FRAME_HEADER))
    {
      //emberSerialPrintf(1,"\r\n return 1,serialbuffer.length is %d",serialbuffer.length);
       return;
    }
    else if(checkSum(&serialbuffer.buffer[0],serialbuffer.length-1)
             != serialbuffer.buffer[serialbuffer.length-1] )
    {
       
       serialSendAck(serialbuffer.buffer[3],0x01);
    }
     else
    {
        MEMMOVE(&protocolRxBuffer.header,&serialbuffer.buffer[0],serialbuffer.length);
        emberSerialPrintf(1,"\r\n protocolRxBuffer.cmd1 is %d",protocolRxBuffer.cmd1);
        switch(protocolRxBuffer.cmd1)
        {
             case  GW_QUERY_CONTROL_POINT_VALE:
                   if(protocolRxBuffer.length != 0x0c)
                   {
                       serialSendAck(serialbuffer.buffer[3],0x01);
                   }
                   else
                   {
                       serialSendAck(serialbuffer.buffer[3],0x00);
                       pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);
                   }
                   break;
             case  GW_CONTROL_CONTROL_POINT:
                   if(protocolRxBuffer.length != 11 + 4*protocolRxBuffer.data[10])
                   {
                       serialSendAck(serialbuffer.buffer[3],0x01);
                   }
                   else
                   {
                       serialSendAck(serialbuffer.buffer[3],0x00);
                       pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);
                   }
                   break;
             case  GW_CONTROL_CONTROL_POINT_BROAD_CAST:
                   {
                       serialSendAck(serialbuffer.buffer[3],0x00);
                       pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);
                   }
                   break;

             case  GW_UPDATE_PANNEL:
                   if(protocolRxBuffer.length != 0x0e)
                   {
                       serialSendAck(serialbuffer.buffer[3],0x01);
                   }
                   else
                   {
                       serialSendAck(serialbuffer.buffer[3],0x00);
                       pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);
                   }
                   break;
             //case  GW_UPDATE_PANNEL_SCENE:
             //      if(protocolRxBuffer.length != 0x0b)
             //      {
             //          serialSendAck(serialbuffer.buffer[3],0x01);
             //      }
             //      else
             //      {
             //          serialSendAck(serialbuffer.buffer[3],0x00);
                       //pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);
             //      }
             //       break;
             case GW_OTA_NOTIFY:
                    // emberSerialPrintf(1,"\r\n cmd is GW_OTA_NOTIFY");
                     serialSendAck(serialbuffer.buffer[3],0x00);
                     pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);
                  break;
             case  GW_IMAGE_NOTIFY:
                        /* setSerialOtaState(OTA_STATE_START);
                         emberAfOtaStorageClearTempDataCallback();
                         
                         imageType[0] = serialbuffer.buffer[5];
                         imageType[1] = serialbuffer.buffer[4];
                         
                         imageVersion[0] = serialbuffer.buffer[9];
                         imageVersion[1] = serialbuffer.buffer[8];
                         imageVersion[2] = serialbuffer.buffer[7];
                         imageVersion[3] = serialbuffer.buffer[6];
                         
                         imageTotalSize = ((uint32_t)(serialbuffer.buffer[10]<<24))|((uint32_t)(serialbuffer.buffer[11]<<16))|((uint32_t)(serialbuffer.buffer[12]<<8))|((uint32_t)serialbuffer.buffer[13]);*/
                         ///////////////////////////////////////////
                   
                      packIndex = 0;
                     otaImageType = ((uint16_t)serialbuffer.buffer[4]<<8)|((uint16_t)serialbuffer.buffer[5]);
                     otaVersion = ((uint32_t)(serialbuffer.buffer[6]<<24))|((uint32_t)(serialbuffer.buffer[7]<<16))|((uint32_t)(serialbuffer.buffer[8]<<8))|((uint32_t)serialbuffer.buffer[9]);
                     fileSize = ((uint32_t)(serialbuffer.buffer[10]<<24))|((uint32_t)(serialbuffer.buffer[11]<<16))|((uint32_t)(serialbuffer.buffer[12]<<8))|((uint32_t)serialbuffer.buffer[13]);
                      halCommonGetToken(&otaInfo,TOKEN_MODULE_OTA_INFO);
                      if(otaInfo.version == otaVersion)
                      {
                            emberSerialPrintf(1,"\r\n invalid iamge\n");
                            break;
                      }
                      
                        
                        
                        
                     emberAfOtaStorageClearTempDataCallback();
                  
                     serialSendImageRequest(0);
                     serialSendImagePacketRequest(packIndex,64);
                     packIndex += 64;
                   
                     
                     
                    break;
             case  GW_IMAGE_TX:
                    
                      res=0;
                    offset = ((uint32_t)(serialbuffer.buffer[4]<<24))|((uint32_t)(serialbuffer.buffer[5]<<16))|((uint32_t)(serialbuffer.buffer[6]<<8))|((uint32_t)serialbuffer.buffer[7]);
                   
#if 0
  // cut  ota api
                     res=emberAfPluginEepromWrite(offset,
                                 &serialbuffer.buffer[9],
                                 serialbuffer.buffer[8]);
                     
#endif

                    if(fileSize <= packIndex)
                    {
                         otaInfo.version = otaVersion;
                         otaInfo.imageType = otaImageType;
                         otaInfo.isComplete = 1;
                        halCommonSetToken(TOKEN_MODULE_OTA_INFO,&otaInfo);
                        break;
                    }
                   else if(fileSize - packIndex < 64)
                   {
                       serialSendImagePacketRequest(packIndex,fileSize - packIndex);
                       packIndex = fileSize;
                   }
                   else 
                   {
                    serialSendImagePacketRequest(packIndex,64);
                    
                    packIndex += 64;
                    

                    
                   }
                    break;
             case  GW_ZIGBEE_REBOOT:
                   emberEventControlSetDelayMS(appRebootEventControl,50);
                   break;
             case  GW_MCU_ACK:
                   break;
             case  GW_DIRECT_TX:
                   if(protocolRxBuffer.length <= 0x0A)
                   {
                       serialSendAck(serialbuffer.buffer[3],0x01);
                   }
                   else
                   {
                       serialSendAck(serialbuffer.buffer[3],0x00);
                       pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);
                   }
                   break;
             default:
                   break;
        }
    }

}

void bin2str(char *to, const unsigned char *p, size_t len) {
  static const char *hex = "0123456789abcdef";
  for (; len--; p++) {
    *to++ = hex[p[0] >> 4];
    *to++ = hex[p[0] & 0x0f];
  }
  *to = '\0';
}

//yh 3.0 network operating            getOutgoingApsFrameCounter()
void yhDealWith(int8u* usartData,int8u len){

   if(usartData[0]==0x41 && usartData[3]==0x14 && len==4){
      emberSerialWriteData(PROTOCAL_SERIAL_PORT,usartData,4);
      emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
      yhNodeId.data[0] = usartData[1];
      yhNodeId.data[1] = usartData[2];
      yhZdoLeaveRequestCommand(yhNodeId.nodeId);

  }else if(usartData[0]==0x31 && usartData[3]==0x13 && len==4){
      emberSerialWriteData(PROTOCAL_SERIAL_PORT,usartData,4);
      emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
      yhNodeId.data[0] = usartData[1];
      yhNodeId.data[1] = usartData[2];
      yhZdoIeeeAddressRequestCommand(yhNodeId.nodeId);

  }else if(usartData[0]==YH_ZCL_HEAD \
      && ( usartData[7]==YH_ZCL_TAIL || usartData[8]==YH_ZCL_TAIL || usartData[9]==YH_ZCL_TAIL ) \
      && (len==YH_WINDOW_CONTROL || len==YH_WINDOW_PERCENTAGE_CONTROL ||len==YH_WINDOW_VALUE_CONTROL)){

      yhNodeId.data[0] = usartData[1];
      yhNodeId.data[1] = usartData[2];
      emberAfCorePrintln("test rev yh cmd = %x ",usartData[6]);
      switch(usartData[6]){
        case YH_WINDOW_OPEN:
          emberAfFillCommandWindowCoveringClusterWindowCoveringUpOpen();
          emberAfSetCommandEndpoints(1,usartData[3]);
          emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
          break;

        case YH_WINDOW_CLOSE:
          emberAfFillCommandWindowCoveringClusterWindowCoveringDownClose();
          emberAfSetCommandEndpoints(1,usartData[3]);
          emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
          break;

        case YH_WINDOW_STOP:
          emberAfFillCommandWindowCoveringClusterWindowCoveringStop();
          emberAfSetCommandEndpoints(1,usartData[3]);
          emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
          break;

        case YH_WINDOW_LIFT_PERCENTAGE:
          emberAfFillCommandWindowCoveringClusterWindowCoveringGoToLiftPercentage(usartData[7])
          emberAfSetCommandEndpoints(1,usartData[3]);
          emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
          break;

        case YH_WINDOW_TILT_PERCENTAGE:
          break;

        case YH_WINDOW_LIFT_VALUE:
          break;

        case YH_WINDOW_TILT_VALUE:
          break;

        case YH_DOOR_LOCK_CONTROL:
          yh_hex_obj[0] = yhNodeId.data[1];
          yh_hex_obj[1] = yhNodeId.data[0];
          emberAfCorePrintln("yh_hex_obj[0]=%x yh_hex_obj[1]=%x ",yh_hex_obj[0], yh_hex_obj[1]);
          bin2str(yh_obj,yh_hex_obj,2);
          emberAfCorePrintln("string = %p",yh_obj);
          memcpy(&yh_send[7],yh_obj,4);
          emberAfCorePrintln("will send string = %p",yh_send);
          emberProcessCommandString((uint8_t*)"raw 0x0000 {10 00 02 00424214AA0A900000000100000000000000000000003155}",\
                                    strlen("raw 0x0000 {10 00 02 00424214AA0A900000000100000000000000000000003155}") );
          emberProcessCommandString((uint8_t*)"\n", strlen("\n"));

          emberProcessCommandString((uint8_t*)yh_send, strlen(yh_send));
          emberProcessCommandString((uint8_t*)"\n", strlen("\n"));
          break;

        default:
          return;
      }



  }else if( usartData[0]==YH_ZCL_HEAD &&  usartData[4] == 0 &&  usartData[5] == 0){

      yhInfraredData.nodeId = usartData[1];
      yhInfraredData.nodeId |= usartData[2]<<8;
      yhInfraredData.clusterId = usartData[4];
      yhInfraredData.clusterId |= usartData[5];
      yhInfraredData.commandId = usartData[6];
      yhInfraredData.attrId = usartData[7];
      yhInfraredData.attrId |= usartData[8]<<8;
      yhInfraredData.type = usartData[9];
      yhInfraredData.length = usartData[10];
      memcpy(yhInfraredData.data, &usartData[11],yhInfraredData.length);

      yhZclGlobalWriteCommand(yhInfraredData.type, yhInfraredData.commandId, yhInfraredData.clusterId,\
                              yhInfraredData.attrId, yhInfraredData.data, yhInfraredData.length);
      yh_hex_obj[0] = yhInfraredData.nodeId>>8;
      yh_hex_obj[1] = yhInfraredData.nodeId;
      bin2str(yh_obj,yh_hex_obj,2);
      memcpy(&yh_send[7],yh_obj,4);
      emberProcessCommandString((uint8_t*)yh_send, strlen(yh_send));
      emberProcessCommandString((uint8_t*)"\n", strlen("\n"));

  }



}

//间隔100ms从串口接收的环形队列中提取数据
//并发送相应的RF命令到设备端
void popRingBufferOneFrame(void)
{

    int16u time;
    time = halCommonGetInt16uMillisecondTick();
    if (elapsedTimeInt16u(lastSerialRxTime, time) > RF_TX_TIMEOUT)
    {
        lastSerialRxTime = time;
        if(readRingBuffer(&protocolRingBuffer.header))
        {
           if(readRingBuffer(&protocolRingBuffer.length))
           {
              readRingBuffer(&protocolRingBuffer.cmd0);
              readRingBuffer(&protocolRingBuffer.cmd1);
              popRingBufferData(&protocolRingBuffer.data[0],protocolRingBuffer.length + 1);//+1 is fcs
              //for test print output one frame
              //emberSerialWriteData(PROTOCAL_SERIAL_PORT,
              //                     &protocolRingBuffer.header,
              //                     protocolRingBuffer.length + 5);//5 == header length cmd0 cmd1 fcs
              //emberSerialWaitSend(PROTOCAL_SERIAL_PORT);
              //
              switch(protocolRingBuffer.cmd1)
              {
                 case  GW_QUERY_CONTROL_POINT_VALE:
                       rfSendCommandQueryControlPointValue();
                       break;
                 case  GW_CONTROL_CONTROL_POINT:
                       rfSendCommandCtrlControlPoint();
                       break;
                 case  GW_CONTROL_CONTROL_POINT_BROAD_CAST:
                       {
                         rfSendBroadcastCommandCtrlControlPoint();
                       }
                       break;

                 case  GW_UPDATE_PANNEL:
                       rfSendCommandUpdatePannel();
                       break;
                 //case  GW_UPDATE_PANNEL_SCENE:
                 //      rfSendCommandUpdatePannelScene();
                 //      break;
                 case  GW_DIRECT_TX:
                       rfSendCommandDirectTx();
                       break;
                 case GW_OTA_NOTIFY:
                        rfNotifyDeviceOTA();
                        break;
                 
                 //case GW_IMAGE_NOTIFY:
                  //      serialSendImageRequest(0);
                  //     break;
                 default:
                       break;
              }
           }
        }
    }
}
