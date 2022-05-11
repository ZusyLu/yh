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

#ifdef _XX_SERIAL_WIFI_H
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
#define xxSerialWifiPrint                                       emberAfAppPrint
#define xxSerialWifiPrintBuffer                                 emberAfAppPrintBuffer

//#define xxSerialWifiPrint(pstring, ...)
//#define xxSerialWifiPrintBuffer(x,y,z)

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/
uint8  *xx_serial_wifi_data_point_buf = 0;
XxSerialQueueDef *xx_serial_wifi_queue_head = 0;
uint8  xx_serial_wifi_queue_count = 0;
uint32 xx_serial_wifi_queue_system_time_backup = 0;
uint8  xx_serial_wifi_packet_index = 0x5A;
XxSerialWifiCommandCb_t xx_serial_wifi_command_cb = 0;
uint8  xx_serial_wifi_handshake_flag = 0;
uint16 xx_serial_rx_timeout = 0;

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
void XxSerialWifiCommandCbRegister(XxSerialWifiCommandCb_t f)
{
    xx_serial_wifi_command_cb = f;
}

void XxSerialWifiHandshakeClear(void)
{
    xx_serial_wifi_handshake_flag = 0;
}

uint8 XxSerialWifiHandshakeStatus(void)
{
    return xx_serial_wifi_handshake_flag;
}

void xxSerialWifiFrameSend(uint8 frameVersion, uint8 frameIndex, uint8 frameType, uint8 *data, uint16 len)
{
    uint8  buf[10];
    uint8  i = 0;
    uint16 j = 0;
    uint8  checksum = 0;

    if (XX_SERIAL_TEST_MODE_FLAG)
    {
        return;
    }

    buf[i++] = XX_SERIAL_WIFI_FRAME_SOF_1;
    buf[i++] = XX_SERIAL_WIFI_FRAME_SOF_2;
    buf[i++] = frameVersion;
    buf[i++] = frameIndex;
    buf[i++] = frameType;
    buf[i++] = (uint8)(len>>8);
    buf[i++] = (uint8)len;

    for (j = 0; j < i; j++)
    {
        checksum += buf[j];
    }

    for (j = 0;j < len; j++)
    {
        checksum += data[j];
    }

    if (frameType == XX_SERIAL_WIFI_FRAME_TYPE_ACK)
        xxSerialWifiPrint("WIFI : <-- ACK  : ");
    else
        xxSerialWifiPrint("WIFI : <-- DATA : ");

    for (j = 0; j < i; j++)
        xxSerialWifiPrint("%x ", buf[j]);

    for (j = 0; j < len; j++)
        xxSerialWifiPrint("%x ", data[j]);

    xxSerialWifiPrint("%x\r\n", checksum);

    emberSerialWriteData(XX_SERIAL_WIFI_PORT_NUM, buf, i);
    if (len)
        emberSerialWriteData(XX_SERIAL_WIFI_PORT_NUM, data, len);
    emberSerialWriteByte(XX_SERIAL_WIFI_PORT_NUM, checksum);
    emberSerialWaitSend(XX_SERIAL_WIFI_PORT_NUM);
}

uint8 XxSerialWifiFramePacket(uint8 frameVersion, uint8 frameIndex, uint8 frameType, uint8 *data, uint16 len, uint8 retryDelaySecond, uint8 retryCount)
{
    if (frameType == XX_SERIAL_WIFI_FRAME_TYPE_ACK)
    {
        xxSerialWifiFrameSend(frameVersion, frameIndex, frameType, data, len);
    }
    else
    {
        if (frameIndex == 0)
        {
            xx_serial_wifi_packet_index++;
            if (xx_serial_wifi_packet_index == 0)
                xx_serial_wifi_packet_index = 1;
            frameIndex = xx_serial_wifi_packet_index;
        }
        
        XxSerialQueueDef *msgpkt;
        msgpkt = (XxSerialQueueDef *)XxMemAlloc(len + sizeof(XxSerialQueueDef) + 4);
        
        if (!msgpkt)//-- 内存不够，申请不到空间
        {
            return 0;//-- 申请内存失败，返回。
        }

        msgpkt->next = NULL;
        msgpkt->frameIndex = frameIndex;
        msgpkt->frameType = frameType;
        msgpkt->dataLen = len;
        msgpkt->sendTimeout = 0;
        if (retryDelaySecond == 0)
        {
            msgpkt->retryDelayCount = 1000/XX_SERIAL_WIFI_TX_POLL_TIME_MILLISECOND;
        }
        else
        {
            msgpkt->retryDelayCount = retryDelaySecond*(1000/XX_SERIAL_WIFI_TX_POLL_TIME_MILLISECOND);
        }
        msgpkt->retryCount = retryCount;
        
        memcpy((uint8 *)(msgpkt+1), data, len);

        xx_serial_wifi_queue_count++;
        XxSerialQueuePush(&xx_serial_wifi_queue_head, msgpkt);
    }

    return frameIndex;
}

