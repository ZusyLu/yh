#include PLATFORM_HEADER
#include "app/framework/util/common.h"
#include "app/framework/include/af.h"
#include "gw-cfg-tokens.h"
#include "serial-rx-task.h"
#include "serial-rx-queue.h"
#include "rf-tx-command.h"
#include "serial-ota-task.h"
#include "protocol-parsing.h"

protocolType protocolData;
cmdDataType cmdData;
uint8_t protocolBuff[PROTOCOL_BUFF_MAX_LEN];
uint8_t sendSeq=0;

//
extern SerialRxDateStruct  serialbuffer;
extern yhNodeIDType yhNodeId;
void ParseProtocolCmdFunction(uint16_t cmdType, uint8_t *data, uint16_t len);
uint8_t calcSum(uint8_t *src, uint16_t len)
{
    uint32_t sum = 0;
    uint16_t i;
    for(i = 0; i < len; i++){
        sum += src[i];
    }
    return (sum%256);
}


void protocolFramingFunction(uint8_t version,uint8_t seq, uint8_t framType,uint16_t dataLen,uint8_t *framData)
{
    if(dataLen > (PROTOCOL_BUFF_MAX_LEN - PROTOCOL_BUFF_MIN_LEN) )
    {
        return;
    }

    protocolBuff[PROTOCOL_HEADER_POSITION] = PROTOCOL_HEADER;
    protocolBuff[PROTOCOL_VERSION_POSITION] = version;
    protocolBuff[PROTOCOL_SEQ_POSITION] = seq;
    protocolBuff[PROTOCOL_FRAM_TYPE_POSITION] = framType;
    protocolBuff[PROTOCOL_FRAM_LEN_H_POSITION] = dataLen>>8;
    protocolBuff[PROTOCOL_FRAM_LEN_L_POSITION] = dataLen;
    memmove(&protocolBuff[PROTOCOL_FRAM_DATA_POSITION], framData, dataLen);
    protocolBuff[PROTOCOL_FRAM_DATA_POSITION + dataLen] = calcSum(protocolBuff, dataLen + PROTOCOL_BUFF_MIN_LEN-1);

    emberSerialWriteData(PROTOCAL_SERIAL_PORT,protocolBuff,dataLen + PROTOCOL_BUFF_MIN_LEN);
    emberSerialWaitSend(PROTOCAL_SERIAL_PORT);

    sendSeq++;
}

void protocolDataPushRingBuffer(uint8_t version,uint8_t seq, uint8_t framType,uint16_t dataLen,uint8_t *framData)
{
    if(dataLen > (PROTOCOL_BUFF_MAX_LEN - PROTOCOL_BUFF_MIN_LEN) )
    {
        return;
    }

    protocolBuff[PROTOCOL_HEADER_POSITION] = PROTOCOL_HEADER;
    protocolBuff[PROTOCOL_VERSION_POSITION] = version;
    protocolBuff[PROTOCOL_SEQ_POSITION] = seq;
    protocolBuff[PROTOCOL_FRAM_TYPE_POSITION] = framType;
    protocolBuff[PROTOCOL_FRAM_LEN_H_POSITION] = dataLen>>8;
    protocolBuff[PROTOCOL_FRAM_LEN_L_POSITION] = dataLen;
    memmove(&protocolBuff[PROTOCOL_FRAM_DATA_POSITION], framData, dataLen);
    protocolBuff[PROTOCOL_FRAM_DATA_POSITION + dataLen] = calcSum(protocolBuff, dataLen + PROTOCOL_BUFF_MIN_LEN-1);

    rfWriteRingBuffer(dataLen+PROTOCOL_BUFF_MIN_LEN);
    rfPushBufferToRing(protocolBuff,dataLen+PROTOCOL_BUFF_MIN_LEN);

    sendSeq++;
}

void ParseProtocolFramingFunction(void)
{
    if(serialbuffer.buffer[0] != PROTOCOL_HEADER || \
       serialbuffer.length > (PROTOCOL_BUFF_MAX_LEN - PROTOCOL_BUFF_MIN_LEN))
    {
        return;
    }
    emberAfCorePrintln("cmdData.cmd = %2x ",cmdData.cmd);
    if(calcSum(&serialbuffer.buffer[0],serialbuffer.length-1)\
               != serialbuffer.buffer[serialbuffer.length-1] )
    {
        protocolFramingFunction(PROTOCOL_VERSION,sendSeq, PROTOCOL_ERROR_FRAM_TYPE,0,NULL);
        return;
    }

    uint8_t i=0;
    memmove(&protocolData.head, &serialbuffer.buffer[0], serialbuffer.length);
    cmdData.cmd = protocolData.data[0]<<8;
    cmdData.cmd |= protocolData.data[1];
    protocolData.dataLen = (serialbuffer.buffer[4]<<8) | serialbuffer.buffer[5];
    memmove(&cmdData.data[0], &protocolData.data[2], protocolData.dataLen-2);

    emberAfCorePrintln("protocolData.head = %x ",protocolData.head);
    emberAfCorePrintln("protocolData.version = %x ",protocolData.version);
    emberAfCorePrintln("protocolData.seq = %x ",protocolData.seq);
    emberAfCorePrintln("protocolData.framType = %x ",protocolData.framType);
    emberAfCorePrintln("protocolData.dataLen = %2x ",protocolData.dataLen);
    emberAfCorePrintln("cmdData.cmd = %2x ",cmdData.cmd);
   // emberAfCorePrint("cmdData.data[i] = ");
   // for(i=0;i<protocolData.dataLen-2;i++)
   // {
    //    emberAfCorePrintln("%x ",cmdData.data[i]);
   // }
    emberAfCorePrintln("");
    switch(protocolData.framType)
    {
        case PROTOCOL_NORMAL_FRAM_TYPE:
             {
                 protocolFramingFunction(PROTOCOL_VERSION,protocolData.seq, PROTOCOL_NORMAL_ACK_FRAM_TYPE,0,NULL);
                 ParseProtocolCmdFunction(cmdData.cmd, &cmdData.data[0],protocolData.dataLen-2);
             }
             break;

        case PROTOCOL_NORMAL_ACK_FRAM_TYPE:
             {

             }
             break;

        case PROTOCOL_ERROR_FRAM_TYPE:
             {

             }
             break;

        default:
              {
                  break;
              }
    }
}

