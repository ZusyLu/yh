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

#ifdef _XX_MFG_H
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
#define xxMfgPrintln                                  emberAfAppPrintln
    
#define xxMfgPrintBuffer                              emberAfAppPrintBuffer

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/
#define GPIO_TEST_NUMBER                        5
/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/
//uint32_t gpioNumber[GPIO_TEST_NUMBER]={PORTA_PIN(0),PORTA_PIN(3),PORTA_PIN(4),\
//                                         PORTB_PIN(0),PORTB_PIN(1),\
//                                         PORTC_PIN(0),PORTC_PIN(1),PORTC_PIN(2),\
//                                         PORTD_PIN(0),PORTD_PIN(1)};

static uint8_t gpio_test_HL=0;
static uint8_t gpio_test_model=1;//1 is xx_test_gpio[0] output model ,0 is xx_test_gpio[1] output model
static uint8_t gpio_test_cunt=0;
int8_t xx_mfg_rssi = 0;

typedef struct
{
    uint32_t gpioPort;
    unsigned int pin;
    uint32_t gpioN;
    char *name;
}gpioType;

gpioType xx_test_gpio[2][GPIO_TEST_NUMBER] = {
                                                {
                                                    {gpioPortC,1,PORTC_PIN(1),"PC01"},\
                                                    {gpioPortC,0,PORTC_PIN(0),"PC00"},\
                                                    {gpioPortA,0,PORTA_PIN(0),"PA00"},\
                                                    {gpioPortA,3,PORTA_PIN(3),"PA03"},
                                                    {gpioPortA,4,PORTA_PIN(4),"PA04"},\
                                                },\
                                                {
                                                    {gpioPortB,1,PORTB_PIN(1),"PB01"},\
                                                    {gpioPortB,0,PORTB_PIN(0),"PB00"},\
                                                    {gpioPortC,2,PORTC_PIN(2),"PC02"},\
                                                    {gpioPortD,0,PORTD_PIN(0),"PD00"},\
                                                    {gpioPortD,1,PORTD_PIN(1),"PD01"}
                                                }
                                            };

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

void xxMfgInitFunction(bool gpioHL,bool inOrOutPutModel)//0 is in put model  1 is out put model  for xx_test_gpio[0];
{
    CMU_ClockEnable(cmuClock_GPIO, false); //GPIO clock enable
    //GPIOINT_Init();  GPIO interrupt init
    uint8_t i=0;
    //GPIO output mode  for xx_test_gpio[0];  PA00 ...
    for(i=0;i<GPIO_TEST_NUMBER;i++)
    {
        if(inOrOutPutModel)
        {
            GPIO_PinModeSet(xx_test_gpio[0][i].gpioPort, xx_test_gpio[0][i].pin, gpioModePushPull,0);
            GPIO_PinModeSet(xx_test_gpio[1][i].gpioPort, xx_test_gpio[1][i].pin, gpioModeInput,0);

            if(gpioHL)
            {
                if((i%2) == 0)
                {
                    GPIO_PinOutSet(xx_test_gpio[0][i].gpioPort, xx_test_gpio[0][i].pin);
                }
                else
                {
                    GPIO_PinOutClear(xx_test_gpio[0][i].gpioPort, xx_test_gpio[0][i].pin);
                }
            }
            else
            {
                if((i%2) == 1)
                {
                    GPIO_PinOutSet(xx_test_gpio[0][i].gpioPort, xx_test_gpio[0][i].pin);
                }
                else
                {
                    GPIO_PinOutClear(xx_test_gpio[0][i].gpioPort, xx_test_gpio[0][i].pin);
                }
            }

        }
        else
        {
            GPIO_PinModeSet(xx_test_gpio[1][i].gpioPort, xx_test_gpio[1][i].pin, gpioModePushPull,0);
            GPIO_PinModeSet(xx_test_gpio[0][i].gpioPort, xx_test_gpio[0][i].pin, gpioModeInput,0);

            if(gpioHL)
            {
                if((i%2) == 0)
                {
                    GPIO_PinOutSet(xx_test_gpio[1][i].gpioPort, xx_test_gpio[1][i].pin);
                }
                else
                {
                    GPIO_PinOutClear(xx_test_gpio[1][i].gpioPort, xx_test_gpio[1][i].pin);
                }
            }
            else
            {
                if((i%2) == 1)
                {
                    GPIO_PinOutSet(xx_test_gpio[1][i].gpioPort, xx_test_gpio[1][i].pin);
                }
                else
                {
                    GPIO_PinOutClear(xx_test_gpio[1][i].gpioPort, xx_test_gpio[1][i].pin);
                }
            }
        }

    }

    CMU_ClockEnable(cmuClock_GPIO, true);
    
}

