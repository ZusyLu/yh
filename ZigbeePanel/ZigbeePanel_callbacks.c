// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include "hal/hal.h"
#include EMBER_AF_API_NETWORK_STEERING
#include "app/framework/include/af.h"
#include "app/util/ezsp/ezsp-enum.h"

#include "network-manage-router.h"
//#include "app-device-ui.h"
#include "serial-rx-task.h"
#include "module-cfg-tokens.h"

#define  BUTTON_LONG_PRESS_TIMER_TICK                         120 //120 * 25ms = 87   help
#define  YH_INTERPAN_DATA_LEN                                 150
#define  YH_ZCL_PENETRATE                                     0XFC0F

EmberEventControl appPowerOnEventControl;
EmberEventControl deviceMonitorEventControl;
EmberEventControl redLedSteadyOnTimeEventControl;
EmberEventControl yhSetNetDataEvenControl;
extern EmberEventControl appRebootEventControl;

extern uint8_t checkSum(uint8_t *data,uint8_t length);
void yhSetNetDataEventHandler(void);
void yhEmAfCliSendCommand(uint16_t dest);
static bool useNextSequence = true;
bool powerOnSuccess = false;
uint16_t deviceTokenAddr;

extern uint8_t testMessage[];
extern tokTypeModuleCfgStruct tokenModuleCfgInfo;
static uint8_t yhPenetrateData[80]={0};
tokTypeStackNodeData tokenNodeData;
tokTypeStackKeys tokenKeys;

yhPenetrateDataType yhPenetrateDataId;

static const  unsigned  char aucCRCHi[] = {

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40

};

static const unsigned char aucCRCLo[] = {

    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,

    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,

    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,

    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,

    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,

    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,

    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,

    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,

    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,

    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,

    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,

    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,

    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,

    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,

    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,

    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,

    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,

    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,

    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,

    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,

    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,

    0x41, 0x81, 0x80, 0x40

};


uint16_t getCRC(uint8_t* buf,int len)
{
  unsigned char ucCRCHi = 0xFF;
  unsigned char ucCRCLo = 0xFF;
  int iIndex;
  int i = 0;
  for(i = 0;i < len;i++)
  {

          iIndex = ucCRCLo ^ buf[i];

          ucCRCLo = ( unsigned char )( ucCRCHi ^ aucCRCHi[iIndex] );

          ucCRCHi = aucCRCLo[iIndex];

   }
  return ((uint16_t)ucCRCHi << 8 | (uint16_t)ucCRCLo);

}
typedef union{
  uint8_t cm[2];
  uint16_t cmd;
}yhcmdType;

typedef union{
  uint8_t lens[2];
  uint16_t len;
}yhLenType;

typedef union{
  uint8_t pans[2];
  uint16_t panid;
}yhPanType;

typedef union{
  uint8_t crc8[2];
  uint16_t crc16;
}yhcrcType;

typedef union{
  uint8_t Nodeids[2];
  EmberNodeId Nodeid;
}yhNodeidType;

typedef struct{
  yhLenType len;
  uint8_t localMac[8];
  yhNodeidType yhNodeid;
  EmberNodeType nodeType;
  EmberNetworkParameters NetParameters;
  uint8_t networkKey[8];
  yhcrcType crc;
}yhDeviceInfoType;

typedef struct{
   uint8_t  rfSumLength;  //
   uint8_t  header[2];
   uint8_t  version;//;
   yhcmdType  cmdid;
   uint8_t  functionCode;
   yhLenType length;
   uint8_t  tarLongId[8];
   yhPanType tarPanId;
   uint8_t  data[80];
   yhcrcType crc;//
}yhDebugProtocolDataStruct;

typedef union{
  uint8_t lens[2];
  uint16_t len;
}yhSetDataLenType;



typedef struct{
    //
   uint8_t  SetLengthL;
   uint8_t  SetLengthH;
   uint8_t  setLongId[8];
   uint8_t  deviceType;//;
   uint8_t  channel;
   uint8_t  power;
   uint8_t  shortidL;
   uint8_t  shortidH;
   uint8_t  setPanidL;
   uint8_t  setPanidH;
   uint8_t setExpanid[8];
   uint8_t  setKey[16];
}yhSetDeviceDataStruct;

// Event function forward declarations
void appPowerOnHandler(void);
void yhEmAfInterpanFragmentTestCommand(EmberPanId htPanId,uint8_t *htMacId , \
                                       uint16_t HTclusterId,uint8_t *data,uint16_t HTmessageLen);

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
boolean emberAfStackStatusCallback(EmberStatus status)
{
  return false;
}


/** @brief Complete
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  emberAfCorePrintln("Network Steering Completed: %p (0x%X)",
                     (status == EMBER_SUCCESS ? "Join Success" : "FAILED"),
                     status);
  emberAfCorePrintln("Finishing state: 0x%X", finalState);
  emberAfCorePrintln("Beacons heard: %d\nJoin Attempts: %d", totalBeacons, joinAttempts);
}

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
void emberAfMainInitCallback(void)
{
  //Start power on event for LED indication
  emberEventControlSetDelayMS(appPowerOnEventControl,100);
  clearSerialBuffer();
  modeInitialFromToken();
}

/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 *
 */
void emberAfMainTickCallback(void)
{
  serialRxProcess(HAL_SERIAL_PORT_USART2);
}

/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 *
 * @param cmd   Ver.: always
 */
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
  if(emberGetNodeId()  == cmd->source)return true;
  if(((cmd->commandId) == 0x00)&&(cmd->clusterSpecific)
      &&(cmd->apsFrame->clusterId == 0xfc00)
      &&(!cmd->mfgSpecific))
  {
      // cmd->buffer
      //byte0  frame control
      //byte1  sequence number
      //byte2  command id
      //byte3  length
      //.......payload
      #ifdef LED_BLINK_FOR_TEST
      customDeviceLedActionShow(ACTION_PRESS_BUTTON,1);
      #endif
      emberSerialWriteData(HAL_SERIAL_PORT_USART2, &(cmd->buffer[4]), cmd->bufLen-4);
      emberSerialWaitSend(HAL_SERIAL_PORT_USART2);
      return true;
  }
  else if( cmd->apsFrame->clusterId == YH_ZCL_PENETRATE )
  {
      uint8_t i=0;
      emberAfCorePrintln("tou chuan = ");
      for( i = 0; i < cmd->bufLen-3; i++)
      emberAfCorePrint("%x ",cmd->buffer[3+i]);

      //emberSerialWriteData(HAL_SERIAL_PORT_USART2, &cmd->buffer[3], cmd->bufLen-3);
      //emberSerialWaitSend(HAL_SERIAL_PORT_USART2);
      yhSerialSendProtocolFuction(FRAME_HEADER, cmd->bufLen-3, FRAME_CMD0, YH_PENETRATE_CMD1, &cmd->buffer[3]);
      return true;
  }
  else
      return false;

}

/** @brief Pre Message Received
 *
 * This function is called by the Interpan plugin when an interpan message is
 * received but has not yet been handled by the plugin or the framework. The
 * application should return true if the message was handled.
 *
 * @param header The inter-PAN header Ver.: always
 * @param msgLen The message payload length Ver.: always
 * @param message The message payload Ver.: always
 */
bool emberAfPluginInterpanPreMessageReceivedCallback(const EmberAfInterpanHeader *header,
                                                     uint8_t msgLen,
                                                     uint8_t *message)
{
#if 0
  emberAfCorePrintln("panid=%2x ",header->panId);
  emberAfCorePrintln("short id=%2x ",header->shortAddress);
  emberAfCorePrintln("mac = ");
  for(uint8_t i=0;i<8;i++)
  {
      emberAfCorePrint(" %x ",header->longAddress[i]);
  }
  emberAfCorePrintln(" ");

  emberAfCorePrintln("interpan data = ");
  for(uint8_t i=0;i<msgLen;i++)
  {
      emberAfCorePrint(" %x ",message[i]);
  }
  emberAfCorePrintln(" ");

  yhEmAfInterpanFragmentTestCommand(0x123,(uint8_t*) header->longAddress[0] , \
                                         0xfc88,yhTestData,4);

  return false;
#endif
  uint8_t temp[50];
  yhDebugProtocolDataStruct yhScanProtocolData;
  yhDeviceInfoType yhDeviceInfo;
  yhSetDeviceDataStruct yhSetDeviceData;
  yhNodeidType yhsetshortid;
  yhPanType yhsetPanId;
  yhSetDataLenType  yhSetLength;
  uint8_t i;
  uint8_t panidH;

  emberAfCorePrintln("interpan data = ");
  for( i=0;i<msgLen;i++)
  {
      emberAfCorePrint(" %x ",message[i]);
  }
  emberAfCorePrintln(" ");
  memcpy(yhPenetrateDataId.longid,header->longAddress,8);
  if(header->clusterId == 0xfc99)
  {
      yhPenetrateDataId.panid = 0X6868;
      memcpy(yhPenetrateDataId.longid,header->longAddress,8);
      yhPenetrateData[0] = 0x7C;
      yhPenetrateData[1] = msgLen;
      yhPenetrateData[2] = 0x01;
      yhPenetrateData[3] = 0x86;
      memcpy(&yhPenetrateData[4], message,msgLen);
      yhPenetrateData[4+msgLen] = checkSum(yhPenetrateData,(msgLen+4) );
      emberSerialWriteData(HAL_SERIAL_PORT_USART2, yhPenetrateData, (msgLen+4+1) );
      emberSerialWaitSend(HAL_SERIAL_PORT_USART2);


      emberAfCorePrintln("yhEmAfInterpanFragmentTestCommand ");
      return false;
  }

  yhScanProtocolData.header[0]=0xf0;
  yhScanProtocolData.header[1]=0x0f;
  yhScanProtocolData.version=0x01;
  yhScanProtocolData.cmdid.cmd = 0;
  yhScanProtocolData.functionCode = 0x03;
  yhScanProtocolData.length.len = 0x33;

  emberAfCorePrintln("panid=%2x ",header->panId);
  emberAfCorePrintln("short id=%2x ",header->shortAddress);
  emberAfCorePrintln("mac = ");
  for( i=0;i<8;i++)
  {
      emberAfCorePrint(" %x ",header->longAddress[i]);
  }
  emberAfCorePrintln(" ");

  if(message[0]==0 && message[1]==0)
  {
      EmberKeyStruct nwkKey;
      emberAfCorePrintln("this is scan device ");
      uint8_t yhData[100];
      emberAfGetEui64(yhDeviceInfo.localMac);
      emberAfCorePrint("yhDeviceInfo.localMac = ");
      for( i=0;i<8;i++)
      {
          emberAfCorePrint(" %x ",yhDeviceInfo.localMac[i]);
      }
      emberAfCorePrintln(" ");

      yhDeviceInfo.yhNodeid.Nodeid=emberGetNodeId();
      emberAfCorePrintln(" yhDeviceInfo.Nodeid = %2x ", yhDeviceInfo.yhNodeid.Nodeid);

      emberAfGetNetworkParameters(&yhDeviceInfo.nodeType,&yhDeviceInfo.NetParameters);
      emberAfCorePrintln(" yhDeviceInfo.nodeType = %x ", yhDeviceInfo.nodeType);
      emberAfCorePrintln(" yhDeviceInfo.NetParameters。radioChannel = %x ", yhDeviceInfo.NetParameters.radioChannel);
      emberAfCorePrintln(" yhDeviceInfo.NetParameters。radioTxPower = %x ", yhDeviceInfo.NetParameters.radioTxPower);
      emberAfCorePrintln(" yhDeviceInfo.NetParameters。panId = %2x ", yhDeviceInfo.NetParameters.panId);

      emberAfCorePrint("yh extendedPanId = ");
      for( i=0;i<8;i++)
      {
          emberAfCorePrint(" %x ",yhDeviceInfo.NetParameters.extendedPanId[i]);
      }
      emberAfCorePrintln(" ");

      if ( EMBER_SUCCESS != emberGetKey(EMBER_CURRENT_NETWORK_KEY,
                                        &nwkKey)) {
        MEMSET((uint8_t*)&nwkKey, 0xFF, sizeof(EmberKeyStruct));
      }
      panidH = yhDeviceInfo.NetParameters.panId>>8;
      yhDeviceInfo.len.len =  8+1+1+1+2+1+1+8+16;//0x27
      memcpy(yhData,yhScanProtocolData.header,2);
      memcpy(yhData+2,&yhScanProtocolData.version,1);
      memcpy(yhData+2+1,yhScanProtocolData.cmdid.cm,2);
      memcpy(yhData+2+1+2,&yhScanProtocolData.functionCode,1);
      memcpy(yhData+2+1+2+1,&yhScanProtocolData.length.lens[1],1);
      memcpy(yhData+2+1+2+1+1,&yhScanProtocolData.length.lens[0],1);
      memset(yhData+2+1+2+1+2,0x00,10);
      memcpy(&yhData[18],&yhDeviceInfo.len.lens[1],1);
      memcpy(&yhData[18]+1,&yhDeviceInfo.len.lens[0],1);

      memcpy(&yhData[18]+2,yhDeviceInfo.localMac,8);
      memcpy(&yhData[18]+2+8,&yhDeviceInfo.nodeType,1);
      memcpy(&yhData[18]+2+8+1,&yhDeviceInfo.NetParameters.radioChannel,1);
      memcpy(&yhData[18]+2+8+1+1,&yhDeviceInfo.NetParameters.radioTxPower,1);

      memcpy(&yhData[18]+2+8+1+1+1,&yhDeviceInfo.yhNodeid.Nodeids[1],1);
      memcpy(&yhData[18]+2+8+1+1+1+1,&yhDeviceInfo.yhNodeid.Nodeids[0],1);

      memcpy(&yhData[18]+2+8+1+1+1+2,&panidH,1);
      memcpy(&yhData[18]+2+8+1+1+1+2+1,&yhDeviceInfo.NetParameters.panId,1);

      memcpy(&yhData[18]+2+8+1+1+1+2+1+1,yhDeviceInfo.NetParameters.extendedPanId,8);
      //memcpy(yhData+2+8+1+1+1+2+1+1+8,yhDeviceInfo.NetParameters.extendedPanId,8);
      memcpy(&yhData[18]+2+8+1+1+1+2+1+1+8,nwkKey.key.contents,16);

      emberAfCorePrint("yh yhData = ");
      for( i=0;i<(2+8+1+1+1+2+1+1+8+16 + 18+2);i++)
      {
          emberAfCorePrint(" %x ",yhDeviceInfo.NetParameters.extendedPanId[i]);
      }
      emberAfCorePrintln(" ");

      yhDeviceInfo.crc.crc16 = getCRC(yhData,2+8+1+1+1+2+1+1+8+16 + 18);
      emberAfCorePrint("yh yhDeviceInfo.crc.crc16 = %2x ",yhDeviceInfo.crc.crc16);
      memcpy(&yhData[18]+2+8+1+1+1+2+1+1+8+16,&yhDeviceInfo.crc.crc8[1],1);
      memcpy(&yhData[18]+2+8+1+1+1+2+1+1+8+16+1,&yhDeviceInfo.crc.crc8[0],1);
      emberAfCorePrintln("scan device ");

      yhEmAfInterpanFragmentTestCommand(0x6868,(uint8_t*) header->longAddress , \
                                             0xfc88,yhData,(2+8+1+1+1+2+1+1+8+16 + 18 +2) );
  }else if(message[0]==0 && message[1]==0x27)
  {
      halCommonGetToken(&tokenNodeData, TOKEN_STACK_NODE_DATA);
      halCommonGetToken(&tokenKeys, TOKEN_STACK_KEYS);
     // MEMMOVE(&yhSetDeviceData.Length.lens,message,msgLen);
      MEMMOVE(&yhSetDeviceData.SetLengthL,message,msgLen);
      yhsetshortid.Nodeids[1] = yhSetDeviceData.shortidL;
      yhsetshortid.Nodeids[0] = yhSetDeviceData.shortidH;
      yhsetPanId.pans[1] = yhSetDeviceData.setPanidL;
      yhsetPanId.pans[0] = yhSetDeviceData.setPanidH;

      emberAfCorePrint("yh set longId = ");
      for( i=0;i<8;i++)
      {
          emberAfCorePrint(" %x ",yhSetDeviceData.setLongId[i]);
      }
      emberAfCorePrintln("\r\n yh set device type = %x",yhSetDeviceData.deviceType);
      emberAfCorePrintln(" yh set device chanel = %x",yhSetDeviceData.channel);
      emberAfCorePrintln(" yh set device power = %x",yhSetDeviceData.power);
      emberAfCorePrintln(" yh set device short id =%2X ",yhsetshortid.Nodeid);
      emberAfCorePrintln(" yh set device pan id =%2X ",yhsetPanId.panid);

      emberAfCorePrint("yh set expanid = ");
      for( i=0;i<8;i++)
      {
          emberAfCorePrint(" %x ",yhSetDeviceData.setExpanid[i]);
      }

      emberAfCorePrint("\r\n yh set set key = ");
      for( i=0;i<16;i++)
      {
          emberAfCorePrint(" %x ",yhSetDeviceData.setKey[i]);
      }

      tokenModuleCfgInfo.channel = yhSetDeviceData.channel;
      tokenModuleCfgInfo.panId = yhsetPanId.panid;
      tokenModuleCfgInfo.shortAddress = yhsetshortid.Nodeid;
      tokenModuleCfgInfo.radioTxPower = yhSetDeviceData.power;
      MEMMOVE(tokenModuleCfgInfo.extendPanId,yhSetDeviceData.setExpanid,8);

      MEMMOVE(&tokenModuleCfgInfo.nwkKey[0],yhSetDeviceData.setKey,16);
      tokenModuleCfgInfo.deviceType = 0x02;
#if 1
      MEMMOVE(temp, emberGetEui64(), EUI64_SIZE);
      temp[8] = YH_SET_NETWORK_OK;
      yhDebugDataAndSendFunction( LEWIN_PRO_VERSION,\
                                  LEWIN_FRAME_DEFAULT_CMD, \
                                  YH_SET_NETWORK_FUNCTION_CODE, \
                                  19, \
                                  (uint8_t*) header->longAddress,\
                                  YH_DEBUG_DEFAULT_PANID,\
                                  temp  );
#endif
     // halCommonSetToken(TOKEN_MODULE_CFG_INFO,&tokenModuleCfgInfo);
      emberEventControlSetDelayMS(yhSetNetDataEvenControl,1000);
      emberAfCorePrint("\r\n yh set set data finish = ");
  }

  return false;
}



// Event function stubs
void appPowerOnHandler(void)
{
   emberEventControlSetInactive(appPowerOnEventControl);
   return ;                    //cut
   //modeInitialFromToken();
   powerOnSuccess = true;
   deviceTokenAddr = getDeviceTokenAddr();
   emberEventControlSetDelayMS(deviceMonitorEventControl,5000);
   //emberSerialPrintf(1,"power %x\r\n",emberGetRadioPower());
}

void appNetworkJoinHandler(void){
  emberEventControlSetInactive(appNetworkJoinEventControl);
    return;

}
void deviceMonitorHandler(void){ // redLedSteadyOnTimeEventControl
  emberEventControlSetInactive(deviceMonitorEventControl);
  return;
}

void redLedSteadyOnTimeHandler(void){
  emberEventControlSetInactive(redLedSteadyOnTimeEventControl);
  return;
}

void yhSetNetDataEventHandler(void)
{
  uint8_t yhExpanid[8]={0};
  emberEventControlSetInactive(yhSetNetDataEvenControl);
 // halCommonGetToken(&tokenModuleCfgInfo,TOKEN_MODULE_CFG_INFO) ;

  halCommonSetToken(TOKEN_MODULE_CFG_INFO,&tokenModuleCfgInfo);
  emberEventControlSetDelayMS(appRebootEventControl,500);

  //halCommonSetToken(TOKEN_STACK_NODE_DATA,&tokenNodeData);
  emberAfCorePrintln("\r\n yh set yhSetNetDataEventHandler ");
}


// interpan

void yhEmAfInterpanFragmentTestCommand(EmberPanId htPanId,uint8_t *htMacId , \
                                       uint16_t HTclusterId,uint8_t *data,uint16_t HTmessageLen)
{
  EmberPanId panId = (EmberPanId)htPanId;
  EmberEUI64 eui64;
  uint16_t i;

  memcpy(eui64,htMacId,EUI64_SIZE);

 // emberCopyBigEndianEui64Argument(1, eui64);

  uint16_t clusterId = (uint16_t)HTclusterId;
  uint16_t messageLen = (uint16_t)HTmessageLen;

  messageLen = (messageLen > YH_INTERPAN_DATA_LEN)
               ? YH_INTERPAN_DATA_LEN : messageLen;

  for (i = 0; i < messageLen; i++) {
    testMessage[i] = (data[i] & 0xFF);
  }

  emberAfCorePrint("Sending inter-PAN message of len %d to ", messageLen);
 // emberAfCoreDebugExec(emberAfPrintBigEndianEui64(eui64));
 // emberAfCorePrintBuffer(eui64, EUI64_SIZE, true);


  emberAfCorePrint("dest mac ID : ");
  for (i = 0; i < 8; i++) {
    emberAfCorePrint("%x ",htMacId[i]);
  }
  emberAfCorePrintln("");
  emberAfCorePrint("dest copy eui64 ID : ");
  for (i = 0; i < 8; i++) {
    emberAfCorePrint("%x ",eui64[i]);
  }
  emberAfCorePrint(" with random values: ");
  EmberStatus status = emberAfSendInterPan(panId,
                                           eui64,
                                           EMBER_NULL_NODE_ID,
                                           0,             // mcast id - unused
                                           clusterId,
                                           SE_PROFILE_ID, // GBCS only
                                           messageLen,
                                           testMessage);

  emberAfCorePrintln("%s (0x%X)",
                     (EMBER_SUCCESS == status) ? "success" : "failure",
                     status);
  emberAfCoreFlush();

}

