#ifndef _SERIAL_RX_TASK_H_
#define _SERIAL_RX_TASK_H_
#define  SERIAL_BUFFER_LENGTH  128
#include <stdint.h>
#include"app-custom-application/gw-cfg-tokens.h"

#ifdef OWN_SERIAL_RX_TASK
    #define  EXTERN_T
#else
    #define  EXTERN_T  extern
#endif

typedef struct {
   int8u length;
   int8u buffer[SERIAL_BUFFER_LENGTH];
} SerialRxDateStruct;

typedef union{
  int8u data[2];
  int16u nodeId;
}yhNodeIDType;

typedef struct{
   uint8_t  rfLength;
   uint8_t  header;
   uint8_t  length;
   uint8_t  cmd0;
   uint8_t  cmd1;
   uint8_t  data[80];
   uint8_t  fcs;//frame check-sum
}ProtocolDataStruct;

typedef struct
{
  uint16_t nodeId;
  uint16_t clusterId;
  uint16_t attrId;
  uint8_t type;
  uint8_t commandId;
  uint8_t data[100];
  uint8_t length;
}yhWriteCommandtype;

//customer protocal define
/*---------------------------protocal format define-----------------------------

HEAD	   LEN	      CMD0	CMD1	  DATA	   FCS
1 Byte	  1 Byte	      1 Byte	1 Byte		   1 Byte
֡ͷ,7C	  ���ݳ���     Ԥ��	������	  ���ݶ�    У��

--------------------------------------------------------------------------------*/
#define  PROTOCAL_SERIAL_PORT          0x22       //change by lu 1 to 0x22
#define FULL_BUFFER_MIN_LENGTH    5
#define CMD0_OFFSET   2
#define CMD1_OFFSET   3
#define DATA_OFFSET   4
#define FRAME_HEADER  0x7C
#define FRAME_CMD0    0x01
//Define CMD1
//MCU-->ZIGBEE
#define GW_QUERY_CONTROL_POINT_VALE                                 0x04
#define RF_QUERY_CONTROL_POINT_VALE                                 0X82

#define GW_CONTROL_CONTROL_POINT                                    0x05
#define RF_GW_CONTROL_CONTROL_POINT                                 0X83

#define GW_CONTROL_CONTROL_POINT_BROAD_CAST                         0X0D

#define GW_UPDATE_PANNEL                                            0x06
#define RF_GW_UPDATE_PANNEL                                         0X84  //

#define GW_UPDATE_PANNEL_SCENE                                      0x07
#define RF_GW_UPDATE_PANNEL_SCENE                                   0X85

#define GW_IMAGE_NOTIFY                                             0x08
#define GW_IMAGE_TX                                                 0X09
#define GW_OTA_NOTIFY                                               0X0B

#define GW_ZIGBEE_REBOOT                                            0x0A
#define GW_MCU_ACK                                                  0x7F

#define GW_DIRECT_TX                                                0x40
#define RF_GW_DIRECT_TX                                             0x40

#define RF_GW_TEST_COMMAND                                          0xF0

//ZIGBEE-->MCU
#define GW_PUSH_CONTROL_POINT                                       0x83
#define GW_CTRL_CONTROL_POINT                                       0x84
#define GW_RECALL_SCENE                                             0x85
#define GW_IMAGE_REQUEST                                            0x86
#define GW_IMAGE_PACKET_REQUEST                                     0x87
#define GW_ZIGBEE_ACK                                               0xFF

//add by yh
#define   GW_VERSION                                                0X04

#define    YH_WINDOW_OPEN                                           0X00
#define    YH_WINDOW_CLOSE                                          0X01
#define    YH_WINDOW_STOP                                           0X02
#define    YH_WINDOW_LIFT_PERCENTAGE                                0X03

//Send mode
#define RF_SEND_MODE_UNICASE                                        0x00
#define RF_SEND_MODE_BROADCASE                                      0x01

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
EXTERN_T yhWriteCommandtype yhInfraredData;
EXTERN_T void yhZdoLeaveRequestCommand(EmberNodeId target);
EXTERN_T void yhZdoIeeeAddressRequestCommand(EmberNodeId nodeId);
EXTERN_T void yhZclGlobalWriteCommand(uint8_t type, uint8_t commandId, uint16_t clusterId, uint16_t attrId, uint8_t* data, uint8_t length);

extern uint8_t yh_send[];
extern uint8_t yh_obj[];
extern uint8_t yh_hex_obj[];
extern void bin2str(char *to, const unsigned char *p, size_t len);
void yhDealWith(int8u* usartData,int8u len);

extern tokTypeModuleOtaInfoStruct otaInfo;
//extern tokTypeModuleOtaDownLoadInfoStruct otaDownloadInfo;

#endif