uint8_t xxMfgGpioTestFunction()
{
    uint8_t i=0;
    for(i=0;i<GPIO_TEST_NUMBER;i++)
    {
       if(gpio_test_model)//PC01 is high output 
       {
            if( halGpioRead(xx_test_gpio[1][i].gpioN) )//halGpioRead(0)
            {
                if(gpio_test_HL == 1 && (i%2) ==1)
                {
                    xxMfgPrintln("AT+MFG_GPIO_TEST_LOW_FAIL_(%s) ",xx_test_gpio[1][i].name);
                    XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event 
                    return 0;
                }

                if(gpio_test_HL == 0 && (i%2) ==0)
                {
                    xxMfgPrintln("AT+MFG_GPIO_TEST_LOW_FAIL_(%s) ",xx_test_gpio[1][i].name);
                    XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event 
                    return 0;
                }
            
            }
            else
            {
                if(gpio_test_HL == 0 && (i%2) ==1)
                {
                    xxMfgPrintln("AT+MFG_GPIO_TEST_HIGH_FAIL_(%s) ",xx_test_gpio[1][i].name);
                    XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event               
                    return 0;
                } 

                if(gpio_test_HL == 1 && (i%2) ==0)
                {
                    xxMfgPrintln("AT+MFG_GPIO_TEST_HIGH_FAIL_(%s) ",xx_test_gpio[1][i].name);
                    XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event               
                    return 0;
                } 
            }
       }
       else
       {
           if( halGpioRead(xx_test_gpio[0][i].gpioN) )//halGpioRead(0)
           {
               if(gpio_test_HL == 1 && (i%2) ==1)
               {
                   xxMfgPrintln("AT+MFG_GPIO_TEST_LOW_FAIL_(%s) ",xx_test_gpio[0][i].name);
                   XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event 
                   return 0;
               }
           
               if(gpio_test_HL == 0 && (i%2) ==0)
               {
                   xxMfgPrintln("AT+MFG_GPIO_TEST_LOW_FAIL_(%s) ",xx_test_gpio[0][i].name);
                   XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event 
                   return 0;
               }
           
           }
           else
           {
               if(gpio_test_HL == 0 && (i%2) ==1)
               {
                   xxMfgPrintln("AT+MFG_GPIO_TEST_HIGH_FAIL_(%s) ",xx_test_gpio[0][i].name);
                   XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event               
                   return 0;
               } 
           
               if(gpio_test_HL == 1 && (i%2) ==0)
               {
                   xxMfgPrintln("AT+MFG_GPIO_TEST_HIGH_FAIL_(%s) ",xx_test_gpio[0][i].name);
                   XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, 0, true); //clear event               
                   return 0;
               } 
           }

       }

    }
    
    if(gpio_test_cunt == 3)
    {
        xxMfgPrintln("AT+MFG_GPIO_TEST_OK");
        emberAfMfglibStart(1);
    }

    return 1;
}

