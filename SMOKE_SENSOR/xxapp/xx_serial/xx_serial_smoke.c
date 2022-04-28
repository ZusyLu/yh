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

#ifdef _XX_SERIAL_SMOKE_H
/***************************************************************************************************
*                                               NOTE
*                                               说明
***************************************************************************************************/

/***************************************************************************************************
*                                             INCLUDES
*                                            系统头文件
***************************************************************************************************/

/***************************************************************************************************
*                                               MACRO
*                                               宏定义
***************************************************************************************************/
#define xxSerialSmokePrint                                     emberAfAppPrint
#define xxSerialSmokePrintln                                   emberAfAppPrintln

#define xxSerialSmokePrintBuffer                               emberAfAppPrintBuffer

//#define xxSerialATPrint(pstring, ...)
//#define xxSerialATPrintBuffer(x,y,z)

//smoke cmd 

#define XX_SERIAL_SMOKE_SCAN_NETWORK_RESPONSE_ACK	    	  0X00
#define XX_SERIAL_SMOKE_SCAN_NETWORK_RESPONSE_SUCCESS_DATA    0X01
#define XX_SERIAL_SMOKE_SCAN_NETWORK_RESPONSE_FAIL_DATA    	  0X02
#define XX_SERIAL_SMOKE_SCAN_NETWORK_RESPONSE_LEAVE_DATA      0X03


#define XX_SERIAL_SMOKE_SELF_TEST_CMD				  	      0X0F
#define XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_CMD				  0X89
#define XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_ACK				  0X00
#define XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_NO_PARENT_DATA     0X01
#define XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_IN_NETWORK_DATA    0X02
#define XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_NOT_NETWORK_DATA   0X03

//报警、防拆

#define XX_SERIAL_SMOKE_ALARM_OR_TARMPER_CMD				  0X0C
		  
#define XX_SERIAL_SMOKE_ALARM_OR_TARMPER_UP_DATE			  0X0D							  




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
uint16 xx_serial_rx_timeout = 0;

uint8 xx_serial_smoke_buff[128] = {0};
uint8 xx_serial_smoke_buff_len = 0;
/***************************************************************************************************
*                                             FUNCTIONS
*                                                函数
***************************************************************************************************/

