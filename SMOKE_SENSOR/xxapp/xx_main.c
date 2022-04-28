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

#ifdef _XX_MAIN_H

/***************************************************************************************************
*                                               NOTE
*                                               说明
***************************************************************************************************/
//-- source insight使用UTF-8解码，配合SimpliCity Studio开发环境，File->Reload As Encoding...->UTF-8
//-- source insight设置默认解码格式，Option->Preferences->Files->Default encoding->UTF-8

/*
一，基于Manufacturing Library测试，首先要勾选插件“Manufacturing Library”，以下是jlink输入命令
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
    例如：plugin mfglib send test 10 10 *

 二，关于OTA失败，需要在SDK里面更改该文件：
    1， Set the macro
"SLOT_MANAGER_VERIFICATION_CONTEXT_SIZE" to
"BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE"
in \util\plugin\slot-manager\slot-manager.h. This must be modified
in the SDK directly
2，jixian ota mechanism
SDK:  ota.h中添加
#define XX_OTA_MECHANISM  

三 /*
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
#define XxMainPrintString                                 "MAIN :"

//#define xxMainPrint                                     emberAfAppPrint
//#define xxMainPrintBuffer                               emberAfAppPrintBuffer

#define xxMainPrint(pstring, ...)
#define xxMainPrintBuffer(x,y,z)

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                              全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                              局部变量
***************************************************************************************************/
//EmberEventControl wifiHandShakeEventControl;

XxMainInitCb_t  xxMainInitCb1 = NULL;
XxMainInitCb_t  xxMainInitCb2 = NULL;
XxMainInitCb_t  xxMainInitCb3 = NULL;
XxMainInitCb_t  xxMainInitCb4 = NULL;
XxMainInitCb_t  xxMainInitCb5 = NULL;
XxMainInitCb_t  xxMainInitCb6 = NULL;
XxMainInitCb_t  xxMainInitCb7 = NULL;
XxMainInitCb_t  xxMainInitCb8 = NULL;
XxMainInitCb_t  xxMainInitCb9 = NULL;
XxMainInitCb_t  xxMainInitCb10 = NULL;

XxMainTickCb_t  xxMainTickCb1 = NULL;
XxMainTickCb_t  xxMainTickCb2 = NULL;
XxMainTickCb_t  xxMainTickCb3 = NULL;
XxMainTickCb_t  xxMainTickCb4 = NULL;
XxMainTickCb_t  xxMainTickCb5 = NULL;
XxMainTickCb_t  xxMainTickCb6 = NULL;
XxMainTickCb_t  xxMainTickCb7 = NULL;
XxMainTickCb_t  xxMainTickCb8 = NULL;
XxMainTickCb_t  xxMainTickCb9 = NULL;
XxMainTickCb_t  xxMainTickCb10 =NULL;

XxStackStatusCb_t xxMainStackStatusCb = NULL;


/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/
uint8 XxMainInitCbRegister(XxMainInitCb_t f)
{
    if (xxMainInitCb1 == NULL)
    {
        xxMainInitCb1 = f;
    }
    else if (xxMainInitCb2 == NULL)
    {
        xxMainInitCb2 = f;
    }
    else if (xxMainInitCb3 == NULL)
    {
        xxMainInitCb3 = f;
    }
    else if (xxMainInitCb4 == NULL)
    {
        xxMainInitCb4 = f;
    }
    else if (xxMainInitCb5 == NULL)
    {
        xxMainInitCb5 = f;
    }
    else if (xxMainInitCb6 == NULL)
    {
        xxMainInitCb6 = f;
    }
    else if (xxMainInitCb7 == NULL)
    {
        xxMainInitCb7 = f;
    }
    else if (xxMainInitCb8 == NULL)
    {
        xxMainInitCb8 = f;
    }
    else if (xxMainInitCb9 == NULL)
    {
        xxMainInitCb9 = f;
    }
    else if (xxMainInitCb10 == NULL)
    {
        xxMainInitCb10 = f;
    }
    else
    {
        return false;
    }

    return true;
}

uint8 XxMainTickCbRegister(XxMainTickCb_t f)
{
    if (xxMainTickCb1 == NULL)
    {
        xxMainTickCb1 = f;
    }
    else if (xxMainTickCb2 == NULL)
    {
        xxMainTickCb2 = f;
    }
    else if (xxMainTickCb3 == NULL)
    {
        xxMainTickCb3 = f;
    }
    else if (xxMainTickCb4 == NULL)
    {
        xxMainTickCb4 = f;
    }
    else if (xxMainTickCb5 == NULL)
    {
        xxMainTickCb5 = f;
    }
    else if (xxMainTickCb6 == NULL)
    {
        xxMainTickCb6 = f;
    }
    else if (xxMainTickCb7 == NULL)
    {
        xxMainTickCb7 = f;
    }
    else if (xxMainTickCb8 == NULL)
    {
        xxMainTickCb8 = f;
    }
    else if (xxMainTickCb9 == NULL)
    {
        xxMainTickCb9 = f;
    }
    else if (xxMainTickCb10 == NULL)
    {
        xxMainTickCb10 = f;
    }
    else
    {
        return false;
    }

    return true;
}

void XxMainStackStatusCbRegister(XxStackStatusCb_t f)
{
    xxMainStackStatusCb = f;
}

