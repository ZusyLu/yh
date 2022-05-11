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

#ifdef _XX_SERIAL_AT_H
/***************************************************************************************************
*                                               NOTE
*                                               说明
***************************************************************************************************/
//-- source insight使用UTF-8解码，配合SimpliCity Studio开发环境，File->Reload As Encoding...->UTF-8
//-- source insight设置默认解码格式，Option->Preferences->Files->Default encoding->UTF-8

/*基于Manufacturing Library测试，首先要勾选插件“Manufacturing Library”，以下是jlink输入命令
1，打开生产测试库 plugin mfglib mfgenable 1
2，启动测试环境 plugin mfglib start 1
3，设置信道 plugin mfglib set-channel 11
4，设置发送模式为正常传输模式 plugin mfglib set-options 0
5，设置发射功率 plugin mfglib set-power 20 0 
6，查看设置状态 plugin mfglib status
6.2：启动发射功率 plugin mfglib tone start
7，发送数据
1）：发送特定内容数据的测试消息 plugin mfglib send message [data:-1] [numPackets:2]
     data - OCTET_STRING – 发送的16进制数据，最多16字节
     numPackets - INT16U - 要发送的数据包数量
     例如 plugin mfglib send message {112233445566778899} 1 //发送1包9字节数据包
     
2）：用随机数据发送一系列测试包 plugin mfglib send random [numPackets:2] [length:1]
     numPackets - INT16U -要发送的数据包数量
     length - INT8U –要发送数据包的包长
     例如 plugin mfglib send random 10 10
3）：发送一系列包含固定数据的测试包 plugin mfglib send test [numPackets:2] [length:1]
    numPackets - INT16U -要发送的数据包数量
    length - INT8U -要发送数据包的包长
    例如：plugin mfglib send test 10 10 */

/***************************************************************************************************
*                                             INCLUDES
*                                            系统头文件
***************************************************************************************************/

/***************************************************************************************************
*                                               MACRO
*                                               宏定义
***************************************************************************************************/
#define xxSerialATPrint                                     emberAfAppPrint
#define xxSerialATPrintln                                   emberAfAppPrintln

#define xxSerialATPrintBuffer                               emberAfAppPrintBuffer

//#define xxSerialATPrint(pstring, ...)
//#define xxSerialATPrintBuffer(x,y,z)

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/
#define XX_SERIAL_AT_COMMAND_NUM                            18
#define XX_SERIAL_AT_COMMAND_LEN                            128
#define XX_SERIAL_AT_COMMAND_START_TX                       0
#define XX_SERIAL_AT_COMMAND_START_RX                       1
#define XX_SERIAL_AT_COMMAND_STOP                           2
#define XX_SERIAL_AT_COMMAND_RESET                          3
#define XX_SERIAL_AT_COMMAND_SINGLE                         4
#define XX_SERIAL_AT_COMMAND_CHANNEL                        5
#define XX_SERIAL_AT_COMMAND_DATA                           6
#define XX_SERIAL_AT_COMMAND_PACKAGES                       7
#define XX_SERIAL_AT_COMMAND_COUNT                          8
#define XX_SERIAL_AT_COMMAND_POWER                          9
#define XX_SERIAL_AT_COMMAND_END                            18

#define XX_SERIAL_MFG_COMMAND_FACTORY                       10
#define XX_SERIAL_MFG_COMMAND_QUERY                         11
#define XX_SERIAL_MFG_COMMAND_GPIO                          12
#define XX_SERIAL_MFG_COMMAND_TRIAD                         13
#define XX_SERIAL_MFG_COMMAND_READ_TRIAD                    14
#define XX_SERIAL_MFG_COMMAND_READ_MAC                      15
#define XX_SERIAL_MFG_COMMAND_READ_VERSION                  16
#define XX_SERIAL_MFG_COMMAND_RF_TEST                       17





#define XX_SERIAL_MFG_COMMAND_TRIAD_STRING                  "AT+MFG_TEST"//"AT+MFG_TRIAD"



