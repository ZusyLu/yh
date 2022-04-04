#include PLATFORM_HEADER
#include "app/framework/util/common.h"
#include "app/framework/include/af.h"
#include <stdint.h>
#include "serial-rx-task.h"
#include "serial-rx-queue.h"

//ll
#include"gw-cfg-tokens.h"
/*------------------------------------------------
 ��C�ļ��Ĺ��������ط���RF֡���豸�Ĵ������
ÿ������֡��Ӧһ������
__________________________________________________*/

ProtocolDataStruct rfProtocolTxBuffer;//rf tx buffer
uint8_t  rfCommandSetup(uint16_t dstAddr,uint8_t sendMode,
                     uint8_t *buffer,uint8_t bufferlen)
{
    uint8_t status;
    if(bufferlen > 80) return EMBER_BAD_ARGUMENT;
#if 0 //���öԷ��ظ�default response
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
          status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,dstAddr);
      }
      else if(sendMode == RF_SEND_MODE_BROADCASE)
      {
          status = emberAfSendCommandBroadcast(0xffff);
      }
      return status;
}
//�Ӵ�������֡�л�ȡ���͵�Ŀ���ַ��NWK Addr��
uint16_t getDstAddrFromRingBuffer(void)
{
     uint16_t dst;
     uint16_t temp;
     temp = (uint16_t)protocolRingBuffer.data[9];
     dst = (uint16_t)protocolRingBuffer.data[8] + (temp << 8);
     return dst;
}
//�Ӵ�������֡�л�ȡ���ݶεĳ��ȣ�data ���ֵĳ��ȣ�
uint8_t getDataLengthFromRingBuffer(void)
{
     uint8_t length;
     length = protocolRingBuffer.length - 10;  //10 = addr length
     return length;
}
//����RF���Ͳ�ѯ�豸�Ŀ��Ƶ��ֵ
//����ID:RF_QUERY_CONTROL_POINT_VALE
void rfSendCommandQueryControlPointValue(void)
{
   uint16_t dstAddr;
   dstAddr = getDstAddrFromRingBuffer();
   rfProtocolTxBuffer.header = FRAME_HEADER;
   rfProtocolTxBuffer.length = 0x02;
   rfProtocolTxBuffer.cmd0 = FRAME_CMD0;
   rfProtocolTxBuffer.cmd1 = RF_QUERY_CONTROL_POINT_VALE;
   rfProtocolTxBuffer.data[0] = protocolRingBuffer.data[10];
   rfProtocolTxBuffer.data[1] =  protocolRingBuffer.data[11];
   rfProtocolTxBuffer.fcs = checkSum(&rfProtocolTxBuffer.header,
                                     rfProtocolTxBuffer.length + DATA_OFFSET);
   rfProtocolTxBuffer.rfLength = rfProtocolTxBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   rfProtocolTxBuffer.data[2] = rfProtocolTxBuffer.fcs;
   //
   rfCommandSetup(dstAddr,0,&rfProtocolTxBuffer.rfLength,rfProtocolTxBuffer.rfLength+1);
}
//����RF���Ϳ��ƿ��Ƶ�����
//����ID:RF_GW_CONTROL_CONTROL_POINT
void rfSendCommandCtrlControlPoint(void)
{
   uint16_t dstAddr;
    uint8_t cpListCount;
   uint8_t cpListIndex;
   dstAddr = getDstAddrFromRingBuffer();
   cpListCount = protocolRingBuffer.data[10];
   rfProtocolTxBuffer.header = FRAME_HEADER;
  // rfProtocolTxBuffer.length = 0x04;
   rfProtocolTxBuffer.length = 1 + 4*cpListCount ;
   rfProtocolTxBuffer.cmd0 = FRAME_CMD0;
   rfProtocolTxBuffer.cmd1 = RF_GW_CONTROL_CONTROL_POINT;
   rfProtocolTxBuffer.data[0] = protocolRingBuffer.data[10];
   
    for(cpListIndex = 0;cpListIndex < cpListCount;cpListIndex++)// cpList code and value
   {
      rfProtocolTxBuffer.data[1+4*cpListIndex] =  protocolRingBuffer.data[11 + 4*cpListIndex];
      rfProtocolTxBuffer.data[2+4*cpListIndex] =  protocolRingBuffer.data[12 + 4*cpListIndex];
      rfProtocolTxBuffer.data[3+4*cpListIndex] =  protocolRingBuffer.data[13 + 4*cpListIndex];
      rfProtocolTxBuffer.data[4+4*cpListIndex] =  protocolRingBuffer.data[14 + 4*cpListIndex];
   }
   cpListIndex = cpListIndex-1;
   
  // rfProtocolTxBuffer.data[1] =  protocolRingBuffer.data[11];
 //  rfProtocolTxBuffer.data[2] =  protocolRingBuffer.data[12];
  // rfProtocolTxBuffer.data[3] =  protocolRingBuffer.data[13];
   rfProtocolTxBuffer.fcs = checkSum(&rfProtocolTxBuffer.header,
                                     rfProtocolTxBuffer.length + DATA_OFFSET);
   rfProtocolTxBuffer.rfLength = rfProtocolTxBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
    rfProtocolTxBuffer.data[5+4*cpListIndex] = rfProtocolTxBuffer.fcs;
  // rfProtocolTxBuffer.data[4] = rfProtocolTxBuffer.fcs;
   //
   rfCommandSetup(dstAddr,0,&rfProtocolTxBuffer.rfLength,rfProtocolTxBuffer.rfLength+1);
}

