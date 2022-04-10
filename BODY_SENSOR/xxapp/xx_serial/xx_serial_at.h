/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                       .
***************************************************************************************************/
#ifndef _XX_SERIAL_AT_H
#define _XX_SERIAL_AT_H

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

//-- 打印串口默认使用PA05(TX),PA06(RX)，与WIFI通讯的串口PC0(TX),PC2(RX)，AT指令串口，暂定PC0(TX),PC2(RX)

/***************************************************************************************************
*                                             INCLUDES
*                                            系统头文件
***************************************************************************************************/

/***************************************************************************************************
*                                             INCLUDES
*                                            用户头文件
***************************************************************************************************/
#include "xxapp/xx_main.h"

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
//#define XX_SERIAL_AT_PORT_NUM                               XX_SERIAL_COM_USART1

#define XX_SERIAL_AT_PACKET_SOF1                            'A'
#define XX_SERIAL_AT_PACKET_SOF2                            'T'
#define XX_SERIAL_AT_PACKET_SOF3                            '+'

//#define XX_SERIAL_AT_PORT_NUM                              XX_SERIAL_COM_USART0

#define XX_SERIAL_AT_PACKET_KEYWORD_LEN                     6 //-- AT+RF_

#define XX_SERIAL_PACKET_SOF1_OFFSET                        0
#define XX_SERIAL_PACKET_SOF2_OFFSET                        1
#define XX_SERIAL_PACKET_LEN_OFFSET                         2
#define XX_SERIAL_PACKET_INDEX_OFFSET                       3
#define XX_SERIAL_PACKET_TYPE_OFFSET                        4
#define XX_SERIAL_PACKET_VERSION_OFFSET                     5
#define XX_SERIAL_ZIGBEE_CMD_OFFSET                         6
#define XX_SERIAL_ZIGBEE_DESTADDR_OFFSET                    8
#define XX_SERIAL_ZIGBEE_SRCADDR_OFFSET                     10
#define XX_SERIAL_ZIGBEE_OPT_OFFSET                         12
#define XX_SERIAL_ZIGBEE_DATA_OFFSET                        16

#define XX_SERIAL_CRC1_OFFSET_FROM_END                      4
#define XX_SERIAL_CRC2_OFFSET_FROM_END                      3
#define XX_SERIAL_EOF1_OFFSET_FROM_END                      2
#define XX_SERIAL_EOF2_OFFSET_FROM_END                      1


#define XX_SERIAL_PACKET_OPT_DATATYPE_MASK                  0x01
#define XX_SERIAL_PACKET_OPT_DATATYPE_ACK                   0
#define XX_SERIAL_PACKET_OPT_DATATYPE_CMD                   1


//#define XX_SERIAL_INTERRUPT_PORT                          GPIO_PAIN
//#define XX_SERIAL_INTERRUPT_PIN                           PORTA_PIN(2)

//#define XX_SERIAL_INTERRUPT_CONFIG_OUTPUT                 do { halGpioSetConfig(XX_SERIAL_INTERRUPT_PIN, GPIOCFG_OUT); } while (0)
//#define XX_SERIAL_INTERRUPT_PIN_SET                       do { halGpioSet(XX_SERIAL_INTERRUPT_PIN); } while (0)
//#define XX_SERIAL_INTERRUPT_PIN_CLEAR                     do { halGpioClear(XX_SERIAL_INTERRUPT_PIN); } while (0)

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern uint8 XxSerialATCommandPoll(void *com, uint32 sysTimeMillisecond);

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