void XxSerialWifiFrameDelByIndex(uint8 frameIndex)
{
    XxSerialQueueDef *pkt_cur = xx_serial_wifi_queue_head;
    XxSerialQueueDef *pkt_pre = xx_serial_wifi_queue_head;
    XxSerialQueueDef *pkt_next;

    while(pkt_cur)
    {
        pkt_next = pkt_cur->next;
        
        if(pkt_cur->frameIndex == frameIndex)
        {
            //-- delete
            xx_serial_wifi_queue_count--;
            
            //-- 链表里的第一包
            if(pkt_pre == pkt_cur)
            {
                xx_serial_wifi_queue_head = pkt_next;
                pkt_pre = xx_serial_wifi_queue_head;
            }
            else
                pkt_pre->next = pkt_next;

            XxMemFree(pkt_cur);
            
            break;
        }

        pkt_pre = pkt_cur;
        pkt_cur = pkt_next;
    }
}

void XxSerialWifiFrameDelByFrameType(uint8 frameType)
{
    XxSerialQueueDef *pkt_cur = xx_serial_wifi_queue_head;
    XxSerialQueueDef *pkt_pre = xx_serial_wifi_queue_head;
    XxSerialQueueDef *pkt_next;

    while(pkt_cur)
    {
        pkt_next = pkt_cur->next;
        
        if(pkt_cur->frameType == frameType)
        {
            //-- delete
            xx_serial_wifi_queue_count--;
            
            //-- 链表里的第一包
            if(pkt_pre == pkt_cur)
            {
                xx_serial_wifi_queue_head = pkt_next;
                pkt_pre = xx_serial_wifi_queue_head;
            }
            else
                pkt_pre->next = pkt_next;

            XxMemFree(pkt_cur);
            
            break;
        }

        pkt_pre = pkt_cur;
        pkt_cur = pkt_next;
    }
}

void xxSerialWifiDealDataPoint(uint8 frameIndex, uint8 frameType, uint8 *data, uint16 len)
{
    uint16 i;
    uint8  cmd = 0xff;
    uint8  cmdOpt = 0xff;

    if (frameType == XX_SERIAL_WIFI_FRAME_TYPE_ZIGBEE)
    {
        if (!XxFuncDataPointCheck(data, len))
            return;

        uint8  dpID;
        //-- uint8  dpDataType;
        uint16 dpDataLen;
        
        for (i = 0; i < len;)
        {
            dpID = data[i + XX_DATAPOINT_TOKEN_OFFSET];
            //-- dpDataType = data[i + XX_DATAPOINT_DATA_TYPE_OFFSET];
            dpDataLen = BUILD_UINT16(data[i + XX_DATAPOINT_LEN_LO_OFFSET], data[i + XX_DATAPOINT_LEN_HI_OFFSET]);
            cmdOpt = data[i + XX_DATAPOINT_DATA_OFFSET];

            if (dpID > 0 && dpID < 0x80)
            {
                cmd = dpID;
                break;
            }

            i += XX_SERIAL_WIFI_DP_HEAD_LEN + dpDataLen;
        }
    }

    if(xx_serial_wifi_command_cb)
        xx_serial_wifi_command_cb(frameIndex, frameType, cmd, cmdOpt, data, len);
}

