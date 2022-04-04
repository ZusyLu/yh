#include PLATFORM_HEADER
#include "app/framework/util/common.h"
#include "app/framework/include/af.h"
//#include "gw-cfg-tokens.h"
#include "serial-rx-task.h"
#include "serial-rx-queue.h"
//#include "rf-tx-command.h"
//#include "serial-ota-task.h"
//#include "eeprom.h"
/*------------------------------------------------
 ��C�ļ��Ĺ��ܰ������¼������֣�
1����������Э�鲿�ֹ���ʵ��
2������HOST�˴������ݵĻ��ζ��н��մ���
3����������HOST�˴������ͨ��RF���Ͷ�Ӧ��ָ��豸��
__________________________________________________*/

#define  OWN_SERIAL_RX_TASK   //

//#define  SERIAL_NACK

#define  SERIAL_RX_TIMEOUT     8
#define  SERIAL_BUFFER_LENGTH  80
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
#define  PROTOCAL_SERIAL_PORT          0x22       //change by lu 1 to 0x22
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
//add yb yh 20210720
#define  YH_CREATE_30NETWORK        0XA1
#define  YH_OPEN_30NETWORK          0XA2
#define  YH_CLOSE_30NETWORK         0XA3
#define  YH_LEAVE_30NETWORK         0XA4
#define  YH_DATA_30NETWORK          0XA5
#define  YH_READ_YH_VERSION          0XA6

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
#define    YH_WINDOW_OPEN                     0X00
#define    YH_WINDOW_CLOSE                    0X01
#define    YH_WINDOW_STOP                     0X02
#define    YH_WINDOW_LIFT_PERCENTAGE          0X05
#define    YH_WINDOW_TILT_PERCENTAGE          0X08
#define    YH_WINDOW_LIFT_VALUE               0X04
#define    YH_WINDOW_TILT_VALUE               0X07

//yh
#define   YH_ZCL_HEAD                         0X51
#define   TH_ZCL_TAIL                         0x15
#define   YH_TEMP_LEN                         150

static uint8_t  yhTemp[YH_TEMP_LEN]={0};

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

typedef union{

  int8u   data[4];
  int32u  linkKey;

}yhNetwork30LinkKeyType;

typedef union{
  int8u data[2];
  int16u nodeId;
}yhNodeIDType;

typedef union{
  int8u data[2];
  int16u clusterId;
}yhClusterIDType;

//
SerialRxDateStruct  serialbuffer;
SerialTxDateStruct  serialTxBuffer;

const  unsigned  char aucCRCHi[] = {

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

   0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

   0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

   0x00, 0xC1, 0x81, 0x40

};

const unsigned char aucCRCLo[] = {

   0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,

   0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,

   0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,

   0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,

   0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,

   0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,

   0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,

   0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,

   0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,

   0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,

   0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,

   0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,

   0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,

   0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,

   0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,

   0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,

   0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,

   0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,

   0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,

   0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,

   0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,

   0x41, 0x81, 0x80, 0x40

};
uint16_t getCRC(uint8_t* buf,int len)
{
  unsigned char ucCRCHi = 0xFF;
  unsigned char ucCRCLo = 0xFF;
  int iIndex;
  int i = 0;
  for(i = 0;i < len;i++)
  {

          iIndex = ucCRCLo ^ buf[i];

          ucCRCLo = ( unsigned char )( ucCRCHi ^ aucCRCHi[iIndex] );

          ucCRCHi = aucCRCLo[iIndex];

   }
  return ((uint16_t)ucCRCHi << 8 | (uint16_t)ucCRCLo);

}

void serialCommandParse(void);

//
ProtocolDataStruct protocolRxBuffer; //
ProtocolDataStruct protocolRingBuffer;
yhDebugProtocolDataStruct yhProtocolRingBuffer;
uint16_t lastSerialRxTime = 0;


//Function



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
           serialbuffer.length=0;

      }
      else if(emberSerialReadByte(Port, &ch) == EMBER_SUCCESS)
      {
           lastTime = time;
           serialbuffer.buffer[serialbuffer.length] = ch;
           serialbuffer.length ++;
      }

}

//yh

void serialCommandParse()
{
  EmberPanId yhTargetPanid;
  uint8_t yhTargetLongid[8];
  uint8_t i=0;
  if(serialbuffer.length == 0)
  {
      return;
  }
  emberAfCorePrintln("usart data = ");
  for(i=0;i<serialbuffer.length;i++)
  {
      emberAfCorePrint("%x",serialbuffer.buffer[i]);
  }
  emberAfCorePrintln(" ");

  if(serialbuffer.buffer[0] == 0xf0 && serialbuffer.buffer[1] == 0x0f)
  {
      //void HTemAfInterpanFragmentTestCommand(EmberPanId htPanId,uint8_t *htMacId , \
  uint16_t HTclusterId,uint8_t *data,uint16_t HTmessageLen)
      pushBufferToRing(&serialbuffer.buffer[0],serialbuffer.length);

  }else{
      //other command
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


//间隔100ms从串口接收的环形队列中提取数据
//并发送相应的RF命令到设备端
void popRingBufferOneFrame(void)
{

    int16u time;
    time = halCommonGetInt16uMillisecondTick();
    if (elapsedTimeInt16u(lastSerialRxTime, time) > RF_TX_TIMEOUT)
    {
        lastSerialRxTime = time;
        if(readRingBuffer( &yhProtocolRingBuffer.header.heads[0] )\
            &&  readRingBuffer( &yhProtocolRingBuffer.header.heads[1] ) )
        {
           if(readRingBuffer(&yhProtocolRingBuffer.version))
           {
              readRingBuffer(&yhProtocolRingBuffer.cmdid.cm[1]);
              readRingBuffer(&yhProtocolRingBuffer.cmdid.cm[0]);
              readRingBuffer(&yhProtocolRingBuffer.functionCode);
              readRingBuffer( &yhProtocolRingBuffer.length.lens[1] );
              readRingBuffer(&yhProtocolRingBuffer.length.lens[0] );
              popRingBufferData(&yhProtocolRingBuffer.tarLongId[0],8);//
              readRingBuffer(&yhProtocolRingBuffer.tarPanId.pans[1]);
              readRingBuffer(&yhProtocolRingBuffer.tarPanId.pans[0] );
              popRingBufferData(&yhProtocolRingBuffer.data[0],yhProtocolRingBuffer.length.len-10 );//
              readRingBuffer(&yhProtocolRingBuffer.crc.crc8[1]);
              readRingBuffer(&yhProtocolRingBuffer.crc.crc8[0] );

              emberAfCorePrintln("yhProtocolRingBuffer.cmdid.cmd =%2x ",yhProtocolRingBuffer.cmdid.cmd);
              emberAfCorePrintln("yhProtocolRingBuffer.length.len =%2x ",yhProtocolRingBuffer.length.len);
              emberAfCorePrintln("yhProtocolRingBuffer.functionCode =%x ",yhProtocolRingBuffer.functionCode);
              emberAfCorePrintln("yhProtocolRingBuffer.tarPanId.panid =%2x ",yhProtocolRingBuffer.tarPanId.panid);
              emberAfCorePrint("yhProtocolRingBuffer.tarLongId=");
              for(uint8_t i=0;i<8;i++)
              {
                  emberAfCorePrint("%x",yhProtocolRingBuffer.tarLongId[i]);
              }
              emberAfCorePrintln(" ");

              emberAfCorePrint("yhProtocolRingBuffer.data=");
              for(uint8_t i=0;i<yhProtocolRingBuffer.length.len-10;i++)
              {
                  emberAfCorePrint("%x ",yhProtocolRingBuffer.data[i]);
              }
              emberAfCorePrintln(" ");

              emberAfCorePrintln("yhProtocolRingBuffer.crc.crc16 =%2x ",yhProtocolRingBuffer.crc.crc16);


              memcpy(yhTemp,&yhProtocolRingBuffer.header.heads[0],1);
              memcpy(yhTemp+1,&yhProtocolRingBuffer.header.heads[1],1);
              memcpy(yhTemp+2,&yhProtocolRingBuffer.version,1);

              memcpy(yhTemp+2+1,&yhProtocolRingBuffer.cmdid.cm[1],1);
              memcpy(yhTemp+2+1+1,&yhProtocolRingBuffer.cmdid.cm[0],1);

              memcpy(yhTemp+2+1+1+1,&yhProtocolRingBuffer.functionCode,1);

              memcpy(yhTemp+2+1+1+1+1,&yhProtocolRingBuffer.length.lens[1],1);
              memcpy(yhTemp+2+1+1+1+1+1,&yhProtocolRingBuffer.length.lens[0],1);

              memcpy(yhTemp+2+1+1+1+1+1+1,yhProtocolRingBuffer.tarLongId,8);
              memcpy(yhTemp+2+1+1+1+1+1+1+8,&yhProtocolRingBuffer.tarPanId.pans[1],1);
              memcpy(yhTemp+2+1+1+1+1+1+1+8+1,&yhProtocolRingBuffer.tarPanId.pans[0],1);

              memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1,yhProtocolRingBuffer.data,2);
              memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2,&yhProtocolRingBuffer.data[2],yhProtocolRingBuffer.data[1]);//LEN <100

              memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1],&yhProtocolRingBuffer.crc.crc8[1],1);
              memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]+1,&yhProtocolRingBuffer.crc.crc8[0],1);

              emberAfCorePrint("\r\n yhTemp[] = ");
              for(uint8_t i=0;i<(2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]);i++)
              {
                  emberAfCorePrint("%x ",yhTemp[i]);
              }
              emberAfCorePrintln(" ");
              uint16_t yhcrc = getCRC(yhTemp,2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]);
              emberAfCorePrint("\r\n yhcrc =%2x ",yhcrc);
              emberAfCorePrint("\r\ functionCode = %X  crc len= %d",yhProtocolRingBuffer.functionCode,(2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]) );
              if( yhProtocolRingBuffer.crc.crc16 != yhcrc )
              {
                  emberAfCorePrint("\r\n CRC error");
                  return;
              }else{
                  emberAfCorePrint("\r\n CRC ok");
              }

              switch(yhProtocolRingBuffer.functionCode)//function code
              {
                case YH_SET_CHANNEL:
                {
                  if(yhProtocolRingBuffer.data[2]<11 || yhProtocolRingBuffer.data[2]>26) //11~26 is zb channel
                  {
                      break;
                  }

                  if(yhChangeChannel(yhProtocolRingBuffer.data[2]) == 0x00) //success
                  {
                      yhTemp[5] = 0x01;
                      yhProtocolRingBuffer.crc.crc16=getCRC(yhTemp,2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]);
                      memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1],&yhProtocolRingBuffer.crc.crc8[1],1);
                      memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]+1,&yhProtocolRingBuffer.crc.crc8[0],1);
                      emberSerialWriteData(HAL_SERIAL_PORT_USART2, yhTemp, yhProtocolRingBuffer.length.len+10);//10 is len
                      emberSerialWaitSend(HAL_SERIAL_PORT_USART2);
                  }

                  break;
                }

                case YH_SET_CHANNEL_RESULT:
                {
                  break;
                }

                case YH_SCANNING_DEVICE:
                {
                   HTemAfInterpanFragmentTestCommand( (EmberPanId)yhProtocolRingBuffer.tarPanId.panid,\
                                                          yhProtocolRingBuffer.tarLongId, \
                                                         0xfc88,\
                                                         yhProtocolRingBuffer.data,\
                                                         yhProtocolRingBuffer.length.len-10);
                  break;
                }

                case YH_TARGET_DEVICE_INFORMATION:
                {
                  break;
                }

                case YH_SET_TARGET_PARAMETERS:
                {
                  HTemAfInterpanFragmentTestCommand( (EmberPanId)yhProtocolRingBuffer.tarPanId.panid,\
                                                         yhProtocolRingBuffer.tarLongId, \
                                                        0xfc88,\
                                                        yhProtocolRingBuffer.data,\
                                                        yhProtocolRingBuffer.length.len-10);
                  break;
                }

                case YH_RAW_OUT:
                {
                  HTemAfInterpanFragmentTestCommand( (EmberPanId)yhProtocolRingBuffer.tarPanId.panid,\
                                                         yhProtocolRingBuffer.tarLongId, \
                                                        0xfc99,\
                                                        &yhProtocolRingBuffer.data[2],\
                                                        yhProtocolRingBuffer.data[1]);
                  break;
                }

                case YH_RAW_IN:
                {
                  break;
                }

                case YH_CHECK_CHANNEL:
                {
                  yhTemp[5] = YH_CHANNEL_RETURN;//function code
                  yhTemp[7] = 0x0d;
                  yhProtocolRingBuffer.data[1] = 0x01;
                  yhProtocolRingBuffer.data[2] = emberAfGetRadioChannel();
                  memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1,yhProtocolRingBuffer.data,2);
                  memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2,&yhProtocolRingBuffer.data[2],yhProtocolRingBuffer.data[1]);
                  yhProtocolRingBuffer.crc.crc16=getCRC(yhTemp,2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]);
                  memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1],&yhProtocolRingBuffer.crc.crc8[1],1);
                  memcpy(yhTemp+2+1+1+1+1+1+1+8+1+1+2+yhProtocolRingBuffer.data[1]+1,&yhProtocolRingBuffer.crc.crc8[0],1);
                  emberSerialWriteData(HAL_SERIAL_PORT_USART2, yhTemp, yhProtocolRingBuffer.length.len+10);//10 is len
                  emberSerialWaitSend(HAL_SERIAL_PORT_USART2);

                  break;
                }
                default:
                {
                  break;
                }

              }//end switch

              memset(yhTemp,0,sizeof(yhTemp));

           }
        }
    }
}
