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

#ifdef _XX_ZDO_PARSE_H
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
#define XX_ZDO_PARSE_NWK_ADDRESS_CLUSTER                                         0X8000
#define XX_ZDO_PARSE_ZDO_ENDPOINT                                                0
/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/

/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/

/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/


#if 0
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
    uint8_t i = 0;
    switch( incomingMessage->apsFrame->clusterId)
    {
      case XX_ZDO_PARSE_NWK_ADDRESS_CLUSTER:
           {
             if( incomingMessage->apsFrame->sourceEndpoint != XX_ZDO_PARSE_ZDO_ENDPOINT )
             {
                 return false;
             }

             for( i = 0; i < XX_IRQ_BSP_BUTTON_CUNT; i++ )
             {
                 halCommonGetIndexedToken(&Xx_project_scene_action, TOKEN_XX_SCENE, i);
                 if( !memcmp( &incomingMessage->message[2], Xx_project_scene_action.destMacId, XX_SMART_SIX_SWITCH_ALPHA_MAC_LED ) )
                 {
                     Xx_project_scene_action.destShortId = (uint16_t)( incomingMessage->message[10] | incomingMessage->message[11]<<8 );
                     halCommonSetIndexedToken( TOKEN_XX_SCENE, i, &Xx_project_scene_action);

                     return true;
                 }
             }
           }
           break;

    }
    emberAfCorePrintln("########### message is :");
    for( uint8_t i = 0; i < incomingMessage->msgLen; i++ )
    {
        emberAfCorePrint("%x ",incomingMessage->message[i]);
    }
    emberAfCorePrintln("\r\n %s %d %s\n",__FILE__,__LINE__,__func__);

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


