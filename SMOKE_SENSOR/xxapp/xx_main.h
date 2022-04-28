/**************************************************************************************************
  Filename:       xx_example.h
  Revised:        $Date: 202-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.3.0(2 stacks)

  Copyright 2022 lu.                                .
***************************************************************************************************/
#ifndef _XX_MAIN_H
#define _XX_MAIN_H

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
#include "app/framework/include/af.h"
#include "app/framework/util/util.h"
#include "app/framework/util/common.h"
#include "hal/hal.h"
#include EMBER_AF_API_NETWORK_STEERING
#include "app/util/ezsp/ezsp-enum.h"
#include "stack/include/ember-types.h"
#include "enums.h"
#include "common.h"

#include "app/framework/plugin/ota-common/ota.h"


/***************************************************************************************************
*                                             INCLUDES
*                                            用户头文件
***************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "xx_defines.h"
//#include "xx_button/xx_irq_button.h"
//#include "xx_button/xx_irq_button_handle.h"
//#include "xx_blink_led/xx_blink_led.h"

#include "xx_common/xx_ias.h"


//#if 0

#include "xx_project/xx_project.h"
//#include "xx_project/xx_alpha_cluster.h"


#include "xx_nwk/xx_nwk.h"
#include "xx_common/xx_zcl_cmd.h"
#include "xx_common/xx_zcl_parse.h"
#include "xx_common/xx_zdo_parse.h"
#include "xx_common/xx_common_function.h"
#include "xx_memory/xx_memory.h"
#include "xx_common/xx_mfg.h"
#include "xx_common/xx_delay_event.h"

#include "xx_common/xx_power_configuration.h"
#include "xx_common/xx_heart_beat.h"
#include "xx_common/xx_diagnostics.h"
#include "xx_serial/xx_serial.h"
#include "xx_serial/xx_serial_at.h"
#include "xx_serial/xx_serial_smoke.h"


//#endif

#if 1

/***************************************************************************************************
*                                          
*                                              
***************************************************************************************************/
typedef void (*XxMainInitCb_t)(void);
typedef void (*XxMainTickCb_t)(uint32 sysTimeMillisecond);
typedef void (*XxStackStatusCb_t)(EmberStatus status);

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                              常量定义
***************************************************************************************************/


/***************************************************************************************************
*                                             FUNCTION
*                                             全局函数
***************************************************************************************************/

extern uint8 XxMainInitCbRegister(XxMainInitCb_t f);
extern uint8 XxMainTickCbRegister(XxMainTickCb_t f);
extern void  XxMainStackStatusCbRegister(XxStackStatusCb_t f);
extern void XxMainStart1(void);

#endif

/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