//add 20220116
void rfSendBroadcastCommandCtrlControlPoint(void)
{
   uint16_t dstAddr;
    uint8_t cpListCount;
   uint8_t cpListIndex;
   dstAddr = getDstAddrFromRingBuffer();
   cpListCount = protocolRingBuffer.data[10];
   rfProtocolTxBuffer.header = FRAME_HEADER;
  // rfProtocolTxBuffer.length = 0x04;
   rfProtocolTxBuffer.length = 1 + 4*cpListCount ;
   rfProtocolTxBuffer.cmd0 = FRAME_CMD0;
   rfProtocolTxBuffer.cmd1 = RF_GW_CONTROL_CONTROL_POINT;
   rfProtocolTxBuffer.data[0] = protocolRingBuffer.data[10];

    for(cpListIndex = 0;cpListIndex < cpListCount;cpListIndex++)// cpList code and value
   {
      rfProtocolTxBuffer.data[1+4*cpListIndex] =  protocolRingBuffer.data[11 + 4*cpListIndex];
      rfProtocolTxBuffer.data[2+4*cpListIndex] =  protocolRingBuffer.data[12 + 4*cpListIndex];
      rfProtocolTxBuffer.data[3+4*cpListIndex] =  protocolRingBuffer.data[13 + 4*cpListIndex];
      rfProtocolTxBuffer.data[4+4*cpListIndex] =  protocolRingBuffer.data[14 + 4*cpListIndex];
   }
   cpListIndex = cpListIndex-1;

  // rfProtocolTxBuffer.data[1] =  protocolRingBuffer.data[11];
 //  rfProtocolTxBuffer.data[2] =  protocolRingBuffer.data[12];
  // rfProtocolTxBuffer.data[3] =  protocolRingBuffer.data[13];
   rfProtocolTxBuffer.fcs = checkSum(&rfProtocolTxBuffer.header,
                                     rfProtocolTxBuffer.length + DATA_OFFSET);
   rfProtocolTxBuffer.rfLength = rfProtocolTxBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
    rfProtocolTxBuffer.data[5+4*cpListIndex] = rfProtocolTxBuffer.fcs;
  // rfProtocolTxBuffer.data[4] = rfProtocolTxBuffer.fcs;
   //
   rfCommandSetup(dstAddr,1,&rfProtocolTxBuffer.rfLength,rfProtocolTxBuffer.rfLength+1);
}

//����RF����ͬ�������Ƶ���Ϣ
//����ID:RF_GW_UPDATE_PANNEL
void rfSendCommandUpdatePannel(void)
{
   uint16_t dstAddr;
   dstAddr = getDstAddrFromRingBuffer();
   rfProtocolTxBuffer.header = FRAME_HEADER;
   rfProtocolTxBuffer.length = 0x04;
   rfProtocolTxBuffer.cmd0 = FRAME_CMD0;
   rfProtocolTxBuffer.cmd1 = RF_GW_UPDATE_PANNEL;
   rfProtocolTxBuffer.data[0] = protocolRingBuffer.data[10];
   rfProtocolTxBuffer.data[1] =  protocolRingBuffer.data[11];
   rfProtocolTxBuffer.data[2] =  protocolRingBuffer.data[12];
   rfProtocolTxBuffer.data[3] =  protocolRingBuffer.data[13];
   rfProtocolTxBuffer.fcs = checkSum(&rfProtocolTxBuffer.header,
                                     rfProtocolTxBuffer.length + DATA_OFFSET);
   rfProtocolTxBuffer.rfLength = rfProtocolTxBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   rfProtocolTxBuffer.data[4] = rfProtocolTxBuffer.fcs;
   //
   rfCommandSetup(dstAddr,0,&rfProtocolTxBuffer.rfLength,rfProtocolTxBuffer.rfLength+1);
}
//����RF����ͬ����峡��ģʽ������
//����ID:RF_GW_UPDATE_PANNEL_SCENE
void rfSendCommandUpdatePannelScene(void)
{
   uint16_t dstAddr;
   dstAddr = getDstAddrFromRingBuffer();
   rfProtocolTxBuffer.header = FRAME_HEADER;
   rfProtocolTxBuffer.length = 0x01;
   rfProtocolTxBuffer.cmd0 = FRAME_CMD0;
   rfProtocolTxBuffer.cmd1 = RF_GW_UPDATE_PANNEL_SCENE;
   rfProtocolTxBuffer.data[0] = protocolRingBuffer.data[10];
   rfProtocolTxBuffer.fcs = checkSum(&rfProtocolTxBuffer.header,
                                     rfProtocolTxBuffer.length + DATA_OFFSET);
   rfProtocolTxBuffer.rfLength = rfProtocolTxBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   rfProtocolTxBuffer.data[1] = rfProtocolTxBuffer.fcs;
   //
   rfCommandSetup(dstAddr,0,&rfProtocolTxBuffer.rfLength,rfProtocolTxBuffer.rfLength+1);
}