/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/
//-- 此表要和以上定义序号对应            
const char xx_serial_at_command_table[XX_SERIAL_AT_COMMAND_NUM][XX_SERIAL_AT_COMMAND_LEN]=
{
    {"RF_START=0"}, //-- TX
    {"RF_START=1"}, //-- RX
    {"RF_STOP"},
    {"RF_RESET"},
    {"RF_SINGLE"},
    {"RF_CHANNEL="},
    {"RF_DATA="},
    {"RF_PACKAGES="},
    {"RF_COUNT"},
    {"RF_POWER="},
    {"MFG_TEST"},
    {"MFG_QUERY"},
    {"MFG_GPIO"},
    {"MFG_TRIAD{\"PI\":\"00000000\",\"DS\":\"0000000000000000000\",\"DN\":\"000000000000\"}"}, //buff pi 16 ds 32 dn 59  
    {"MFG_READ_TRIAD"},
    {"MFG_READ_MAC"},
    {"MFG_READ_VERSION"},
    {"MFG_RF_TEST_RSSI_MIN_VALUE:-00"}
};

const unsigned char xx_serial_at_command_message[5][9]=
{
    {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99},
    {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99},
    {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99},
    {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99},
    {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99},
};

uint8 xx_message_packet_index = 0;

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
extern void emberAfMfglibStart(bool wantCallback);
extern void emberAfMfglibStop(void);
extern void emAfMfglibToneStartCommand(void);
extern void emAfMfglibStatusCommand(void);
extern EmberStatus mfglibSetChannel(uint8_t channel);
extern EmberStatus mfglibSetPower(uint16_t txPowerMode, int8_t power);
extern EmberStatus mfglibSendPacket(uint8_t * packet, uint16_t repeat);

extern uint8 serial_at_buf[];