void ParseProtocolCmdFunction(uint16_t cmdType, uint8_t *data, uint16_t len)
{
    switch(cmdType)
    {
        case PROTOCOL_PENETRATE_CMD:
             {
                 yhNwkSendRfPacket(1, data[0], (uint16_t)( data[1]<<8|data[2] ), data[3], (uint16_t)( data[5]<<8|data[6] ),\
                                   data[4], &data[8], data[7]);
             }
             break;

        case PROTOCOL_OPEN_CLOSE_NET_CMD:
             {
                 if(*data == true)
                 {
                     emberAfPluginNetworkCreatorSecurityOpenNetwork();
                 }
                 else
                 {
                     emberAfPluginNetworkCreatorSecurityCloseNetwork();
                 }
             }
             break;

        case PROTOCOL_REQUEST_NODE_LEAVE_NET_CMD:
             {
                 yhNodeId.nodeId = (data[0]<<8)|data[1];
                 yhZdoLeaveRequestCommand(yhNodeId.nodeId);
             }
             break;

        case PROTOCOL_NETWORK_ADDRESS_REQUEST:
             {
                 EmberEUI64 eui64;
                 reverseHex(eui64, &data[0], 8);
                 emberAfFindNodeId(eui64,NULL);//emAfCliServiceDiscoveryCallback
             }
             break;

        case PROTOCOL_IEEE_ADDRESS_REQUEST:
             {
                 yhNodeId.nodeId = (data[0]<<8)|data[1];
                 yhZdoIeeeAddressRequestCommand(yhNodeId.nodeId);
             }
             break;

        case PROTOCOL_INFRARED_SET_CMD:
             {
                 yhZclGlobalWriteCommand(0X42, 0X02, 0X0000,\
                                          0X410C, &data[3], data[2]);
                 yh_hex_obj[0] = data[0];
                 yh_hex_obj[1] = data[1];
                 bin2str(yh_obj,yh_hex_obj,2);
                 memcpy(&yh_send[7],yh_obj,4);
                 emberProcessCommandString((uint8_t*)yh_send, strlen(yh_send));
                 emberProcessCommandString((uint8_t*)"\n", strlen("\n"));
             }
             break;

        case PROTOCOL_DOOR_LOCK_CMD:
             {
                 yhZclGlobalWriteCommand(0X42, 0X02, 0X0000,\
                                          0X4200, &data[3], data[2]);
                 yh_hex_obj[0] = data[0];
                 yh_hex_obj[1] = data[1];
                 bin2str(yh_obj,yh_hex_obj,2);
                 memcpy(&yh_send[7],yh_obj,4);
                 emberProcessCommandString((uint8_t*)yh_send, strlen(yh_send));
                 emberProcessCommandString((uint8_t*)"\n", strlen("\n"));
             }
             break;

        case PROTOCOL_CURTAIN_CMD:
             {
                  yhNodeId.nodeId = data[0]<<8 | data[1];
                  switch(data[2])
                  {
                    case YH_WINDOW_OPEN:
                        {
                            emberAfFillCommandWindowCoveringClusterWindowCoveringUpOpen();
                            emberAfSetCommandEndpoints(1,1);
                            emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
                        }
                        break;

                    case YH_WINDOW_CLOSE:
                        {
                            emberAfFillCommandWindowCoveringClusterWindowCoveringDownClose();
                            emberAfSetCommandEndpoints(1,1);
                            emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
                        }
                        break;

                    case YH_WINDOW_STOP:
                        {
                            emberAfFillCommandWindowCoveringClusterWindowCoveringStop();
                            emberAfSetCommandEndpoints(1,1);
                            emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
                        }
                        break;

                    case YH_WINDOW_LIFT_PERCENTAGE:
                        {
                            emberAfFillCommandWindowCoveringClusterWindowCoveringGoToLiftPercentage(data[3])
                            emberAfSetCommandEndpoints(1,1);
                            emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, yhNodeId.nodeId);
                        }
                        break;
                  }
             }
             break;

        default:
              {

              }
              break;
    }
}

uint8_t *reverseHex(uint8_t *dest, uint8_t *source, uint8_t len)
{
  uint8_t i=0;
  for(i=0;i<len;i++)
  {
    dest[i] = source[len-1-i];
  }

  return dest;
}

