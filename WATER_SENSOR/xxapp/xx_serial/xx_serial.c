/**************************************************************************************************
  Filename:       xx_example.c
  Revised:        $Date: 2021-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.1.0(2 stacks)

  Copyright 2022 lu.
                       
***************************************************************************************************/
#include "xxapp/xx_main.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _XX_SERIAL_H
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
*                                               MACRO
*                                               宏定义
***************************************************************************************************/
//#define xxSerialPrint                                   emberAfAppPrint
//#define xxSerialPrintBuffer                             emberAfAppPrintBuffer

#define xxSerialPrint(pstring, ...)
#define xxSerialPrintBuffer(x,y,z)

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
//#define XX_SERIAL_AT_BUF_SIZE                           128
#define XX_SERIAL_WIFI_BUF_SIZE                         1040

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/
uint8 serial_at_buf[XX_SERIAL_AT_BUF_SIZE];
XxSerialReceiveDef serial_at;

uint8 serial_wifi_buf[XX_SERIAL_WIFI_BUF_SIZE];
XxSerialReceiveDef serial_wifi;

uint8 Xx_serial_status = 0;

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
void XxSerialQueuePush(XxSerialQueueDef **this, XxSerialQueueDef *addr)
{
    uint8 errCount=0;
    
    while(*this)
    {
        XX_WATCHDOG_RESET;
        
        if((*this)->next!=NULL)
            this=&((*this)->next);
        else
            break;

        if(errCount++ >= 50)
            return;
    }

    if(*this==NULL)
        *this=addr;
    else
        (*this)->next=addr;
}

XxSerialQueueDef *XxSerialQueuePop(XxSerialQueueDef **queue_head)
{
    XxSerialQueueDef *msg;
        
    if(*queue_head==NULL)
        return 0;
    else
    {
        msg=*queue_head;
        (*queue_head)=(*queue_head)->next;
        msg->next = NULL;
    }
    
    return msg;
}

void xxSerialDataConfig(XxSerialReceiveDef *uart, uint16 size, uint8 *buf)
{
    uart->head = 0;
    uart->tail = 0;
    uart->len  = 0;

    uart->size = size;
    uart->buf = buf;
}

void xxSerialDataInit(XxSerialReceiveDef *uart)
{
    uart->head = 0;
    uart->tail = 0;
    uart->len  = 0;
}


/*
 * 此函数添加到SDK里的文件里
 * C:\SiliconLabs\SimplicityStudio\v5\developer\sdks\gecko_sdk_suite\v3.2\protocol\zigbee\app\util\serial\command-interpreter2.c
 * 在函数bool emberProcessCommandString(uint8_t *input, uint8_t sizeOrPort)前先添加extern uint8_t XxSerialSystemComReceive(uint8_t c);
 * 在函数bool emberProcessCommandString(uint8_t *input, uint8_t sizeOrPort)里做以下修改

    switch (emberSerialReadByte(sizeOrPort, &next)) {
      case EMBER_SUCCESS:
        if(XxSerialSystemComReceive(next))
        {
          commandReaderInit();
          return false;
        }
        break;
      case EMBER_SERIAL_RX_EMPTY:
        return isEol;
      default:
        commandState.error = EMBER_CMD_ERR_PORT_PROBLEM;
        goto READING_TO_EOL;
    }
*/
uint8_t XxSerialSystemComReceive(uint8 c)
{
    XxSerialReceiveDef *uart;
    uart = &serial_at;
    if (uart->size == uart->len)
    {
        uart->head = 0;
        uart->tail = 0;
        uart->len  = 0;
        return 0;
    }

    //if (c != '\r')
    //    emberAfAppPrint("%c",c);
    
    uart->buf[uart->tail] = c;
    uart->len++;
    uart->tail++;
    uart->tail %= uart->size;

    //-- 判断回车，马上处理
    if (c == '\r' || c == '\n')
    //if ( c == '\n' )
    {
        if (XxSerialATCommandPoll((void*)uart, 0))
        {
            return 1;
        }
    }

    return 0;
}

