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

#ifdef _XX_COMMON_FUNCTION_H
/***************************************************************************************************
*                                               NOTE
*                                               说明
***************************************************************************************************/
//-- source insight使用UTF-8解码，配合SimpliCity Studio开发环境，File->Reload As Encoding...->UTF-8
//-- source insight设置默认解码格式，Option->Preferences->Files->Default encoding->UTF-8

/***************************************************************************************************
*                                           MACRO DEFINED
*                                               宏定义
***************************************************************************************************/
#define xxFuncPrint                                         emberAfAppPrint
#define xxFuncPrintBuffer                                   emberAfAppPrintBuffer
#define XXFuncPrintString                                   "FUNC :"

//#define xxFuncPrint(pstring, ...)
//#define xxFuncPrintBuffer(x,y,z)

/***************************************************************************************************
*                                             INCLUDES
*                                            系统头文件
***************************************************************************************************/

/***************************************************************************************************
*                                          CONSTANT DEFINE
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
*                                             FUNCTIONS
*                                               函数
***************************************************************************************************/

uint8_t XxFuncHex2Asc(uint8_t h, uint8_t *c)
{
    if (h <= 9)
        h += 0x30;
    else if (h >= 10 && h <= 15)
        h += 0x37;
    else
        return 0;

    if (c)
        *c = h;
        
    return 1;  
}

uint8_t XxFuncAsc2Hex(uint8_t c, uint8_t *h)
{
    if ((c >= 0x30) && (c <= 0x39))
        c -= 0x30;
    else if (c >= 0x41 && c <= 0x46)
        c -= 0x37;
    else if (c >= 0x61 && c <= 0x66)
        c -= 0x57;
    else
        return 0;

    if(h)
        *h = c;
        
    return 1;  
}


void XxFuncByte2HexStr(const unsigned char* source, char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
  
    for (i = 0; i < sourceLen; i++)  
    {  
        highByte = source[i] >> 4;  
        lowByte = source[i] & 0x0f ;  
  
        highByte += 0x30;  
  
        if (highByte > 0x39)  
            dest[i * 2] = highByte + 0x07;  
        else  
            dest[i * 2] = highByte;  
  
        lowByte += 0x30;  
        if (lowByte > 0x39)  
            dest[i * 2 + 1] = lowByte + 0x07;  
        else  
            dest[i * 2 + 1] = lowByte;  
    }  
}  
  
//void XxFuncHex2Str(const char *sSrc,  char *sDest, uint8_t nSrcLen, uint8_t format)  
//{  
//    int  i;  
//    char szTmp[3];  
//  
//    for( i = 0; i < nSrcLen; i++ )  
//    {  
//        if(format==1)
//        {
//            sprintf( szTmp, "%x", (unsigned char) sSrc[i] );  
//            memcpy( &sDest[i], szTmp, 1 ); 
//        }
//        else
//        {
//            sprintf( szTmp, "%02x", (unsigned char) sSrc[i] );  
//            memcpy( &sDest[i * 2], szTmp, 2 ); 
//        }
//    }  
//}  
  
uint8_t XxFuncStr2Hex(const char* source, unsigned char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;

    if ((sourceLen%2) != 0)
        return 0;
        
    for (i = 0; i < sourceLen; i += 2)  
    {  
//        highByte = toupper(source[i]);  
//        lowByte  = toupper(source[i + 1]);  
        highByte = source[i];  
        lowByte  = source[i + 1];  

        if (!XxFuncAsc2Hex(highByte, &highByte))
            return 0;
            
        if (!XxFuncAsc2Hex(lowByte, &lowByte))
            return 0;
  
        dest[i / 2] = (highByte << 4) | lowByte;  
    }  
    return 1;  
}  

uint8_t XxFuncStr2Num(const char* source, unsigned long *num, int sourceLen)  
{  
    short i;  
    unsigned char byte;
    unsigned long number = 0;
        
    for (i = 0; i < sourceLen; i++)  
    {  
        byte = source[i];  

        if (!XxFuncAsc2Hex(byte, &byte))
            return 0;

        number *= 10;
        number += byte;  
    }  

    if(num)
        *num = number;
        
    return 1;  
}  

uint8 XxFuncDataPointCheck(uint8 *data, uint16 len)
{
    uint16 i=0;
    uint8  count=0;
    uint16 dataLen;

    if (len == 0)
        return 1;
    
    while (i < len)
    {
        XX_WATCHDOG_RESET;

        if (count++ >= 100)
            return 0;
        
        if (data[i+XX_DATAPOINT_DATA_TYPE_OFFSET] >= XX_DATAPOINT_DATA_TYPE_NULL)
            return 0;

        dataLen = BUILD_UINT16(data[i+XX_DATAPOINT_LEN_LO_OFFSET], data[i+XX_DATAPOINT_LEN_HI_OFFSET]);
            
        i += (dataLen + XX_DATAPOINT_HEAD_LEN);
    }

    if (i==len)
        return 1;
    else
        return 0;
}

uint8 XxFuncDataPointPick(uint8 dpID, uint8 dpIDLen, uint8 *data, uint16 *len, uint8 *dpData, uint16 *dpDataLen, uint8 littleEndianMode, uint8 delFlag, uint8 print)
{
    uint8  i=0,j;
    uint16 dataLen = *len;
    uint8  offset;
    uint16 dpLen;
    
    while (i < dataLen)
    {
        dpLen = BUILD_UINT16(data[i+XX_DATAPOINT_LEN_LO_OFFSET], data[i+XX_DATAPOINT_LEN_HI_OFFSET]);
        
        if (dpLen >= dataLen)
            return 0;

        if (data[i+XX_DATAPOINT_TOKEN_OFFSET] == dpID)
        {
            if (dpIDLen == XX_DATAPOINT_INGNORE_LEN || dpIDLen == dpLen)
            {
                if (print)
                {
                    uint8 k;
                    
                    xxFuncPrint("%p dpID = %x\r\n", XXFuncPrintString, dpID);
                    xxFuncPrint("%p dpDataType = %x\r\n", XXFuncPrintString, data[i+XX_DATAPOINT_DATA_TYPE_OFFSET]);
                    xxFuncPrint("%p dpDataLen = %2x\r\n", XXFuncPrintString, dpLen);
                    xxFuncPrint("%p dpData: ", XXFuncPrintString);
                    for (k = 0; k < dpLen; k++ )
                    {
                        xxFuncPrint("%x ", data[k]);
                    }
                    xxFuncPrint("\r\n");
                }
                
                if (dpData)
                {
                    if (littleEndianMode)
                    {
                        for (j=0;j<dpLen;j++)
                            dpData[j] = data[i+XX_DATAPOINT_DATA_OFFSET+dpIDLen-j-1];
                    }
                    else
                    {
                        for (j=0;j<dpLen;j++)
                            dpData[j] = data[i+XX_DATAPOINT_DATA_OFFSET+j];
                    }
                }

                offset = i + XX_DATAPOINT_DATA_OFFSET;

                if (dpDataLen)
                {
                    (*dpDataLen) = dpLen;
                }

                if (delFlag==1)
                {
                    *len -= (dpLen + XX_DATAPOINT_HEAD_LEN);
                    memcpy (data+i+XX_DATAPOINT_TOKEN_OFFSET, data+i+XX_DATAPOINT_TOKEN_OFFSET+(dpLen+XX_DATAPOINT_HEAD_LEN), dataLen-(i+XX_DATAPOINT_TOKEN_OFFSET)-(dpLen+XX_DATAPOINT_HEAD_LEN));
                }
                else if (delFlag==2)
                {
                    //-- only disable
                    data[i+XX_DATAPOINT_TOKEN_OFFSET] = XX_DATAPOINT_TOKEN_NULL;
                }
                
                return offset;
            }
        }

        i += (dpLen + XX_DATAPOINT_HEAD_LEN);
    }
    
    return 0;
}

void XxFuncDataPointPrint(uint8 id, uint8 dataType, uint8 *data, uint16 len)
{
    uint8 i;

    xxFuncPrint("%p dpID = %x\r\n", XXFuncPrintString, id);
    xxFuncPrint("%p dpDataType = %x\r\n", XXFuncPrintString, dataType);
    xxFuncPrint("%p dpDataLen = %2x\r\n", XXFuncPrintString, len);
    xxFuncPrint("%p dpData: ", XXFuncPrintString);
    for (i = 0; i < len; i++ )
    {
        xxFuncPrint("%x ", data[i]);
    }
    xxFuncPrint("\r\n");
}

uint16 XxFuncDataPointPacket(uint8 dpID, uint8 dpType, uint8 *dpData, uint16 dpLen, uint8 *buf, uint16 *len)
{
    uint8  i=*len;
    uint16 tempLen;
    uint8  j;

    buf[i++] = dpID;      
    buf[i++] = dpType;
    buf[i++] = (uint8)(dpLen>>8);
    buf[i++] = (uint8)(dpLen);
    for (j = 0; j < dpLen; j++)
    {
        buf[i++] = dpData[j];
    }
        
    tempLen = i - (*len);
    *len = i;
    
    return tempLen;
}

uint8 XxFuncBuildZCLAttrPayload(uint8 *buf, uint8 *len,uint16 attrID, uint8 status, uint8 dataType, const uint8 *data, uint8 dataLen)
{
    //-- buf size at least 6 bytes
    uint8 i = *len;
    uint8 tempLen;

    buf[i++] = attrID;                      //-- attribute id
    buf[i++] = attrID>>8;
    if (status != XX_VALUE_NULL_UINT8)
        buf[i++] = status;                  //-- status
    if (dataType != XX_VALUE_NULL_UINT8)
    {
        buf[i++] = dataType;                //-- data type
        switch(dataType)
        {
            case ZCL_CHAR_STRING_ATTRIBUTE_TYPE:
                 ZCL_BOOLEAN_ATTRIBUTE_TYPE:
                buf[i++] = dataLen;             //-- value len
                break;
        }
        memcpy(buf+i, data, dataLen);
        i += dataLen;
    }
        
    tempLen = i-(*len);
    *len = i;
    
    return tempLen;
}

uint32_t XxjitterTimeDelayMs( uint32_t maxRanomNumber, uint32_t basicTime )
{

    uint32_t seed;
    uint32_t jitterDelayMs;

    seed = halCommonGetInt32uMillisecondTick();
    halStackSeedRandom(seed);
    jitterDelayMs = (emberGetPseudoRandomNumber() % maxRanomNumber) + basicTime;
   // emberAfCorePrintln("\r\n xx Random jitterDelayMs to scan network = %d",jitterDelayMs);
    return jitterDelayMs;

}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