void yhEmAfCliRawCommand(uint8_t *srcbuff,uint8_t Length,uint16_t htClusterID,uint16_t  htDest)
{
  uint8_t seqNumIndex;
  //uint8_t length;
  emAfApsFrameClusterIdSetup((uint16_t)htClusterID);
 // emberCopyStringArgument(1, appZclBuffer, APP_ZCL_BUFFER_SIZE_CLI, false);

  MEMSET(appZclBuffer, 0xff, Length+3);

  MEMCOPY(appZclBuffer + 3, srcbuff, Length);

  appZclBuffer[0] = 0x18;

  appZclBuffer[2] = 0x0a;

  seqNumIndex = (appZclBuffer[0] & ZCL_MANUFACTURER_SPECIFIC_MASK) ? 3 : 1;

  if (useNextSequence) {

    appZclBuffer[seqNumIndex] = emberAfNextSequence();

  }
  //emberStringCommandArgument(1, &length);
  appZclBufferLen = Length+3;

  cliBufferPrint();

  yhEmAfCliSendCommand(htDest);

}

void yhEmAfCliSendCommand(uint16_t dest)
{
  uint16_t destination = (uint16_t)dest;
  uint8_t srcEndpoint = (uint8_t)1;
  uint8_t dstEndpoint = (uint8_t)1;
  uint8_t *commandName = (uint8_t *)emberCurrentCommand->name;
  EmberStatus status;
  uint8_t label;

  // check that cmd is built
#if 0
  if (zclCmdIsBuilt == false) {
    emberAfCorePrintln("no cmd");
    return;
  }
#endif

  emAfApsFrameEndpointSetup(srcEndpoint, dstEndpoint);

  if (emberAfPreCliSendCallback(&globalApsFrame,
                                emberAfGetNodeId(),
                                destination,
                                appZclBuffer,
                                appZclBufferLen)) {
    return;
  }
#if 0

  if (commandName[4] == '_') {
    label = 'M';
    status = emberAfSendMulticast(destination,
                                  &globalApsFrame,
                                  appZclBufferLen,
                                  appZclBuffer);
  } else if (destination >= EMBER_BROADCAST_ADDRESS) {
    label = 'B';
    status = emberAfSendBroadcast(destination,
                                  &globalApsFrame,
                                  appZclBufferLen,
                                  appZclBuffer);
  } else {
    label = 'U';
    status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                destination,
                                &globalApsFrame,
                                appZclBufferLen,
                                appZclBuffer);
  }
#endif

  if(destination == 0xffff){
      status = emberAfSendBroadcast(destination,
                                    &globalApsFrame,
                                    appZclBufferLen,
                                    appZclBuffer);
  }else{
    status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                    destination,
                                    &globalApsFrame,
                                    appZclBufferLen,
                                    appZclBuffer);
  }


  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Error: CLI Send failed, status: 0x%X", status);
  }

  UNUSED_VAR(label);

  emberAfDebugPrintln("T%4x:TX (%p) %ccast 0x%x%p",
                      emberAfGetCurrentTime(),
                      "CLI",
                      label,
                      status,
                      ((globalApsFrame.options & EMBER_APS_OPTION_ENCRYPTION)
                       ? " w/ link key" : ""));

  emberAfDebugPrint("TX buffer: [");

  emberAfDebugFlush();

  emberAfDebugPrintBuffer(appZclBuffer, appZclBufferLen, true);

  emberAfDebugPrintln("]");

  emberAfDebugFlush();

  zclCmdIsBuilt = false;

  mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;

  disableDefaultResponse = 0;

  useNextSequence = true;

}


