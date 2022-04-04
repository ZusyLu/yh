#ifndef _SERIAL_RX_TASK_H_
#define _SERIAL_RX_TASK_H_

#define  FRAME_HEADER                                                   0x7C
#define  FRAME_CMD0                                                     0x01
#define  YH_PENETRATE_CMD1                                              0X88

#define LEWIN_PRO_VERSION                                               0x01
#define LEWIN_FRAME_DEFAULT_CMD                                         0X0000
#define YH_SET_NETWORK_FUNCTION_CODE                                    0X0A
#define YH_DEBUG_DEFAULT_PANID                                          0X6868
#define YH_SET_NETWORK_OK                                               0X01
typedef struct {
  uint8_t panid;
  uint8_t longid[8];
}yhPenetrateDataType;

extern  void clearSerialBuffer(void);
extern  void serialRxProcess(int8u Port);
extern  uint16_t getDeviceTokenAddr(void);
extern  void modeInitialFromToken(void);
extern  void rfSendCommandOtaFinish(uint8_t status);
extern void yhDebugDataAndSendFunction( int8u version, int16u cmd, int8u functionCode, int16u dataLen, int8u *destLongMac, int16u panId, int8u *data  );

//raw
extern EmberApsFrame globalApsFrame;
extern uint8_t appZclBuffer[];
extern uint16_t appZclBufferLen;
extern bool zclCmdIsBuilt;
extern uint16_t mfgSpecificId;
extern uint8_t disableDefaultResponse;

extern void yhEmAfCliRawCommand(uint8_t *srcbuff,uint8_t Length,uint16_t htClusterID,uint16_t  htDest);
extern void yhSerialSendProtocolFuction(int8u head, int8u len, int8u cmd0, int8u cmd1, int8u *data);
#endif
