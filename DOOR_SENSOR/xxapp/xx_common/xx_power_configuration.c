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

#ifdef _XX_POWER_CONFIGURATION_H
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
#define xxPowerConfigurationPrint                                   emberAfAppPrint
#define xxPowerConfigurationPrintln                                 emberAfAppPrintln
//#define XX_POWER_VONFIGURATION_CHANGED_VOLTAGEMILLIV_REPORT         220

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
static void xxPowerConfigurationAttributeReport( uint16_t attributeId )
{
    uint16_t readA[1];
	readA[0] = attributeId;
	
    XxReportSpecificAttributeEx( ZCL_POWER_CONFIG_CLUSTER_ID, 1,readA,1,1,0x0000);
}
    
static void xxPowerConfigurationAttributeChangedReport( uint16_t attributeId )
{
    static uint16_t batteryVoltage=0;
    if ( batteryVoltage > halGetBatteryVoltageMilliV() )
    {
    	#ifdef XX_POWER_VONFIGURATION_CHANGED_VOLTAGEMILLIV_REPORT
	        if ( (batteryVoltage - halGetBatteryVoltageMilliV()) >= XX_POWER_VONFIGURATION_CHANGED_VOLTAGEMILLIV_REPORT )
	        {
	            xxPowerConfigurationAttributeReport( attributeId );
	        }
		#else 
			xxPowerConfigurationAttributeReport( attributeId );
		#endif
    }
    else
    {
        batteryVoltage = halGetBatteryVoltageMilliV();
    }
    
    batteryVoltage = halGetBatteryVoltageMilliV();
}



void emberAfPowerConfigClusterServerAttributeChangedCallback(int8u endpoint,
                                                             EmberAfAttributeId attributeId)
{
    //xxPowerConfigurationPrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
}

EmberAfStatus emberAfPowerConfigClusterServerPreAttributeChangedCallback(int8u endpoint,
                                                                      EmberAfAttributeId attributeId,
                                                                      EmberAfAttributeType attributeType,
                                                                      int8u size,
                                                                      int8u *value)
{
    xxPowerConfigurationPrintln("%s %d %s\n",__FILE__,__LINE__,__func__);
    xxPowerConfigurationPrintln("arrtibuteId =%2x ", attributeId );
    if ( ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID == attributeId || ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID == attributeId )
    {
        #ifdef XX_POWER_VONFIGURATION_CHANGED_VOLTAGEMILLIV_REPORT
            xxPowerConfigurationAttributeChangedReport( attributeId );
        #else
            xxPowerConfigurationAttributeReport( attributeId );
        #endif

    }
    
return EMBER_ZCL_STATUS_SUCCESS;
}












/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