void xxSerialSmokeIasZoneStatusUpDateTriggerFunction( uint8_t trigerBit )
{
    
    uint16_t newStatus=0;
    uint8_t  endpoint;
    uint8_t  timeSinceOccurredSeconds;
    static uint8_t i=0 ,j= 0;
    xxRwAttributeData_t iasStatus;
    EmberNetworkStatus state;
    state = emberAfNetworkState();
	newStatus = trigerBit;
    iasStatus = XxReadOrWriteFunction( XX_PROJECT_READ_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                                            ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                                            ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    //newStatus = iasStatus.data[0] | (iasStatus.data[1]<<8);
    //newStatus |= iasStatus.data[0];
    //newStatus = trigerBit;
    //newStatus = 1;
    endpoint  = 1;
    timeSinceOccurredSeconds = 1;
    if ( halGetBatteryVoltageMilliV() < XX_PROJECT_LOW_POWERMV )
    {
        newStatus |= XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }
    else if ( !halGetBatteryVoltageMilliV() )
    {
        emberEventControlSetDelayMS(emberAfPluginBatteryMonitorReadADCEventControl,200);
    }
    else
    {
       newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }
    
    #ifdef XX_IAS_READ_TAMPER_PIN()
        if ( !XX_IAS_READ_TAMPER_PIN() )
        {
            newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
        else
        {
            newStatus |= XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
    #endif
    
    iasStatus.data[0] = newStatus;
    iasStatus.data[1] = newStatus>>8;
    XxReadOrWriteFunction( XX_PROJECT_WRITE_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );

}

void xxSerialSmokeIasZoneStatusChangeNotificationTriggerFunction( uint8_t trigerBit )
{
    
    uint16_t newStatus=0;
    uint8_t  endpoint;
    uint8_t  timeSinceOccurredSeconds;
    static uint8_t i=0 ,j= 0;
    xxRwAttributeData_t iasStatus;
    EmberNetworkStatus state;
    state = emberAfNetworkState();
	xxSerialSmokePrintln("trigerBit = %x",trigerBit);
	newStatus = trigerBit;
    iasStatus = XxReadOrWriteFunction( XX_PROJECT_READ_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                                            ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                                            ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    //newStatus = iasStatus.data[0] | (iasStatus.data[1]<<8);
    //newStatus |= iasStatus.data[0];

    //newStatus = 1;
    endpoint  = 1;
    timeSinceOccurredSeconds = 1;
    if ( halGetBatteryVoltageMilliV() < XX_PROJECT_LOW_POWERMV )
    {
        newStatus |= XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }
    else if ( !halGetBatteryVoltageMilliV() )
    {
        emberEventControlSetDelayMS(emberAfPluginBatteryMonitorReadADCEventControl,200);
    }
    else
    {
       newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_LOW_POWER_BIT;
    }
    
    #ifdef XX_IAS_READ_TAMPER_PIN()
        if ( !XX_IAS_READ_TAMPER_PIN() )
        {
            newStatus &= ~XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
        else
        {
            newStatus |= XX_IAS_MOTION_SENSOR_ZONE_TAMPER_BIT;
        }
    #endif
    
    iasStatus.data[0] = newStatus;
    iasStatus.data[1] = newStatus>>8;
    XxReadOrWriteFunction( XX_PROJECT_WRITE_MODE, XX_PROJECT_SOURCE_ENPOINT,\
                                ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID,\
                                ZCL_BITMAP16_ATTRIBUTE_TYPE, iasStatus );
    if ( state == EMBER_JOINED_NETWORK )
    {
        #if 0
            emberAfPluginIasZoneServerUpdateZoneStatus(endpoint,
                                                     newStatus,
                                                     timeSinceOccurredSeconds);
        #else 
            XxIasZoneStatusChangeNotificationFunction( endpoint,\
                                                       newStatus, \
                                                       timeSinceOccurredSeconds );
        #endif
    }
    else if ( state == EMBER_JOINED_NETWORK_NO_PARENT )
    {   
        xxSerialSmokePrintln(" xx will rejoin ");
        emberAfStartMoveCallback();
    }

}

void xxSerialSmokeSend7cFrameToSerial( uint8 dataLen, uint8 reservedCmd, uint8 cmd, uint8 *data )
{
	uint8  buf[10];
	uint16 i = 0, j = 0;
	uint8  checksum = 0;
	uint8  responseFlag;
	uint8  responseStatus;

	buf[i++] = XX_SERIAL_SMOKE_FRAME_SOF_1;
    buf[i++] = dataLen;
    buf[i++] = reservedCmd;
    buf[i++] = cmd;
	
    for (j = 0; j < i; j++)
    {
        checksum += buf[j];
    }

    for (j = 0; j < dataLen; j++)
    {
        XX_WATCHDOG_RESET;
        checksum += data[j];
    }

	emberSerialWriteData(XX_SERIAL_SMOKE_PORT_NUM, buf, i);
    if (dataLen)
        emberSerialWriteData(XX_SERIAL_SMOKE_PORT_NUM, data, dataLen);
    emberSerialWriteByte(XX_SERIAL_SMOKE_PORT_NUM, checksum);
    emberSerialWaitSend(XX_SERIAL_SMOKE_PORT_NUM);

}

void xxSerialSmokeCmdHandle( uint8 cmd, uint8 *data, uint8 len )
{
	uint8 value = 0;
	xxSerialSmokePrintln("SMOKE CMD : %X ",cmd);
	switch ( cmd )
	{
		case XX_SERIAL_SMOKE_SCAN_NETWORK_CMD:
			{
				 value = 0;
				//ACK
				xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SCAN_NETWORK_RESPONSE_CMD,&value );
				XxLeaveNetworkFuction();

			}
			break;

		case XX_SERIAL_SMOKE_SELF_TEST_CMD:
			{
				value = 0;
				//xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_CMD,&value );
				EmberNetworkStatus state;
				state = emberAfNetworkState();
				if ( state == EMBER_NO_NETWORK )
			    {
			    	value = 1;
					xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_CMD,&value );
				}
				else if ( state == EMBER_JOINED_NETWORK ) 
			    {
			    	value = 2;
					xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_CMD,&value );
				}
				else if ( state == EMBER_JOINED_NETWORK_NO_PARENT )
			   {
			   		value = 3;
					xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_SELF_TEST_RESPONSE_CMD,&value );
			   }
			}
			break;
		
		case XX_SERIAL_SMOKE_ALARM_OR_TARMPER_CMD:
			{
				xxSerialSmokePrint("data[0] = %x",data[0]);
				value = 0;
				xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_ALARM_OR_TARMPER_CMD,&value );
				xxSerialSmokeIasZoneStatusChangeNotificationTriggerFunction( data[0] );
			}
			break;

		case XX_SERIAL_SMOKE_ALARM_OR_TARMPER_UP_DATE:
			{
				value = 0;
				xxSerialSmokeSend7cFrameToSerial(1, XX_SERIAL_SMOKE_RESERVED_CMD, XX_SERIAL_SMOKE_ALARM_OR_TARMPER_UP_DATE,&value );
				xxSerialSmokeIasZoneStatusUpDateTriggerFunction( data[0] );
			}
			break;
		
		default:
			{}
			break;
    }
}