//����ͨ��RF֪ͨ�豸OTA����
void rfNotifyDeviceOTA(void)
{
   emberSerialPrintf(1,"\r\n cmd is %d",protocolRingBuffer.data[0]);
   halCommonGetToken(&otaInfo,TOKEN_MODULE_OTA_INFO);
    EmberAfOtaImageId id;
   id.manufacturerId=otaInfo.manuId;
   id.firmwareVersion=otaInfo.version;
   id.imageTypeId=otaInfo.imageType;
    emberAfOtaServerSendImageNotifyCallback(protocolRingBuffer.data[0],1,3,48,&id); //֪ͨ�豸����OTA����
}
//����RF͸������
//����ID:RF_GW_DIRECT_TX
/*----------------------------------
1�����ط��͸�ʽ
Ŀ���豸��MAC��ַ + Ŀ���豸�������ַ +͸������
exampel:
7C ---------header
11 ---------length
01 ---------cmd0
40 ---------cmd1
AE 26 47 FE FF 57 0B 00 -------MAC Addr
01 00  -------NWK Addr
01 02 03 04 05 06 07 ---------͸������
65                 ------------У��

2���豸�ķ��͸�ʽ
ֱ�����͸������
7C  ---------header
07  ---------length
01  ---------cmd0
40  ---------cmd1
01 02 03 04 05 06 07 ---------͸������
E0           ------------У��
------------------------------------*/
void rfSendCommandDirectTx(void)
{
   uint16_t dstAddr;
   uint8_t  dataIndex = 0;
   dstAddr = getDstAddrFromRingBuffer();
   rfProtocolTxBuffer.header = FRAME_HEADER;
   rfProtocolTxBuffer.length = getDataLengthFromRingBuffer();
   rfProtocolTxBuffer.cmd0 = FRAME_CMD0;
   rfProtocolTxBuffer.cmd1 = RF_GW_DIRECT_TX;
   for(dataIndex = 0;dataIndex < rfProtocolTxBuffer.length;dataIndex++)
   {
      rfProtocolTxBuffer.data[dataIndex] = protocolRingBuffer.data[10 + dataIndex];
   }
   rfProtocolTxBuffer.fcs = checkSum(&rfProtocolTxBuffer.header,
                                     rfProtocolTxBuffer.length + DATA_OFFSET);
   rfProtocolTxBuffer.rfLength = rfProtocolTxBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   rfProtocolTxBuffer.data[dataIndex] = rfProtocolTxBuffer.fcs;
   //
   rfCommandSetup(dstAddr,0,&rfProtocolTxBuffer.rfLength,rfProtocolTxBuffer.rfLength+1);
}
//��������
EmberStatus rfSendCommandDirectTxTestCommand(uint16_t dstAddr)
{
   uint8_t  dataIndex = 0;
   uint8_t status;
   rfProtocolTxBuffer.header = FRAME_HEADER;
   rfProtocolTxBuffer.length = 20;
   rfProtocolTxBuffer.cmd0 = FRAME_CMD0;
   rfProtocolTxBuffer.cmd1 = RF_GW_DIRECT_TX;
   for(dataIndex = 0;dataIndex < rfProtocolTxBuffer.length;dataIndex++)
   {
      rfProtocolTxBuffer.data[dataIndex] = dataIndex + 1;
   }
   rfProtocolTxBuffer.fcs = checkSum(&rfProtocolTxBuffer.header,
                                     rfProtocolTxBuffer.length + DATA_OFFSET);
   rfProtocolTxBuffer.rfLength = rfProtocolTxBuffer.length + FULL_BUFFER_MIN_LENGTH;
    //fcs for end data
   rfProtocolTxBuffer.data[dataIndex] = rfProtocolTxBuffer.fcs;
   //
   status = rfCommandSetup(dstAddr,0,&rfProtocolTxBuffer.rfLength,rfProtocolTxBuffer.rfLength+1);
   return status;
}
