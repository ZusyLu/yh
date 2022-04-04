#ifndef _PROTOCOL_PARSING_H_
#define _PROTOCOL_PARSING_H_
#include <stdint.h>

#define protocolPrintln                                 emberAfCorePrintln
#define protocolPrintBuffer                             emberAfCorePrintBuffer

//local

#define PROTOCOL_BUFF_MAX_LEN                           128
#define PROTOCOL_BUFF_MIN_LEN                           7
#define PROTOCOL_HEADER_POSITION                        0X00
#define PROTOCOL_VERSION_POSITION                       0X01
#define PROTOCOL_SEQ_POSITION                           0X02
#define PROTOCOL_FRAM_TYPE_POSITION                     0X03
#define PROTOCOL_FRAM_LEN_H_POSITION                    0X04
#define PROTOCOL_FRAM_LEN_L_POSITION                    0X05
#define PROTOCOL_FRAM_DATA_POSITION                     0X06

#define PROTOCOL_HEADER                                 0X68
#define PROTOCOL_VERSION                                0X01

//protocol fram type
#define PROTOCOL_NORMAL_FRAM_TYPE                       0x01
#define PROTOCOL_NORMAL_ACK_FRAM_TYPE                   0x02
#define PROTOCOL_ERROR_FRAM_TYPE                        0x0F

//protocol fram cmd
//down
#define PROTOCOL_OPEN_CLOSE_NET_CMD                     0X0014
#define PROTOCOL_REQUEST_NODE_LEAVE_NET_CMD             0X0026
#define PROTOCOL_DEVICE_ANNOUNCE_CMD                    0X004D
#define PROTOCOL_IEEE_ADDRESS_REQUEST                   0X0041
#define PROTOCOL_NETWORK_ADDRESS_REQUEST                0X0040
#define PROTOCOL_INFRARED_SET_CMD                       0X00f1
#define PROTOCOL_DOOR_LOCK_CMD                          0X00f2
#define PROTOCOL_CURTAIN_CMD                            0X00f3

#define PROTOCOL_PENETRATE_CMD                          0X7001
#define PROTOCOL_SEND_ERROR_CMD                         0XF001
#define PROTOCOL_SEND_ERROR_CMD_LEN                     8

//up
#define PROTOCOL_NODE_LEAVE_NET_CMD                     0X8026
#define PROTOCOL_SIMPLE_DESCRIPTOR_RESPONSE             0X8043
#define PROTOCOL_IEEE_ADDRESS_RESPONSE                  0X8041
#define PROTOCOL_NETWORK_ADDRESS_RESPONSE               0X8040
#define PROTOCOL_READ_ARRIBUTES_RESPONSE                0X8100
#define PROTOCOL_INFRARED_REPROT_CMD                    0X8ff1
#define PROTOCOL_DOOR_LOCK_REPORT_CMD                   0X8ff2
#define PROTOCOL_CURTAIN_REPORT_CMD                     0X8ff3

#define PROTOCOL_REPORT_ARRIBUTE                        0X8102
#define PROTOCOL_ZONE_STATUS_CHANGE_NOTIFICATION        0X8401

typedef struct
{
    uint8_t head;
    uint8_t version;
    uint8_t seq;
    uint8_t framType;
    uint16_t dataLen;
    uint8_t data[PROTOCOL_BUFF_MAX_LEN - PROTOCOL_BUFF_MIN_LEN];
    uint8_t calcSum;
}protocolType;

typedef struct
{
    uint16_t cmd;
    uint8_t data[PROTOCOL_BUFF_MAX_LEN - PROTOCOL_BUFF_MIN_LEN-2];
}cmdDataType;

extern uint8_t sendSeq;
extern protocolType protocolData;
extern void ParseProtocolFramingFunction(void);
extern void yhZdoLeaveRequestCommand(EmberNodeId target);
extern uint8_t *reverseHex(uint8_t *dest, uint8_t *source, uint8_t len);
extern uint8_t yhNwkSendRfPacket(uint8_t srcEp, uint8_t dstEp, uint16_t dstAddr, uint8_t frameControl, uint16_t clusterID, uint8_t command, uint8_t *buf, uint8_t len);

#endif