/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
uint8 XxSerialATCommandPoll(void *com, uint32_t sysTimeMillisecond)
{
    uint8 cmdLen;
    uint8 cmdOffset = 0xff;
    uint8 argOffset = 0;
    uint8 endOffset = 0;
    char *ret = NULL;
    char testLen=0;
    XxSerialReceiveDef *uart = (XxSerialReceiveDef *)com;
    #ifdef XX_SLEEPY_END_DEVICE
        ret = strstr(uart->buf,"TEST");
        if ( ret != NULL && uart->len > 8 )
        {
            XX_SERIAL_TEST_MODE_FLAG_SET;
            if(emberNetworkState() != EMBER_NO_NETWORK)
            {
                emberLeaveNetwork();
            }
            
            xxSerialATPrintln("xx will into mfg mode");
            uart->head = 0;
            uart->tail = 0;
            uart->len = 0;
            memset( serial_at_buf, '\0', XX_SERIAL_AT_BUF_SIZE );
            //uart->buf = '\0';
            //Xx_project_sleepy_end_device_flat = true;
            return 1;
        }
        //else
    #endif
    
    if (uart->len > XX_SERIAL_AT_PACKET_KEYWORD_LEN)
    {
        uint8 i;
        uint8 j;
        for (i=0;i<uart->len;i++)
        {
            if (uart->buf[i] != 0x0d)
                continue;

            for (j=i;j>0;j--)
            {
                if(uart->buf[j] == XX_SERIAL_AT_PACKET_SOF2 && uart->buf[j-1] == XX_SERIAL_AT_PACKET_SOF1)
                {
                    cmdOffset = j+2;//-- 跳到+后面
                    cmdLen = i - cmdOffset;
                    endOffset = i;
                    //-- emberAfAppPrint("\r\nAT Command find\r\n");
                }
            }
        }
    }

    if (cmdOffset!=0xff)
    {
        uint8 i;        
        for (i = 0; i < XX_SERIAL_AT_COMMAND_NUM; i++)
        {
            if (cmdLen < strlen(xx_serial_at_command_table[i]))
            {
                continue;
            }
                
                
            if (0==memcmp(uart->buf+cmdOffset, xx_serial_at_command_table[i], strlen(xx_serial_at_command_table[i])))            
            {
                argOffset = cmdOffset + strlen(xx_serial_at_command_table[i]);
                break;
            }
            else if (0==memcmp(uart->buf+cmdOffset, xx_serial_at_command_table[i], 10))
            {
                argOffset = cmdOffset + strlen(xx_serial_at_command_table[i]);
                break;
            }
            else if (0==memcmp(uart->buf+cmdOffset, xx_serial_at_command_table[i], 28))
            {
                argOffset = cmdOffset + strlen(xx_serial_at_command_table[i]);
                break;
            }
        

        }

        if(i < XX_SERIAL_AT_COMMAND_END)
        {            
            XX_SERIAL_TEST_MODE_FLAG_SET;
            if(emberNetworkState() != EMBER_NO_NETWORK)
            {
                emberLeaveNetwork();
            }
        }

        switch (i)
        {
        case XX_SERIAL_AT_COMMAND_START_TX:
            xxSerialATPrint("AT Command : start tx\r\n");
            xx_message_packet_index = 0;
            emberAfMfglibStart(0);
            break;

        case XX_SERIAL_AT_COMMAND_START_RX:
            xxSerialATPrint("AT Command : start rx\r\n");
            emberAfMfglibStart(1);
            break;

        case XX_SERIAL_AT_COMMAND_STOP:
            xxSerialATPrint("AT Command : stop\r\n");
            xx_message_packet_index = 0;
            emberAfMfglibStop();
            break;

        case XX_SERIAL_AT_COMMAND_RESET:
            xxSerialATPrint("AT Command : please use start command instead of reset\r\n");
            break;

        case XX_SERIAL_AT_COMMAND_SINGLE:
            xxSerialATPrint("AT Command : set single mode\r\n");
            emAfMfglibToneStartCommand();
            break;

        case XX_SERIAL_AT_COMMAND_CHANNEL:
            {
                uint32 number;
                
                if((endOffset-argOffset) != 2)
                {
                    xxSerialATPrint("argument error\r\n");
                    break;
                }

                if(!XxFuncStr2Num((char*)uart->buf+argOffset, &number, 2))
                {
                    xxSerialATPrint("argument error\r\n");
                    break;
                }

                xxSerialATPrint("AT Command : set channel %d\r\n", number);
                EmberStatus status = mfglibSetChannel(number);
                xxSerialATPrint("%p set channel, status 0x%X", "mfglib", status);
                (void)status;
            }
            break;

        case XX_SERIAL_AT_COMMAND_DATA:
            {
                uint8 index;
                
                if ((endOffset-argOffset) != 1)
                {
                    xxSerialATPrint("argument len error\r\n");
                    break;
                }

                if (!XxFuncAsc2Hex(uart->buf[argOffset], &index))
                {
                    xxSerialATPrint("argument error\r\n");
                    break;
                }

                if (index > 4)
                {
                    xxSerialATPrint("argument error\r\n");
                    break;
                }

                xxSerialATPrint("AT Command : set message index %d\r\n", index);
                xx_message_packet_index = index;
            }
            break;

        case XX_SERIAL_AT_COMMAND_PACKAGES:
            {
                unsigned long number;
                uint8 sendBuf[16];
                
                if ((endOffset-argOffset) != 4 && (endOffset-argOffset) != 5)
                {
                    xxSerialATPrint("argument len error\r\n");
                    break;
                }

                if (!XxFuncStr2Num((char*)uart->buf+argOffset, &number, endOffset-argOffset))
                {
                    xxSerialATPrint("argument num error\r\n");
                    break;
                }

                if (number < 1000)
                {
                    xxSerialATPrint("argument error\r\n");
                    break;
                }

                //-- 重发次数，先发一次，再重发，所以此处次数减1
                number--;
                
                xxSerialATPrint("AT Command : set packages %d\r\n", number+1);
            
                sendBuf[0] = 9 + 2; // message length plus 2-byte CRC
                MEMMOVE(sendBuf + 1, xx_serial_at_command_message[xx_message_packet_index], 9);
                EmberStatus status = mfglibSendPacket(sendBuf, number);
                xxSerialATPrint("%p send message, status 0x%X", "mfglib", status);
                (void)status;
            }
            break;

        case XX_SERIAL_AT_COMMAND_COUNT:
            xxSerialATPrint("AT Command : read status\r\n");
            emAfMfglibStatusCommand();
            break;

        case XX_SERIAL_AT_COMMAND_POWER:
            {
                uint8 signFlag = 0;
                int8 power;

                if (uart->buf[argOffset] == '-')
                {
                    signFlag = 1;
                    argOffset++;
                }

                if ((endOffset-argOffset) > 2)
                {
                    xxSerialATPrint("argument len error\r\n");
                    break;
                }

                if (!XxFuncStr2Num((char*)uart->buf+argOffset, (unsigned long*)&power, endOffset-argOffset))
                {
                    xxSerialATPrint("argument num error\r\n");
                    break;
                }

                if (signFlag)
                {
                    power = -power;
                }
                    
                xxSerialATPrint("AT Command : set power %d\r\n", power);
                EmberStatus status = mfglibSetPower(0, power);
                xxSerialATPrint("%p set power and mode, status 0x%X", "mfglib", status);
                (void)status;
            }
            break;

            case XX_SERIAL_MFG_COMMAND_FACTORY:
            {
                //xxSerialATPrint("AT Command : XX_SERIAL_MFG_COMMAND_FACTORY\r\n");
            }
            break;

            case XX_SERIAL_MFG_COMMAND_QUERY:
            {
                xxSerialATPrint("AT+MFG_IN_MFG\r\n");
            }
            break;

            case XX_SERIAL_MFG_COMMAND_GPIO:
            {
                //xxSerialATPrint("AT Command : XX_SERIAL_MFG_COMMAND_GPIO\r\n");
                XxDelayEventSetEvent(XX_MFG_GPIO_TEST_INIT_EVENT, XX_MFG_GPIO_TEST_INIT_EVENT_TIME, true);
            }
            break;

            case XX_SERIAL_MFG_COMMAND_TRIAD:
            {
                //xxSerialATPrint("AT Command : XX_SERIAL_MFG_COMMAND_TRIAD\r\n");
                
//                xxSerialATPrint("pi= ");
//                xxSerialATPrintBuffer(uart->buf+cmdOffset+16,8,true);
//                xxSerialATPrint("\r\n");
                memcpy(&Xx_project_triple.productId[1], uart->buf+cmdOffset+16, 8);
                
//                xxSerialATPrint("ds= ");
//                xxSerialATPrintBuffer(uart->buf+cmdOffset+32,19,true);
//                xxSerialATPrint("\r\n");
                memcpy(&Xx_project_triple.deviceId[1], uart->buf+cmdOffset+32, 19);
                
//                xxSerialATPrint("dn= ");
//                xxSerialATPrintBuffer(uart->buf+cmdOffset+59,12,true);
//                xxSerialATPrint("\r\n");
                memcpy(&Xx_project_triple.deviceName[1], uart->buf+cmdOffset+59, 12);
                
                XxDelayEventSetEvent(XX_MFG_ERASS_FLASH_EVENT, XX_MFG_ERASS_FLASH_EVENT_TIME, true);
            }
            break;

            case XX_SERIAL_MFG_COMMAND_READ_TRIAD:
            {
                xxSerialATPrint("AT Command : XX_SERIAL_MFG_COMMAND_READ_TRIAD\r\n");
                XxDelayEventSetEvent(XX_MFG_READ_TRIAD_EVENT, XX_MFG_READ_TRIAD_EVENT_TIME, true);
            }
            break;

            case XX_SERIAL_MFG_COMMAND_READ_MAC:
            {
                xxSerialATPrint("AT Command : XX_SERIAL_MFG_COMMAND_READ_MAC\r\n");
                XxDelayEventSetEvent(XX_MFG_READ_MAC_EVENT, XX_MFG_READ_MAC_EVENT_TIME, true);
            }
            break;

            case XX_SERIAL_MFG_COMMAND_READ_VERSION:
            {
                xxSerialATPrint("AT Command : XX_SERIAL_MFG_COMMAND_READ_VERSION\r\n");
                XxDelayEventSetEvent(XX_MFG_READ_version_EVENT, XX_MFG_READ_version_EVENT_TIME, true);
            }
            break;

            case XX_SERIAL_MFG_COMMAND_RF_TEST:
            {
                XxFuncStr2Num(&uart->buf[cmdOffset+28],(unsigned long *)&xx_mfg_rssi,2);
                xx_mfg_rssi = -xx_mfg_rssi;
                xxSerialATPrint("rssi is %d \r\n",xx_mfg_rssi);
                if( mfglibSetChannel(11) != EMBER_SUCCESS)
                {
                    emberAfCorePrintln("AT_MFG_RF_RSSI_SET_ERROR");
                }
                else
                {
                    emberAfCorePrintln("AT_MFG_RF_RSSI_SET_OK");
                }
                //XxDelayEventSetEvent(XX_MFG_READ_version_EVENT, XX_MFG_READ_version_EVENT_TIME, true);
            }
            break;
            
            default:
            {
                
            }
            
        }

        //-- 清除接收到的数据
        uart->head = 0;
        uart->tail = 0;
        uart->len = 0;
        return 1;
    }
    
    return 0;
}

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