void xxMfgDelayEventMfgModelFunction(uint8_t index, uint8_t DelayEvent)
{
    switch (Xx_delay_event[index].type) 
    {  
        case XX_NO_DELAY_EVENT:
        break;

        case XX_MFG_ERASS_FLASH_EVENT:
        {
            uint8_t tstatus;
            Xx_delay_event[index].type = XX_NO_DELAY_EVENT;
			#if 0
            emberAfPluginEepromErase(XX_PROJECT_FLASH_TRIAD_ADDR, ERASE_PAGE_SIZE(1) );
			#endif
            if(tstatus == EEPROM_SUCCESS){

                xxMfgPrintln(" ji xian erass flash SUCCESS");
                XxDelayEventSetEvent(XX_MFG_WRITE_TRIAD_EVENT, XX_MFG_WRITE_TRIAD_EVENT_TIME, true);

            }else{

                xxMfgPrintln("AT+MFG_TRIAD_WRITE_FAIL" );

            }
            
        }
        break;

        case XX_MFG_WRITE_TRIAD_EVENT:
        {
        #if 0
            uint8_t tstatus;
            uint8_t tempBuff[30];
            memset(tempBuff,0xff,sizeof( tempBuff) );
            Xx_delay_event[index].type = XX_NO_DELAY_EVENT;
            xxMfgPrintln("ji xian mfg will write triad flash");
            memcpy(tempBuff+7,&Xx_project_triple.productId[1],8);
            tstatus = emberAfPluginEepromWrite(XX_PROJECT_PRODUCT_ID_ADDR-7,tempBuff,8+7+1);
            if(tstatus == EEPROM_SUCCESS){

                //xxMfgPrintln(" ji xian Write productId to flash SUCCESS");

            }else{

                xxMfgPrintln("AT+MFG_TRIAD_WRITE_FAIL" );
                 return;

            }
            
            memcpy(tempBuff+1,&Xx_project_triple.deviceId[1],19);
            tstatus = emberAfPluginEepromWrite(XX_PROJECT_DEVICE_ID_ADDR-1,tempBuff,19+1);
            if(tstatus == EEPROM_SUCCESS){

                //xxMfgPrintln(" ji xian Write deviceId to flash SUCCESS");

            }else{

                xxMfgPrintln("AT+MFG_TRIAD_WRITE_FAIL" );
                 return;

            }

            memset(tempBuff,0xff,sizeof( tempBuff) );
            memcpy(tempBuff+4,&Xx_project_triple.deviceName[1],12);//12
            tstatus = emberAfPluginEepromWrite(XX_PROJECT_DEVICE_NAME_ADDR-4,tempBuff,12+4);//12
            if(tstatus == EEPROM_SUCCESS){

                //xxMfgPrintln(" ji xian Write deviceName to flash SUCCESS");

            }else{
                
                xxMfgPrintln("AT+MFG_TRIAD_WRITE_FAIL" );
                return;
            }

            xxMfgPrintln("AT+MFG_TRIAD_WRITE_OK" );
			#endif
        }
        break;

        case XX_MFG_READ_TRIAD_EVENT:
        {
            Xx_delay_event[index].type = XX_NO_DELAY_EVENT;
            #if 0
            emberAfPluginEepromRead(XX_PROJECT_PRODUCT_ID_ADDR,\
                                    &Xx_project_triple.productId[1],
                                    XX_PROJECT_PRODUCT_ID_LEN-1);
            
            emberAfPluginEepromRead(XX_PROJECT_DEVICE_ID_ADDR,\
                                    &Xx_project_triple.deviceId[1],
                                    XX_PROJECT_DEVICE_ID_LEN-1);
            
            emberAfPluginEepromRead(XX_PROJECT_DEVICE_NAME_ADDR,\
                                    &Xx_project_triple.deviceName[1],
                                    XX_PROJECT_DEVICE_NAME_LEN-1);
			#endif
            xxMfgPrintln("AT+MFG_TRIAD{PI:%s,DS:%s,DN:%s}",&Xx_project_triple.productId[1],\
                                                           &Xx_project_triple.deviceId[1],\
                                                           &Xx_project_triple.deviceName[1]);
        }
        break;

        case XX_MFG_READ_MAC_EVENT:// read DEVICE_NAME
        {
            Xx_delay_event[index].type = XX_NO_DELAY_EVENT;
			#if 0
            emberAfPluginEepromRead(XX_PROJECT_DEVICE_NAME_ADDR,\
                        &Xx_project_triple.deviceName[1],
                        XX_PROJECT_DEVICE_NAME_LEN-1);
			#endif
            xxMfgPrintln("AT+MFG_ND:%s", &Xx_project_triple.deviceName[1]);
        }
        break;
        
        case XX_MFG_GPIO_TEST_INIT_EVENT:
        {
            xxMfgPrintln(" ji xianXX_MFG_GPIO_TEST_INIT_EVENT " );
            Xx_delay_event[index].type = XX_NO_DELAY_EVENT;
            gpio_test_HL = 0;
            gpio_test_model = 1;
            gpio_test_cunt = 0;
            xxMfgInitFunction(gpio_test_HL,gpio_test_model); //0,1
            XxDelayEventSetEvent(XX_MFG_GPIO_TEST_EVENT, XX_MFG_GPIO_TEST_EVENT_TIME, true);
        }
        break;

        case XX_MFG_GPIO_TEST_EVENT:
        {
            
            if( gpio_test_cunt == 0)
            {                
                if( xxMfgGpioTestFunction() )//0,1
                {
                    gpio_test_HL = 1;
                    gpio_test_model = 1;
                    xxMfgInitFunction(gpio_test_HL,gpio_test_model); //1,1
                    Xx_delay_event[index].time = XX_MFG_GPIO_TEST_EVENT_TIME;
                }
            }
            else if( gpio_test_cunt == 1)
            {
                if( xxMfgGpioTestFunction() )//1,1
                {
                    gpio_test_HL = 0;
                    gpio_test_model = 0;
                    xxMfgInitFunction(gpio_test_HL,gpio_test_model); //0,0
                    Xx_delay_event[index].time = XX_MFG_GPIO_TEST_EVENT_TIME;
                }

            }
            else if( gpio_test_cunt == 2)
            {
                if( xxMfgGpioTestFunction() )//0,0
                {
                    gpio_test_HL = 1;
                    gpio_test_model = 0;
                    xxMfgInitFunction(gpio_test_HL,gpio_test_model); //0,0
                    Xx_delay_event[index].time = XX_MFG_GPIO_TEST_EVENT_TIME;
                }
            }
            else  if( gpio_test_cunt == 3)
            {
                if( xxMfgGpioTestFunction() )//1,0
                {
                    gpio_test_HL = 1;
                    gpio_test_model = 0;
                    xxMfgInitFunction(gpio_test_HL,gpio_test_model); //0,0
                    Xx_delay_event[index].type = XX_NO_DELAY_EVENT;
                }

                gpio_test_cunt = 0;
            }

            gpio_test_cunt++;
            
        }
        break;

        case XX_MFG_READ_version_EVENT:
        {
            EmberAfStatus status;
            EmberAfAttributeType dataType;
            Xx_delay_event[index].type = XX_NO_DELAY_EVENT; 
            MEMSET(Xx_project_zcl_attribute_data, '\0', XX_PROJECT_ZCL_ATTRIBUTE_DATA_LEN);
            dataType = 0x42;
            status = emberAfReadAttribute(1,
                                        ZCL_BASIC_CLUSTER_ID,
                                        ZCL_SW_BUILD_ID_ATTRIBUTE_ID,
                                        CLUSTER_MASK_SERVER,
                                        Xx_project_zcl_attribute_data,
                                        XX_PROJECT_ZCL_ATTRIBUTE_DATA_LEN,
                                        &dataType);
             if (status == EMBER_ZCL_STATUS_SUCCESS) {                
                xxMfgPrintln("AT+MFG_VERSION_IS:%s",Xx_project_zcl_attribute_data);
                //xxMfgPrintln(" DEVICE_TYPE_ID is : 0x%2x ",EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID);
                //xxMfgPrintln(" OTA_VERSION is : 0x%2x ",CUSTOMER_APPLICATION_VERSION);
             }
             else
             {
                xxMfgPrintln(" ###### read error ########### ");
             }
        }
        break;
        
        default:
        {
 //           Xx_delay_event[index].type = XX_NO_DELAY_EVENT;
        }
        break;
    }
}










/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