/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 *
 */
 #if 1
void emberAfMainTickCallback(void)
{
    uint32 systemTimeMillisecond = halCommonGetInt32uMillisecondTick();
    if (xxMainTickCb1)
        xxMainTickCb1(systemTimeMillisecond);

    if (xxMainTickCb2)
        xxMainTickCb2(systemTimeMillisecond);
        
    if (xxMainTickCb3)
        xxMainTickCb3(systemTimeMillisecond);
        
    if (xxMainTickCb4)
        xxMainTickCb4(systemTimeMillisecond);
        
    if (xxMainTickCb5)
        xxMainTickCb5(systemTimeMillisecond);

    if (xxMainTickCb6)
        xxMainTickCb6(systemTimeMillisecond);

    if (xxMainTickCb7)
        xxMainTickCb7(systemTimeMillisecond);
        
    if (xxMainTickCb8)
        xxMainTickCb8(systemTimeMillisecond);
        
    if (xxMainTickCb9)
        xxMainTickCb9(systemTimeMillisecond);
        
    if (xxMainTickCb10)
        xxMainTickCb10(systemTimeMillisecond);
}
#endif
/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
 extern EmberEventControl xxInitEvent;
#if 1
void emberAfMainInitCallback(void)
{
    if (xxMainInitCb1)
        xxMainInitCb1();

    if (xxMainInitCb2)
        xxMainInitCb2();

    if (xxMainInitCb3)
        xxMainInitCb3();

    if (xxMainInitCb4)
        xxMainInitCb4();

    if (xxMainInitCb5)
        xxMainInitCb5();

    if (xxMainInitCb6)
        xxMainInitCb6();

    if (xxMainInitCb7)
        xxMainInitCb7();

    if (xxMainInitCb8)
        xxMainInitCb8();

    if (xxMainInitCb9)
        xxMainInitCb9();

    if (xxMainInitCb10)
        xxMainInitCb10();
}
#endif
/** @brief Main Start
*
* This function is called at the start of main after the HAL has been
* initialized.  The standard main function arguments of argc and argv are
* passed in.  However not all platforms have support for main() function
* arguments.  Those that do not are passed NULL for argv, therefore argv should
* be checked for NULL before using it.  If the callback determines that the
* program must exit, it should return true.  The value returned by main() will
* be the value written to the returnCode pointer.  Otherwise the callback
* should return false to let normal execution continue.
*
* @param returnCode   Ver.: always
* @param argc   Ver.: always
* @param argv   Ver.: always
*/
#if 1
boolean emberAfMainStartCallback(int* returnCode, int argc, char** argv)
{
    // NOTE:  argc and argv may not be supported on all platforms, so argv MUST be
    // checked for NULL before referencing it.  On those platforms without argc 
    // and argv "0" and "NULL" are passed respectively.
    
    xxMainPrint("%p emberAfMainStartCallback()\r\n", XxMainPrintString);

    #ifdef XX_MAIN_START_1
    xxMainPrint("%p XxMainStart1()\r\n", XxMainPrintString);
    XxMainStart1();
    #endif

    #ifdef XX_MAIN_START_2
    xxMainPrint("%p XxMainStart2()\r\n", XxMainPrintString);
    XxMainStart2();
    #endif

    #ifdef XX_MAIN_START_3
    xxMainPrint("%p XxMainStart3()\r\n", XxMainPrintString);
    XxMainStart3();
    #endif

    #ifdef XX_MAIN_START_4
    xxMainPrint("%p XxMainStart4()\r\n", XxMainPrintString);
    XxMainStart4();
    #endif

    #ifdef XX_MAIN_START_5
    xxMainPrint("%p XxMainStart5()\r\n", XxMainPrintString);
    XxMainStart5();
    #endif

    #ifdef XX_MAIN_START_6
    xxMainPrint("%p XxMainStart6()\r\n", XxMainPrintString);
    XxMainStart6();
    #endif

    #ifdef XX_MAIN_START_7
    xxMainPrint("%p XxMainStart7()\r\n", XxMainPrintString);
    XxMainStart7();
    #endif

    #ifdef XX_MAIN_START_8
    xxMainPrint("%p XxMainStart8()\r\n", XxMainPrintString);
    XxMainStart8();
    #endif

    #ifdef XX_MAIN_START_9
    xxMainPrint("%p XxMainStart9()\r\n", XxMainPrintString);
    XxMainStart9();
    #endif

    #ifdef XX_MAIN_START_10
    xxMainPrint("%p XxMainStart10()\r\n", XxMainPrintString);
    XxMainStart10();
    #endif

    return false;  // exit?
}

#endif
/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
 #if 1
boolean emberAfStackStatusCallback(EmberStatus status)
{

    if(xxMainStackStatusCb)
        xxMainStackStatusCb(status);
    switch(status)
    {
    case EMBER_NETWORK_UP:
        xxMainPrint("%p EMBER_NETWORK_UP\r\n", XxMainPrintString);
        break;

    case EMBER_NETWORK_DOWN:
        xxMainPrint("%p EMBER_NETWORK_DOWN and will from network\r\n", XxMainPrintString);
        //-- emberEventControlSetDelayMS(wifiHandShakeEventControl, XX_PROTOCL_WIFI_TIMED_HANDSHAK);
        break;
    }
    
    return false;
}
#endif
/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif
