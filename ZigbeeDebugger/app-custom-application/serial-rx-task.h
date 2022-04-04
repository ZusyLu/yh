#ifndef _SERIAL_RX_TASK_H_
#define _SERIAL_RX_TASK_H_

#include <stdint.h>
//#include"app-custom-application/gw-cfg-tokens.h"

//function code
#define   YH_SET_CHANNEL                  0X00
#define   YH_SET_CHANNEL_RESULT           0X01
#define   YH_SCANNING_DEVICE              0X02
#define   YH_TARGET_DEVICE_INFORMATION    0X03
#define   YH_SET_TARGET_PARAMETERS        0X04
#define   YH_RAW_OUT                      0X05
#define   YH_RAW_IN                       0X06
#define   YH_CHECK_CHANNEL                0X07
#define   YH_CHANNEL_RETURN               0X08

#define   YH_INTERPAN_DATA_LEN            150


#ifdef OWN_SERIAL_RX_TASK
    #define  EXTERN_T
#else
    #define  EXTERN_T  extern
#endif

typedef struct{
   uint8_t  rfLength;
   uint8_t  header;
   uint8_t  length;
   uint8_t  cmd0;
   uint8_t  cmd1;
   uint8_t  data[80];
   uint8_t  fcs;//frame check-sum
}ProtocolDataStruct;

typedef union{
  uint8_t heads[2];
  uint16_t head; //no use
}yhheadType;

typedef union{
  uint8_t lens[2];
  uint16_t len;
}yhLenType;

typedef union{
  uint8_t pans[2];
  uint16_t panid;
}yhPanType;

typedef union{
  uint8_t crc8[2];
  uint16_t crc16;
}yhcrcType;

typedef union{
  uint8_t cm[2];
  uint16_t cmd;
}yhcmdType;


typedef struct{
   uint8_t  rfSumLength;  //
   yhheadType  header;
   uint8_t  version;//;
   yhcmdType  cmdid;
   uint8_t  functionCode;
   yhLenType length;
   uint8_t  tarLongId[8];
   yhPanType tarPanId;
   uint8_t  data[80];
   yhcrcType crc;//
}yhDebugProtocolDataStruct;

//customer protocal define
/*---------------------------protocal format define-----------------------------

HEAD	   LEN	      CMD0	CMD1	  DATA	   FCS
1 Byte	  1 Byte	      1 Byte	1 Byte		   1 Byte
֡ͷ,7C	  ���ݳ���     Ԥ��	������	  ���ݶ�    У��

--------------------------------------------------------------------------------*/
#define FULL_BUFFER_MIN_LENGTH    5
#define CMD0_OFFSET   2
#define CMD1_OFFSET   3
#define DATA_OFFSET   4
#define FRAME_HEADER  0x7C
#define FRAME_CMD0    0x01
//Define CMD1
//MCU-->ZIGBEE
#define GW_QUERY_CONTROL_POINT_VALE    0x04
#define RF_QUERY_CONTROL_POINT_VALE    0X82

#define GW_CONTROL_CONTROL_POINT    0x05
#define RF_GW_CONTROL_CONTROL_POINT 0X83

#define GW_UPDATE_PANNEL       0x06
#define RF_GW_UPDATE_PANNEL    0X84  //

#define GW_UPDATE_PANNEL_SCENE       0x07
#define RF_GW_UPDATE_PANNEL_SCENE    0X85

#define GW_IMAGE_NOTIFY            0x08
#define GW_IMAGE_TX                0X09
#define GW_OTA_NOTIFY              0X0B

#define GW_ZIGBEE_REBOOT   0x0A
#define GW_MCU_ACK         0x7F

#define GW_DIRECT_TX       0x40
#define RF_GW_DIRECT_TX    0x40

#define RF_GW_TEST_COMMAND 0xF0

//ZIGBEE-->MCU
#define GW_PUSH_CONTROL_POINT    0x83
#define GW_CTRL_CONTROL_POINT    0x84
#define GW_RECALL_SCENE          0x85
#define GW_IMAGE_REQUEST         0x86
#define GW_IMAGE_PACKET_REQUEST  0x87
#define GW_ZIGBEE_ACK            0xFF

//add by yh
#define   GW_VERSION             0X01

//Send mode
#define RF_SEND_MODE_UNICASE     0x00
#define RF_SEND_MODE_BROADCASE   0x01

#ifndef OWN_SERIAL_RX_TASK
  extern  uint16_t lastSerialRxTime;
  extern  ProtocolDataStruct protocolRingBuffer;
#endif
EXTERN_T  void clearSerialBuffer(void);
EXTERN_T  uint8_t checkSum(uint8_t *data,uint8_t length);
EXTERN_T  void serialRxProcess(uint8_t Port);
EXTERN_T  void modeInitialFromToken(void);
EXTERN_T  void popRingBufferOneFrame(void);

EXTERN_T EmberApsFrame globalApsFrame;
EXTERN_T uint8_t appZclBuffer[];
EXTERN_T uint16_t appZclBufferLen;
EXTERN_T bool zclCmdIsBuilt;
EXTERN_T uint16_t mfgSpecificId;
EXTERN_T uint8_t disableDefaultResponse;
EXTERN_T void yhZdoLeaveRequestCommand(EmberNodeId target);
EXTERN_T void yhZdoIeeeAddressRequestCommand(EmberNodeId nodeId);
EXTERN_T void HTemAfInterpanFragmentTestCommand(EmberPanId htPanId,uint8_t *htMacId , \
                                       uint16_t HTclusterId,uint8_t *data,uint16_t HTmessageLen);


void yhDealWith(int8u* usartData,int8u len);

//extern tokTypeModuleOtaInfoStruct otaInfo;
extern uint8_t testMessage[];
//extern tokTypeModuleOtaDownLoadInfoStruct otaDownloadInfo;

#endif