uint8 XxSerialWifiCommandPoll(void *com, uint32_t sysTimeMillisecond)
{
    XxSerialReceiveDef *uart = (XxSerialReceiveDef *)com;

    uint16 bufSize = uart->size;
    uint16 head;
    uint16 dataLen;
    uint16 i;
    uint8  checksum = 0;
    uint8  frameIndex;
    uint8  frameType;
    uint16 frameDataLen;
    uint8  retValue = 0;
    uint8  errPrintFlag = 0;
    
    //-- parse command
    while (uart->len >= XX_SERIAL_WIFI_FRAME_KEYWORD_LEN)
    {
        head = uart->head;
        if (uart->packetLen == 0)
        {
            if (uart->buf[head] != XX_SERIAL_WIFI_FRAME_SOF_1 || 
                uart->buf[(head + XX_SERIAL_WIFI_FRAME_SOF_2_OFFSET)%bufSize] != XX_SERIAL_WIFI_FRAME_SOF_2)
            {
                if (errPrintFlag == 0)
                {
                    errPrintFlag = 1;
                    xxSerialWifiPrint("WIFI : --> ERROR : keyword error, SOF:0x%X%X, version:%d\r\n", uart->buf[head], uart->buf[(head + XX_SERIAL_WIFI_FRAME_SOF_2_OFFSET)%bufSize], uart->buf[(head + XX_SERIAL_WIFI_FRAME_VERSION_OFFSET)%bufSize]);
                }
                
                if (uart->len > XX_SERIAL_WIFI_FRAME_KEYWORD_LEN)
                {
                    xxSerialWifiPrint("%X ", uart->buf[head%bufSize]);
                }
                else //-- uart->len == XX_SERIAL_WIFI_FRAME_KEYWORD_LEN
                {
                    for (uint8 k = 0; k < XX_SERIAL_WIFI_FRAME_KEYWORD_LEN; k++)
                    {
                        xxSerialWifiPrint("%X ", uart->buf[(head+k)%bufSize]);
                    }
                }

                uart->head++;
                uart->head %= bufSize;
                uart->len--;
                uart->packetLen = 0;
                
                continue;
            }

            xxSerialWifiPrint("\r\n");

            dataLen = uart->buf[(head+XX_SERIAL_WIFI_FRAME_LEN_HI_OFFSET)%bufSize];
            dataLen <<= 8;
            dataLen |= uart->buf[(head+XX_SERIAL_WIFI_FRAME_LEN_LO_OFFSET)%bufSize];

            uart->packetLen = XX_SERIAL_WIFI_FRAME_KEYWORD_LEN+dataLen;
            
            if (uart->len < uart->packetLen)
            {
                //-- xxSerialWifiPrint("WIFI : Wait more bytes 1, cur len:%u, wait len:%u\r\n", uart->len, uart->packetLen);
                break;
            }
        }
        else if (uart->len < uart->packetLen)
        {
            if (uart->lenBackup == uart->len)
            {
                xx_serial_rx_timeout++;
                if (xx_serial_rx_timeout >= 100)
                {
                    uart->head = 0;
                    uart->tail = 0;
                    uart->len = 0;
                    uart->lenBackup = 0;
                    uart->packetLen = 0;
                    xx_serial_rx_timeout = 0;
                    return 0;
                }
            }
            
            uart->lenBackup = uart->len;
            break;
        }

        xx_serial_rx_timeout = 0;

        //-- verify checksum
        for (i = 0; i < (uart->packetLen - 1); i++)
        {
            checksum += uart->buf[(head+i)%bufSize];
        }

        if (checksum != uart->buf[(head + uart->packetLen - 1) % bufSize])
        {
            xxSerialWifiPrint("WIFI : --> ERROR : checksum error, cal:0x%x, rec:0x%x\r\n", checksum, uart->buf[(head + uart->packetLen - 1) % bufSize]);
            xxSerialWifiPrint("WIFI : --> ");
            for (i = 0; i < (uart->packetLen - 1); i++)
            {
                xxSerialWifiPrint("%X ", uart->buf[(head+i)%bufSize]);
            }
            xxSerialWifiPrint("\r\n");

            uart->head++;
            uart->head %= bufSize;
            uart->len--;
            uart->packetLen = 0;
            
            continue;
        }

        xxSerialWifiPrint("WIFI : --> DATA : ");
        for(uint16 i = 0; i < uart->packetLen; i++)
        {
            xxSerialWifiPrint("%x ", uart->buf[(head + i) % bufSize]);
        }
        xxSerialWifiPrint("\r\n");

        frameIndex = uart->buf[(head + XX_SERIAL_WIFI_FRAME_INDEX_OFFSET) % bufSize];
        frameType = uart->buf[(head + XX_SERIAL_WIFI_FRAME_TYPE_OFFSET) % bufSize];
        frameDataLen = uart->buf[(head + XX_SERIAL_WIFI_FRAME_LEN_HI_OFFSET) % bufSize];
        frameDataLen <<= 8;
        frameDataLen |= uart->buf[(head + XX_SERIAL_WIFI_FRAME_LEN_LO_OFFSET) % bufSize];

        xx_serial_wifi_handshake_flag = 1;

        if (frameType == XX_SERIAL_WIFI_FRAME_TYPE_ACK)
        {
            //-- del packet
            xxSerialWifiPrint("WIFI : --> ACK, index : %d\r\n", frameIndex);
            XxSerialWifiFrameDelByIndex(frameIndex);
        }
        else
        {
            //-- ack
            xxSerialWifiFrameSend(XX_SERIAL_WIFI_FRAME_VERSION_2, frameIndex, XX_SERIAL_WIFI_FRAME_TYPE_ACK, 0, 0);

            if (xx_serial_wifi_data_point_buf)
            {
                XxMemFree(xx_serial_wifi_data_point_buf);
                xx_serial_wifi_data_point_buf = 0;
            }
            
            xx_serial_wifi_data_point_buf = XxMemAlloc(frameDataLen+4);
            if (xx_serial_wifi_data_point_buf)
            {
                //-- pick data point
                if ((head + XX_SERIAL_WIFI_FRAME_DATA_OFFSET + frameDataLen) < bufSize)
                {
                    memcpy(xx_serial_wifi_data_point_buf, uart->buf + head + XX_SERIAL_WIFI_FRAME_DATA_OFFSET, frameDataLen);
                }
                else
                {
                    if ((head + XX_SERIAL_WIFI_FRAME_DATA_OFFSET) < bufSize)
                    {
                        memcpy(xx_serial_wifi_data_point_buf, uart->buf + head + XX_SERIAL_WIFI_FRAME_DATA_OFFSET, bufSize - head - XX_SERIAL_WIFI_FRAME_DATA_OFFSET);
                        memcpy(xx_serial_wifi_data_point_buf + (bufSize - head - XX_SERIAL_WIFI_FRAME_DATA_OFFSET), uart->buf, frameDataLen - (bufSize - head - XX_SERIAL_WIFI_FRAME_DATA_OFFSET));
                    }
                    else
                    {
                        memcpy(xx_serial_wifi_data_point_buf, uart->buf + (head + XX_SERIAL_WIFI_FRAME_DATA_OFFSET)%bufSize, frameDataLen);
                    }
                }

                //-- deal data point
                xxSerialWifiDealDataPoint(frameIndex, frameType, xx_serial_wifi_data_point_buf, frameDataLen);
                XxMemFree(xx_serial_wifi_data_point_buf);
                xx_serial_wifi_data_point_buf = 0;
            }
        }

        //-- skip this packet
        uart->head += uart->packetLen;
        uart->head %= bufSize;
        uart->len -=  uart->packetLen;
        uart->packetLen = 0;

        //-- break out to deal other data
        retValue = 1;
        break;
    }

    //-- tx
    if (xx_serial_wifi_queue_head && DEC(xx_serial_wifi_queue_system_time_backup, sysTimeMillisecond) >= XX_SERIAL_WIFI_TX_POLL_TIME_MILLISECOND)
    {
        //-- if (DEC(xx_serial_wifi_queue_system_time_backup, sysTimeMillisecond) >= 500)
        //--     xxSerialWifiPrint("WIFI : SystemTimeMillisecond : %d\r\n", sysTimeMillisecond);
        
        xx_serial_wifi_queue_system_time_backup = sysTimeMillisecond;

        uint8 haveSendFlag = 0;
        XxSerialQueueDef *this = xx_serial_wifi_queue_head;
        while (this)
        {
            if (xx_serial_wifi_queue_head->sendTimeout == 0)
            {
                if (!haveSendFlag)
                {
                    haveSendFlag = 1;
                    xxSerialWifiFrameSend(XX_SERIAL_WIFI_FRAME_VERSION_2, xx_serial_wifi_queue_head->frameIndex, xx_serial_wifi_queue_head->frameType, (uint8 *)(xx_serial_wifi_queue_head+1), xx_serial_wifi_queue_head->dataLen);
                    xx_serial_wifi_queue_head->sendTimeout = xx_serial_wifi_queue_head->retryDelayCount;
                }
            }
            else
            {
                xx_serial_wifi_queue_head->sendTimeout--;

                if (xx_serial_wifi_queue_head->sendTimeout == 0)
                {
                    xx_serial_wifi_queue_head->retryCount--;
                    if (xx_serial_wifi_queue_head->retryCount == 0)
                    {
                        XxSerialQueueDef *next = xx_serial_wifi_queue_head->next;

                        xx_serial_wifi_queue_count--;
                        XxMemFree(xx_serial_wifi_queue_head);
                        xx_serial_wifi_queue_head = next;
                    }
                }
            }

            this = this->next;
        }
    }

    return retValue;
}

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