void xxSerialReceive(void *com, uint8 com_num)
{
    EmberStatus readStatus;
    uint16 rxBufLen = 0;
    XxSerialReceiveDef *uart = (XxSerialReceiveDef*)com;
    
    //== FIFO INPUT
    rxBufLen = emberSerialReadAvailable(com_num);
    if (rxBufLen)
    {
        if (uart->size == uart->len)
        {
            uart->head = 0;
            uart->tail = 0;
            uart->len  = 0;
            return;
        }
        
        if ((uart->len+rxBufLen)>uart->size)  //-- 数据太多,buf已装不下，只读取装满BUF的字节数
        {
            rxBufLen=uart->size-uart->len;
        }

        //== 从DMA的BUF中读取数据
        if ((uart->tail+rxBufLen)<uart->size) //-- 数据长度还没达到BUF末尾，可以一次读出来
        {
            readStatus=emberSerialReadData(com_num, &uart->buf[uart->tail], rxBufLen, 0);
            if (readStatus!=EMBER_SUCCESS)
                return;

            //--if(!g_WsUartSendFlag)
            {
                xxSerialPrint("UART+: --> ");//--
                xxSerialPrintBuffer(&uart->buf[uart->tail], rxBufLen, TRUE);
                xxSerialPrint("\r\n");
            }
            
            uart->tail+=rxBufLen;
            uart->len+=rxBufLen;

            //--if(!g_WsUartSendFlag)
            //--    emberAfAppPrint(" ==> CURRECT TOTAL LEN:%d, time:%d\r\n",uart->len,halCommonGetInt32uMillisecondTick());
        }
        else
        {
            uint8 c;
            xxSerialPrint("UART-: --> ");
            while(rxBufLen)
            {
                if (EMBER_SUCCESS == emberSerialReadByte(com_num, &c))
                {
                    xxSerialPrint("%X ",c);
                    uart->buf[uart->tail] = c;
                    uart->len++;
                    uart->tail++;
                    uart->tail %= uart->size;
                }
                else
                {
                    break;
                }
                rxBufLen--;
            }
            xxSerialPrint("\r\n");
        
            /*volatile_temp = uart->tail;
            
            readStatus=emberSerialReadData(XX_SERIAL_AT_PORT_NUM, &uart->buf[volatile_temp], XX_SERIAL_RX_SIZE-volatile_temp, 0);
            if(readStatus!=EMBER_SUCCESS)
                return;

            //--if(!g_WsUartSendFlag)
            {
                emberAfAppPrint("\r\n ==> REC DATA2(%d): ",XX_SERIAL_RX_SIZE-volatile_temp);//--
                emberAfAppPrintBuffer(&uart->buf[volatile_temp], XX_SERIAL_RX_SIZE-volatile_temp, TRUE);
            }

            uart->tail=0;
            uart->len+=(XX_SERIAL_RX_SIZE-volatile_temp);

            readStatus=emberSerialReadData(XX_SERIAL_AT_PORT_NUM, uart->buf, rxBufLen-(XX_SERIAL_RX_SIZE-volatile_temp), 0);
            if(readStatus!=EMBER_SUCCESS)
                return;

            //--if(!g_WsUartSendFlag)
            {
                emberAfAppPrintBuffer(uart->buf, rxBufLen-(XX_SERIAL_RX_SIZE-volatile_temp), TRUE);
                emberAfAppPrint("\r\n");
            }

            uart->tail+=(rxBufLen-(XX_SERIAL_RX_SIZE-volatile_temp));
            uart->len+=(rxBufLen-(XX_SERIAL_RX_SIZE-volatile_temp));*/

            //if(!g_WsUartSendFlag)
            //--    emberAfAppPrint(" ==> CURRECT TOTAL LEN:%d,time:%d\r\n",uart->len,halCommonGetInt32uMillisecondTick());
        }
    }
}

void XxSerialPoll(uint32_t sysTimeMillisecond)
{
    //-- AT Command
    #ifdef XX_SERIAL_AT_PORT_NUM
    xxSerialReceive((void*)&serial_at, XX_SERIAL_AT_PORT_NUM);
    XxSerialATCommandPoll((void*)&serial_at, sysTimeMillisecond);
    #endif

    //-- Wifi Command
    #ifdef XX_SERIAL_WIFI_PORT_NUM
    xxSerialReceive((void*)&serial_wifi, XX_SERIAL_WIFI_PORT_NUM);
    XxSerialWifiCommandPoll((void*)&serial_wifi, sysTimeMillisecond);
    #endif
}

void XxSerialInit(void)
{
    xxSerialDataConfig(&serial_at, XX_SERIAL_AT_BUF_SIZE, serial_at_buf);
    xxSerialDataConfig(&serial_wifi, XX_SERIAL_WIFI_BUF_SIZE, serial_wifi_buf);
}

void XxMainStart2(void)
{
    XxMainInitCbRegister(XxSerialInit);
    //XxMainTickCbRegister(XxSerialPoll);
}

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


