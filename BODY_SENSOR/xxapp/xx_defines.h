/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.
                                                  .
***************************************************************************************************/
#ifndef _XX_DEFINES_H
#define _XX_DEFINES_H

#ifdef __cplusplus
extern "C"
{
#endif
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
*                                             INCLUDES
*                                            用户头文件
***************************************************************************************************/

/***************************************************************************************************
*                                          
*                                              
***************************************************************************************************/
typedef int8_t    int8;
typedef uint8_t   uint8;
typedef int16_t   int16;
typedef uint16_t  uint16;
typedef int32_t   int32;
typedef uint32_t  uint32;
typedef int64_t   int64;
typedef uint64_t  uint64;

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
#define XX_DATAPOINT_HEAD_LEN                                       4
#define XX_DATAPOINT_INGNORE_LEN                                    0xff
#define XX_DATAPOINT_TOKEN_NULL                                     0xff

#define XX_DATAPOINT_TOKEN_OFFSET                                   0
#define XX_DATAPOINT_DATA_TYPE_OFFSET                               1
#define XX_DATAPOINT_LEN_HI_OFFSET                                  2
#define XX_DATAPOINT_LEN_LO_OFFSET                                  3
#define XX_DATAPOINT_DATA_OFFSET                                    4

#define XX_DATAPOINT_DATA_TYPE_BOOL                                 0
#define XX_DATAPOINT_DATA_TYPE_INT                                  1
#define XX_DATAPOINT_DATA_TYPE_ENUM                                 2
#define XX_DATAPOINT_DATA_TYPE_STRING                               3
#define XX_DATAPOINT_DATA_TYPE_FLOAT                                4
#define XX_DATAPOINT_DATA_TYPE_NULL                                 5

#define XX_VALUE_NULL_UINT8                                         0xff
#define XX_VALUE_NULL_UINT16                                        0xffff
#define XX_VALUE_NULL_UINT32                                        0xffffffff

#define XX_ZCL_ATTRIBUTE_HEAD_LEN                                   3
#define XX_ZCL_READ_ATTRIBUTE_RESPONSE_HEAD_LEN                     4

#define XX_ZCL_ATTRIBUTE_ID_HEARTBEAT_TIME                          0xF000
#define XX_ZCL_ATTRIBUTE_ID_MAC_ADDR                                0xF001
#define XX_ZCL_ATTRIBUTE_ID_PRODUCT_ID                              0xF002
#define XX_ZCL_ATTRIBUTE_ID_DEVICE_ID                               0xF003
#define XX_ZCL_ATTRIBUTE_ID_DEVICE_NAME                             0xF004


#define XX_SERIAL_WIFI_FRAME_KEYWORD_LEN                            8
#define XX_SERIAL_WIFI_FRAME_CHECKSUM_LEN                           1
#define XX_SERIAL_WIFI_FRAME_VERSION_1                              1
#define XX_SERIAL_WIFI_FRAME_VERSION_2                              2


#define XX_SERIAL_WIFI_FRAME_TYPE_ACK                               0xF0
#define XX_SERIAL_WIFI_FRAME_TYPE_ZIGBEE                            0xA0
#define XX_SERIAL_WIFI_FRAME_TYPE_HANDSHAKE                         0x00
#define XX_SERIAL_WIFI_FRAME_TYPE_WIFI_NWK_STATE                    0x01
#define XX_SERIAL_WIFI_FRAME_TYPE_GET_WIFI_NWK_STATE                0x02
#define XX_SERIAL_WIFI_FRAME_TYPE_MODE                              0x03
#define XX_SERIAL_WIFI_FRAME_TYPE_TEST                              0x04
#define XX_SERIAL_WIFI_FRAME_TYPE_OTA_NOTIFY                        0x05
#define XX_SERIAL_WIFI_FRAME_TYPE_OTA_DATA                          0x06
#define XX_SERIAL_WIFI_FRAME_TYPE_OTA_REPORT_VERSION                0x07
#define XX_SERIAL_WIFI_FRAME_TYPE_OTA_FAIL                          0x08
#define XX_SERIAL_WIFI_FRAME_TYPE_GET_ATTRIBUTE                     0x50
#define XX_SERIAL_WIFI_FRAME_TYPE_SET_ATTRIBUTE                     0x60
#define XX_SERIAL_WIFI_FRAME_TYPE_REPORT_ATTRIBUTE                  0x66
#define XX_SERIAL_WIFI_FRAME_TYPE_REPORT_DATA_POINT                 0x70
#define XX_SERIAL_WIFI_FRAME_TYPE_GET_DATA_POINT                    0x90



#define XX_SERIAL_WIFI_FRAME_SOF_1                                  0x55
#define XX_SERIAL_WIFI_FRAME_SOF_2                                  0xAA   
#define XX_SERIAL_WIFI_FRAME_SOF_1_OFFSET                           0
#define XX_SERIAL_WIFI_FRAME_SOF_2_OFFSET                           1
#define XX_SERIAL_WIFI_FRAME_VERSION_OFFSET                         2
#define XX_SERIAL_WIFI_FRAME_INDEX_OFFSET                           3
#define XX_SERIAL_WIFI_FRAME_TYPE_OFFSET                            4
#define XX_SERIAL_WIFI_FRAME_LEN_HI_OFFSET                          5
#define XX_SERIAL_WIFI_FRAME_LEN_LO_OFFSET                          6
#define XX_SERIAL_WIFI_FRAME_DATA_OFFSET                            7


#define XX_SERIAL_WIFI_DP_HEAD_LEN                                  4

#define XX_SERIAL_WIFI_DP_STATUS                                    0x00

#define XX_SERIAL_WIFI_DP_CMD_REGISTER                              0x01
#define XX_SERIAL_WIFI_DP_CMD_LEAVE                                 0x02
#define XX_SERIAL_WIFI_DP_CMD_DEVICE_STATUS                         0x03

#define XX_SERIAL_WIFI_DP_CMD_ZIGBEE_NWK_OPEN_CLOSE                 0x30
#define XX_SERIAL_WIFI_DP_CMD_GET_NWK_CONFIG                        0x31
#define XX_SERIAL_WIFI_DP_CMD_GET_NODE_INFO                         0x32
#define XX_SERIAL_WIFI_DP_CMD_RESET_NWK_CONFIG                      0x33
#define XX_SERIAL_WIFI_DP_CMD_DEL_NODE                              0x34

#define XX_SERIAL_WIFI_DP_CMD_LIGHT_1_ON_OFF                        0x60
#define XX_SERIAL_WIFI_DP_CMD_LIGHT_2_ON_OFF                        0x61
#define XX_SERIAL_WIFI_DP_CMD_LIGHT_3_ON_OFF                        0x62
#define XX_SERIAL_WIFI_DP_CMD_LIGHT_4_ON_OFF                        0x63
#define XX_SERIAL_WIFI_DP_CMD_LIGHT_5_ON_OFF                        0x64

#define XX_SERIAL_WIFI_DP_CMD_OTA_NOTIFY                            0x70//-- 仅用于测试
#define XX_SERIAL_WIFI_DP_CMD_OTA_QUERY_NEXT_IMAGE_REQUEST          0x71
#define XX_SERIAL_WIFI_DP_CMD_OTA_QUERY_NEXT_IMAGE_RESPONSE         0x72
#define XX_SERIAL_WIFI_DP_CMD_OTA_BLOCK_REQUEST                     0x73
#define XX_SERIAL_WIFI_DP_CMD_OTA_BLOCK_RESPONSE                    0x74
#define XX_SERIAL_WIFI_DP_CMD_OTA_UPGRADE_END_REQUEST               0x75
#define XX_SERIAL_WIFI_DP_CMD_OTA_UPGRADE_END_RESPONSE              0x76

#define XX_SERIAL_WIFI_DP_CMD_BUTTON_TEST                           0x7A


#define XX_SERIAL_WIFI_DP_ATTR_MAC_ADDR                             0x80
#define XX_SERIAL_WIFI_DP_ATTR_PRODUCT_ID                           0x81
#define XX_SERIAL_WIFI_DP_ATTR_DEVICE_ID                            0x82
#define XX_SERIAL_WIFI_DP_ATTR_DEVICE_NAME                          0x83
#define XX_SERIAL_WIFI_DP_ATTR_SHORT_ADDR                           0x84
#define XX_SERIAL_WIFI_DP_ATTR_PANID                                0x85
#define XX_SERIAL_WIFI_DP_ATTR_CHANNEL                              0x86
#define XX_SERIAL_WIFI_DP_ATTR_VERSION                              0x87
#define XX_SERIAL_WIFI_DP_ATTR_ENDPOINT                             0x88

#define XX_SERIAL_WIFI_NWK_STATE_FINISH_INIT                        0x01
#define XX_SERIAL_WIFI_NWK_STATE_CONFIG_NWK                         0x02
#define XX_SERIAL_WIFI_NWK_STATE_CONFIG_NWK_FAIL                    0x03
#define XX_SERIAL_WIFI_NWK_STATE_CONNECT_ROUTER                     0x04
#define XX_SERIAL_WIFI_NWK_STATE_CONNECT_ROUTER_FAIL                0x05
#define XX_SERIAL_WIFI_NWK_STATE_CONNECT_ROUTER_SUCCESS             0x06
#define XX_SERIAL_WIFI_NWK_STATE_CONNECT_SERVER                     0x07
#define XX_SERIAL_WIFI_NWK_STATE_CONNECT_SERVER_FAIL                0x08
#define XX_SERIAL_WIFI_NWK_STATE_CONNECT_SERVER_SUCCESS             0x09


#define XX_STATUS_SUCCESS                                           0
#define XX_STATUS_FAIL                                              1
#define XX_STATUS_NODE_NON_EXSITENT                                 2

/***************************************************************************************************
*                                            MACRO DEFINED
*                                               宏定义
***************************************************************************************************/
#define XX_WATCHDOG_RESET                                           halResetWatchdog()

#define BREAK_UINT32( var, ByteNum )                                (uint8)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3)                    ((uint32_t)((uint32_t)((Byte0) & 0x00FF) + ((uint32_t)((Byte1) & 0x00FF) << 8) + ((uint32_t)((Byte2) & 0x00FF) << 16) + ((uint32_t)((Byte3) & 0x00FF) << 24)))
#define BUILD_UINT16(loByte, hiByte)                                ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
#define HI_UINT16(a)                                                (((a) >> 8) & 0xFF)
#define LO_UINT16(a)                                                ((a) & 0xFF)
#define BUILD_UINT8(hiByte, loByte)                                 ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))
#define HI_UINT8(a)                                                 (((a) >> 4) & 0x0F)
#define LO_UINT8(a)                                                 ((a) & 0x0F)

#ifndef BV
#define BV(n)                                                       (1 << (n))
#endif

#ifndef BF
#define BF(x,b,s)                                                   (((x) & (b)) >> (s))
#endif

#ifndef MIN
#define MIN(n,m)                                                    (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)                                                    (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)                                                      (((n) < 0) ? -(n) : (n))
#endif

#ifndef DEC
#define DEC(A,B)                                                    (((A) >= (B)) ? ((A)-(B)) : ((B)-(A)))
#endif

#ifndef MACSTR
#define MACSTR(a)                                                   a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]
#endif


/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