uint8 XxSerialSmokeCommandPoll(void *com, uint32_t sysTimeMillisecond)
{
	XxSerialReceiveDef *uart = (XxSerialReceiveDef *)com;
	uint16 bufSize = uart->size;
    uint16 head;
    uint16 tempLen;
    uint8  dataLen;
    uint16 packetLen;
    uint16 i;
    uint8  checksum = 0;
    uint16 frameDataLen;
	uint8 reservedCmd = 0;
	uint8 smokeCmd = 0;

    uint8  errPrintFlag = 0;
	while ( uart->len >= XX_SERIAL_SMOKE_MIN_LED  )
    {
		XX_WATCHDOG_RESET;
		head = uart->head;
        tempLen = uart->len;
		if (uart->packetLen == 0)
	    {
			if ( uart->buf[ head ] != XX_SERIAL_SMOKE_FRAME_SOF_1 )
		    {
				if ( errPrintFlag == 0 ) 
                {
                    errPrintFlag = 1;
                    xxSerialSmokePrint("\r\nSMOKE : --> ERROR : date error\r\n");
                }

				if ( uart->len > XX_SERIAL_SMOKE_MIN_LED )
                {
                    xxSerialSmokePrint( "%X ", uart->buf[ head%bufSize ] );
                }
                else //-- uart->len 
                {
                    xxSerialSmokePrint( "SMOKE : --> short ERROR : " );
                    for ( uint8 k = 0; k < XX_SERIAL_SMOKE_MIN_LED; k++ )
                    {
                        xxSerialSmokePrint( "%X ", uart->buf[ ( head+k )%bufSize ] );
                    }
                    xxSerialSmokePrint("\r\n");
                }

				uart->head++;
                uart->head %= bufSize;
                uart->len--;
                uart->packetLen = 0;
                
                continue;
				
			}
			xxSerialSmokePrint("\r\n");
			
			dataLen = uart->buf[(head+XX_SERIAL_SMOKE_FRAME_LEN_SOF)%bufSize];
			uart->packetLen = XX_SERIAL_SMOKE_MIN_LED+dataLen;            
            if (tempLen < uart->packetLen)
            {
                //-- jxSerialWifiPrint("WIFI : Wait more bytes 1, cur len:%u, wait len:%u\r\n", uart->len, uart->packetLen);
                break;
            }
		}
		else if (tempLen < uart->packetLen)
        {
            if (uart->lenBackup == tempLen)
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
            
            uart->lenBackup = tempLen;
            break;
        }

		xx_serial_rx_timeout = 0;
		        //-- verify checksum
		xxSerialSmokePrintln("uart->packetLen is %x",uart->packetLen);
	    //xxSerialSmokePrintln("xx checksum data is :");
        for (i = 0; i < (uart->packetLen - 1); i++)
        {
            XX_WATCHDOG_RESET;
			//xxSerialSmokePrint("%x ",uart->buf[(head+i)%bufSize]);
            checksum += uart->buf[(head+i)%bufSize];
        }
		xxSerialSmokePrintln("");
		xxSerialSmokePrintln("cunt checksum is %x",checksum);
		xxSerialSmokePrintln("uart->buf[checksum] is %x",uart->buf[(head + uart->packetLen - 1) % bufSize]);
		if (checksum != uart->buf[(head + uart->packetLen - 1) % bufSize])
	    {
            xxSerialSmokePrint("SMOKE : --> ERROR : checksum error");
            xxSerialSmokePrint("SMOKE : --> ");
            for (i = 0; i < (uart->packetLen - 1); i++)
            {
                xxSerialSmokePrint("%X ", uart->buf[(head+i)%bufSize]);
            }
            xxSerialSmokePrint("\r\n");

            uart->head++;
            uart->head %= bufSize;
            uart->len--;
            uart->packetLen = 0;
            
            continue;

		}
		
		xxSerialSmokePrint("SMOKE : --> DATA : ");
        for(uint16 i = 0; i < uart->packetLen; i++)
        {
            XX_WATCHDOG_RESET;
            xxSerialSmokePrint("%x ", uart->buf[(head + i) % bufSize]);
        }
        xxSerialSmokePrint("\r\n");

		frameDataLen = uart->buf[head + XX_SERIAL_SMOKE_FRAME_LEN_SOF];
		reservedCmd = uart->buf[head + XX_SERIAL_SMOKE_FRAME_RESERVED_CMD_OFFSET];
		smokeCmd = uart->buf[head + XX_SERIAL_SMOKE_FRAME_CMD_OFFSET];
		xx_serial_smoke_buff_len = uart->buf[head + XX_SERIAL_SMOKE_FRAME_LEN_SOF];
		memcpy(xx_serial_smoke_buff, &uart->buf[head + XX_SERIAL_SMOKE_FRAME_DATA_OFFSET], xx_serial_smoke_buff_len);
		xxSerialSmokePrintln("SMOKE CMD IS : %X ",smokeCmd);
		xxSerialSmokePrint("xx_serial_smoke_buff[0] = %x data = %x ",xx_serial_smoke_buff[0], uart->buf[head+XX_SERIAL_SMOKE_FRAME_DATA_OFFSET]);
		xxSerialSmokeCmdHandle(smokeCmd, xx_serial_smoke_buff, xx_serial_smoke_buff_len);
		//-- skip this packet
        packetLen = uart->packetLen;
        uart->head += packetLen;
        uart->head %= bufSize;
        uart->len -=  packetLen;
        uart->packetLen = 0;
		
		break;
	}
}

void xxSerialSmokeRxGpioWakeInit( void )
{
	GPIOINT_Init();
	GPIO_ExtIntConfig(BSP_USART1_RX_PORT, BSP_USART1_RX_PIN, BSP_USART1_RX_PIN, true, true, true);
}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


