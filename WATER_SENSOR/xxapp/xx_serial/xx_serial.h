/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.
                                                  .
***************************************************************************************************/
#ifndef _XX_SERIAL_H
#define _XX_SERIAL_H

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
#include "xx_serial_at.h"

/***************************************************************************************************
*                                               MACRO
*                                               宏定义
***************************************************************************************************/
#define XX_MAIN_START_2

#define XX_SERIAL_TEST_MODE_BIT                                     0x01
#define XX_SERIAL_TEST_MODE_FLAG                                    (Xx_serial_status & XX_SERIAL_TEST_MODE_BIT)
#define XX_SERIAL_TEST_MODE_FLAG_SET                                (Xx_serial_status |= XX_SERIAL_TEST_MODE_BIT)                           
#define XX_SERIAL_TEST_MODE_FLAG_CLEAR                              (Xx_serial_status &= ~XX_SERIAL_TEST_MODE_BIT)                           

#define XX_SERIAL_AT_BUF_SIZE                           128


/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
//-- COM_USART0 = 1, COM_USART1 = 2, COM_USART2 = 3
#define XX_SERIAL_COM_USART0                                1
#define XX_SERIAL_COM_USART1                                2
#define XX_SERIAL_COM_USART2                                3


//#define XX_SERIAL_INTERRUPT_PORT                          GPIO_PAIN
//#define XX_SERIAL_INTERRUPT_PIN                           PORTA_PIN(2)

//#define XX_SERIAL_INTERRUPT_CONFIG_OUTPUT                 do { halGpioSetConfig(XX_SERIAL_INTERRUPT_PIN, GPIOCFG_OUT); } while (0)
//#define XX_SERIAL_INTERRUPT_PIN_SET                       do { halGpioSet(XX_SERIAL_INTERRUPT_PIN); } while (0)
//#define XX_SERIAL_INTERRUPT_PIN_CLEAR                     do { halGpioClear(XX_SERIAL_INTERRUPT_PIN); } while (0)

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef struct
{
   volatile uint16 head;
   volatile uint16 tail;
   volatile uint16 len;
   volatile uint16 lenBackup;
   volatile uint16 packetLen;
   uint16 size;
   uint8 *buf;
}XxSerialReceiveDef;

typedef struct tagXxSerialQueueDef
{
    uint8 frameIndex;
    uint8 frameType;
    uint8 dataLen;
    uint8 sendTimeout;
    uint8 retryDelayCount;
    uint8 retryCount;
    struct tagXxSerialQueueDef *next;
}XxSerialQueueDef;

extern uint8 Xx_serial_status;
extern XxSerialReceiveDef serial_at;
/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/
extern void XxMainStart2(void);
extern uint8 XxSerialSystemComReceive(uint8 c);
extern void XxSerialQueuePush(XxSerialQueueDef **this, XxSerialQueueDef *addr);
extern XxSerialQueueDef *XxSerialQueuePop(XxSerialQueueDef **queue_head);


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


