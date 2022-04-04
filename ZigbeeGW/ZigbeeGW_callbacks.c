// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include "hal/hal.h"
#include EMBER_AF_API_NETWORK_STEERING
#include "app/framework/include/af.h"
#include "app/util/ezsp/ezsp-enum.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
//yh
#include "network-manage-router.h"
#include "app-device-ui.h"
#include "serial-rx-task.h"
#include "serial-rx-queue.h"
#include "rf-rx-uart-tx-task.h"
#include "rf-tx-command.h"
#include "serial-ota-task.h"
#include "protocol-parsing.h"

#define APP_ZCL_BUFFER_SIZE EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH
//YH
#define   YH_TEST_RESET_MAX_CUNT       200                // help       yh test emberAfTrustCenterJoinCallback EMBER_STANDARD_SECURITY_UNSECURED_JOIN

#define   YH_DEVICE_ANNOUNCE           0x0013u

//zcl
#define   YH_BASIC                        0X0000
#define   YH_POWER_CONFIGURATION          0X0001
#define   YH_IAS_ZONE                     0X0500
#define   YH_WINDOW_COVERING              0X0102
#define   YH_DOOR_LOCK                    0X000f
//zcl Attributes
#define  INFRARED_TRANSPONDER_ATTRIBUTES  0X410C
#define  DOOR_LOCK_ATTRIBUTES             0X4200

#define OTA_DELAY_TIME_MINUTE 15
#define  BUTTON_LONG_PRESS_TIMER_TICK     120 //120 * 25ms = 3000ms
// Custom event stubs. Custom events will be run along with all other events in
// the application framework. They should be managed using the Ember Event API
// documented in stack/include/events.h
//#define  RF_SCHEDULE_TEST
#define  RF_SCHEDULE_END_INDEX   48
// Event control struct declarations
EmberEventControl appPowerOnEventControl;
//EmberEventControl appRebootEventControl;
EmberEventControl scheduleRfTestEventControl;
EmberEventControl otaScheduleDelayEventControl;  //  customDefineProtocalProcess

//eeprom
tokType_test_reset  testResetCunt;

bool powerOnSuccess = false;
bool deviceQueryImage = false;

//yh app
typedef struct
{
  uint16_t deviceId;
  EmberEUI64 ieee;
}yhBindType;

yhBindType yhBind;
//={0};
void yhZdoLeaveRequestCommand(EmberNodeId target);

void appPowerOnHandler(void);
void appRebootEventHandler(void);
void scheduleRfTestHandler(void);
void otaScheduleDelayHandler(void);
void yhZdoBindCommand(EmberNodeId target, uint8_t sourceEndpoint, uint8_t destinationEndpoint, uint16_t clusterId,EmberEUI64 sourceEui);
void yhZclGlobalSendMeAReportCommand(void);

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
  if (status == EMBER_NETWORK_UP){
      emberAfCorePrintln("xx EMBER_NETWORK_UP ");
      //emberEventControlSetDelayMS(appPowerOnEventControl,1000);
  }else if(status == EMBER_NETWORK_DOWN){
      emberAfCorePrintln("xx EMBER_NETWORK_DOWN ");
  }

  return false;
#if PRODUCT_TEST_MODE_ENABLE
    if(mfgProductTestFlagGet())
    {
        if (status == EMBER_NETWORK_UP)
        {
           customDeviceLedActionShow(ACTION_NONE,0);
        }
    }
    #endif
    if(EMBER_NO_NETWORK == emberAfNetworkState())
    {
         resetToFactoryState();

    }
    else if(EMBER_JOINED_NETWORK == emberAfNetworkState())
    {
        if(true == joiningNetworkRuuning)
        {
            emberAfCorePrintln(" EMBER_JOINED_NETWORK == emberAfNetworkState() ");
            //led steady on 10s
     //       customDeviceLedActionShow(ACTION_NONE,0);
      //      customDeviceLedActionShow(ACTION_STEADY_ON_XS,10);
            joiningNetworkRuuning = false;
        }
    }
    return true;
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
  emberAfCorePrintln("yh Network Steering Completed: %p (0x%X)",
                     (status == EMBER_SUCCESS ? "Join Success" : "FAILED"),
                     status);
  emberAfCorePrintln("Finishing state: 0x%X", finalState);
  emberAfCorePrintln("Beacons heard: %d\nJoin Attempts: %d", totalBeacons, joinAttempts);
}

/** @brief Complete
 *
 * This callback notifies the user that the network creation process has
 * completed successfully.
 *
 * @param network The network that the network creator plugin successfully
 * formed. Ver.: always
 * @param usedSecondaryChannels Whether or not the network creator wants to
 * form a network on the secondary channels Ver.: always
 */
void emberAfPluginNetworkCreatorCompleteCallback(const EmberNetworkParameters *network,
                                                 bool usedSecondaryChannels)
{
    emberAfCorePrintln("yh network form");
    halReboot();
}

/** @brief Message Sent
 *
 * This function is called by the application framework from the message sent
 * handler, when it is informed by the stack regarding the message sent status.
 * All of the values passed to the emberMessageSentHandler are passed on to this
 * callback. This provides an opportunity for the application to verify that its
 * message has been sent successfully and take the appropriate action. This
 * callback should return a bool value of true or false. A value of true
 * indicates that the message sent notification has been handled and should not
 * be handled by the application framework.
 *
 * @param type   Ver.: always
 * @param indexOrDestination   Ver.: always
 * @param apsFrame   Ver.: always
 * @param msgLen   Ver.: always
 * @param message   Ver.: always
 * @param status   Ver.: always
 */
boolean emberAfMessageSentCallback(EmberOutgoingMessageType type,
                                   int16u indexOrDestination,
                                   EmberApsFrame* apsFrame,
                                   int16u msgLen,
                                   int8u* message,
                                   EmberStatus status)
{
  uint8_t yhError[8];
  yhError[0] = PROTOCOL_SEND_ERROR_CMD>>8;
  yhError[1] = PROTOCOL_SEND_ERROR_CMD;
  yhError[2] = indexOrDestination>>8;
  yhError[3] = indexOrDestination;
  yhError[4] = apsFrame->destinationEndpoint;
  yhError[5] = status;
  yhError[6] = apsFrame->clusterId>>8;
  yhError[7] = apsFrame->clusterId;
  if(status != 0x66)
    return false;
  protocolFramingFunction(PROTOCOL_VERSION,protocolData.seq, PROTOCOL_NORMAL_FRAM_TYPE,PROTOCOL_SEND_ERROR_CMD_LEN,yhError);
  emberAfCorePrintln("yh tx status = %x ",status);
  return false;
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
  halCommonGetToken(&testResetCunt.testReset, TOKEN_TEST_RESET);
  emberAfCorePrintln("ht test testResetCunt.testReset = %04x",testResetCunt.testReset);
  emberAfCorePrintln("ht test testResetCunt.testReset dd = %d",testResetCunt.testReset);

  if(testResetCunt.testReset > YH_TEST_RESET_MAX_CUNT){
      emberAfCorePrintln("ht test testResetCunt.testReset over ");
     // while(1);
  }
  halCommonIncrementCounterToken(TOKEN_TEST_RESET);
  emberEventControlSetDelayMS(appPowerOnEventControl,100);
  clearSerialBuffer();
  ringBufferInit();
  rfRingBufferInit();
  lastSerialRxTime = halCommonGetInt16uMillisecondTick();
  lastPopRfTime = halCommonGetInt16uMillisecondTick();

  if(EMBER_JOINED_NETWORK == emberAfNetworkState() ){

      emberAfCorePrintln("ll has network ");

  }else if( EMBER_NO_NETWORK == emberAfNetworkState() ){

    emberAfCorePrintln("ll no network ");

  }

}

/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 *
 */
void emberAfMainTickCallback(void)
{
  serialRxProcess(HAL_SERIAL_PORT_USART2);//pushRingBuff
  popRingBufferOneFrame();
  rfPopRingBufferOneFrame();
  //serialOtaTaskRun();
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
  uint8_t yhDataTemp[80];

  if(emberGetNodeId()  == cmd->source)  return true;

  //3.0 device data

  switch(cmd->apsFrame->clusterId){

    case YH_BASIC:
    {
      switch(cmd->buffer[2])
      {
        case 0x0a://report arrtibutes  :
        {
            if(cmd->buffer[3] == 0x0c && cmd->buffer[4] == 0x41 )//INFRARED_TRANSPONDER atttibutes
            {
                yhDataTemp[0] = PROTOCOL_INFRARED_REPROT_CMD>>8;
                yhDataTemp[1] = PROTOCOL_INFRARED_REPROT_CMD;
                yhDataTemp[2] = cmd->source>>8;
                yhDataTemp[3] = cmd->source;
                yhDataTemp[4] = cmd->bufLen-6;
                memmove(&yhDataTemp[5], &cmd->buffer[6], yhDataTemp[4]);
                protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,cmd->bufLen-1,yhDataTemp);
            }
            else if(cmd->buffer[3] == 0x00 && cmd->buffer[4] == 0x42 )//DOOR LOCK
            {
                yhDataTemp[0] = PROTOCOL_DOOR_LOCK_REPORT_CMD>>8;
                yhDataTemp[1] = PROTOCOL_DOOR_LOCK_REPORT_CMD;
                yhDataTemp[2] = cmd->source>>8;
                yhDataTemp[3] = cmd->source;
                yhDataTemp[4] = cmd->bufLen-6;
                memmove(&yhDataTemp[5], &cmd->buffer[6], yhDataTemp[4]);
                protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,cmd->bufLen-1,yhDataTemp);
            }
        }
        break;

      }
    }
    break;

    case  YH_POWER_CONFIGURATION:
    {
        switch(cmd->buffer[2])
        {
            case 0x0a://report arrtibutes
            {
                yhDataTemp[0] = PROTOCOL_REPORT_ARRIBUTE>>8;
                yhDataTemp[1] = PROTOCOL_REPORT_ARRIBUTE;
                yhDataTemp[2] = cmd->apsFrame->sourceEndpoint;
                yhDataTemp[3] = cmd->source>>8;
                yhDataTemp[4] = cmd->source;
                yhDataTemp[5] = cmd->apsFrame->clusterId>>8;
                yhDataTemp[6] = cmd->apsFrame->clusterId;
                yhDataTemp[7] = cmd->buffer[4];
                yhDataTemp[8] = cmd->buffer[3];
                yhDataTemp[9] = cmd->buffer[5];
                yhDataTemp[10] = cmd->buffer[6];
                protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,11,yhDataTemp);
            }
            break;
        }

    }
    break;

    case YH_IAS_ZONE:
    {
        switch(cmd->buffer[2])//zcl cmd  12 13
        {
            case 0x00://zone status change notification
            {
                yhDataTemp[0] = PROTOCOL_ZONE_STATUS_CHANGE_NOTIFICATION>>8;
                yhDataTemp[1] = PROTOCOL_ZONE_STATUS_CHANGE_NOTIFICATION;
                yhDataTemp[2] = cmd->apsFrame->sourceEndpoint;
                yhDataTemp[3] = cmd->source>>8;
                yhDataTemp[4] = cmd->source;
                yhDataTemp[5] = cmd->apsFrame->clusterId>>8;
                yhDataTemp[6] = cmd->apsFrame->clusterId;
                yhDataTemp[7] = cmd->buffer[4];
                yhDataTemp[8] = cmd->buffer[3];
                protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,9,yhDataTemp);
            }
            break;

            case 0x01: //read attributes response
            {
              yhDataTemp[0] = PROTOCOL_READ_ARRIBUTES_RESPONSE>>8;
              yhDataTemp[1] = PROTOCOL_READ_ARRIBUTES_RESPONSE;
              yhDataTemp[2] = cmd->apsFrame->sourceEndpoint;
              yhDataTemp[3] = cmd->source>>8;
              yhDataTemp[4] = cmd->source;
              yhDataTemp[5] = cmd->apsFrame->clusterId>>8;
              yhDataTemp[6] = cmd->apsFrame->clusterId;
              yhDataTemp[7] = cmd->bufLen;
              if(cmd->bufLen == 20)
              {
                  yhDataTemp[7] = 0x06;
                  yhDataTemp[8] = cmd->buffer[9];
                  yhDataTemp[9] = cmd->buffer[8];
                  yhDataTemp[10] = cmd->buffer[10];
                  yhDataTemp[11] = cmd->buffer[11];
                  yhDataTemp[12] = cmd->buffer[13];
                  yhDataTemp[13] = cmd->buffer[12];

                  protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,14,yhDataTemp);
              }
              //yhZclGlobalWriteCommand
            }
            break;

            case 0x0a://report arrtibutes
            {
                yhDataTemp[0] = PROTOCOL_REPORT_ARRIBUTE>>8;
                yhDataTemp[1] = PROTOCOL_REPORT_ARRIBUTE;
                yhDataTemp[2] = cmd->apsFrame->sourceEndpoint;
                yhDataTemp[3] = cmd->source>>8;
                yhDataTemp[4] = cmd->source;
                yhDataTemp[5] = cmd->apsFrame->clusterId>>8;
                yhDataTemp[6] = cmd->apsFrame->clusterId;
                yhDataTemp[7] = cmd->buffer[4];
                yhDataTemp[8] = cmd->buffer[3];
                yhDataTemp[9] = cmd->buffer[5];
                yhDataTemp[10] = cmd->buffer[7];
                yhDataTemp[11] = cmd->buffer[6];
                protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,12,yhDataTemp);
            }
            break;
        }
    }
    break;

    case  YH_WINDOW_COVERING:
    {
      switch(cmd->buffer[2])
      {
          case 0x0a://report arrtibutes
          {
            yhDataTemp[0] = PROTOCOL_CURTAIN_REPORT_CMD>>8;
            yhDataTemp[1] = PROTOCOL_CURTAIN_REPORT_CMD;
            yhDataTemp[2] = cmd->source>>8;
            yhDataTemp[3] = cmd->source;
            yhDataTemp[4] = cmd->buffer[4];
            yhDataTemp[5] = cmd->buffer[3];
            memmove(&yhDataTemp[6],&cmd->buffer[5],cmd->bufLen-5);
            protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,cmd->bufLen+1,yhDataTemp);
          }
          break;
      }

    }
    break;

  }

  //old
  if(((cmd->commandId) == 0x00)&&(cmd->clusterSpecific)
      &&(cmd->apsFrame->clusterId == 0xfc00)
      &&(!cmd->mfgSpecific))
  {
      // cmd->buffer
      //byte0  frame control
      //byte1  sequence number
      //byte2  command id
      //byte3  length
      //.....payload (fourth byte is first payload)
      //emberSerialWriteData(1, &(cmd->buffer[4]), cmd->bufLen-4);
      //emberSerialWaitSend(1);

      emberAfCorePrintln("emberAfPreCommandReceivedCallback test data");

    if(cmd->buffer[4] == FRAME_HEADER)
    {
        rfWriteRingBuffer(cmd->bufLen-4);
        rfPushBufferToRing(&(cmd->buffer[4]),cmd->bufLen-4);
    }
    return true;
  }else if(cmd->apsFrame->clusterId == 0xfc01){

      rfWriteRingBuffer(cmd->bufLen-3);
      rfPushBufferToRing(&(cmd->buffer[3]),cmd->bufLen-3);

  }else{
      return false;
  }
}

/** @brief Pre ZDO Message Received
 *
 * This function passes the application an incoming ZDO message and gives the
 * appictation the opportunity to handle it. By default, this callback returns
 * false indicating that the incoming ZDO message has not been handled and
 * should be handled by the Application Framework.
 *
 * @param emberNodeId   Ver.: always
 * @param apsFrame   Ver.: always
 * @param message   Ver.: always
 * @param length   Ver.: always
 */
boolean emberAfPreZDOMessageReceivedCallback(EmberNodeId emberNodeId,
                                             EmberApsFrame* apsFrame,
                                             int8u* message,
                                             int16u length)
{
  //leave command Noted: must define macro
  //EMBER_APPLICATION_RECEIVES_SUPPORTED_ZDO_REQUESTS
  //in your application
  uint8_t i=0;
  uint8_t yhTemp[80];
  switch(apsFrame->clusterId){

    case LEAVE_REQUEST:
    {
        emberAfCorePrintln("yh test leave ");

        if(message[9] & LEAVE_REQUEST_REJOIN_FLAG)//rejoin flag
        {

            emberFindAndRejoinNetworkWithReason(1,
                                                 EMBER_ALL_802_15_4_CHANNELS_MASK,
                                                 EMBER_AF_REJOIN_DUE_TO_END_DEVICE_MOVE);
        }
    }
    break;

    case YH_DEVICE_ANNOUNCE:
    {
        yhTemp[0] = PROTOCOL_DEVICE_ANNOUNCE_CMD>>8;
        yhTemp[1] = PROTOCOL_DEVICE_ANNOUNCE_CMD;
        yhTemp[2] = message[2];
        yhTemp[3] = message[1];
        for(i=8;i>0;i--)
        {
            yhTemp[4+8-i] = message[2+i];
        }

        memmove(&yhBind.ieee[0],&message[3],8);
        yhTemp[12] = message[11];
        emberAfCorePrintln("yh YH_DEVICE_ANNOUNCE message = ");
        for(i=0;i<length;i++){
           emberAfCorePrint("%x",message[i]);
        }

        protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,13,yhTemp);

        //add by yh
        emberAfAppPrint("Active EP request status: 0x%X",
                        emberActiveEndpointsRequest(emberNodeId, EMBER_APS_OPTION_RETRY));
    }
    break;

    case ACTIVE_ENDPOINTS_RESPONSE:
    {
        emberAfCorePrintln("yh ACTIVE_ENDPOINTS_RESPONSE emberNodeId = %2x ",emberNodeId);
        emberAfCorePrintln("yh ACTIVE_ENDPOINTS_RESPONSE length = %x ",length);

        emberAfCorePrintln("yh ACTIVE_ENDPOINTS_RESPONSE message = ");
        for(i=0;i<length;i++){
            emberAfCorePrint("%x",message[i]);
        }
        emberAfCorePrintln("    yh ACTIVE_ENDPOINTS_RESPONSE message end ");

        //add by yh
        emberAfAppPrintln("ZDO simple desc req %x", emberSimpleDescriptorRequest(emberNodeId,\
                                                                                 1,\
                                                                                 EMBER_AF_DEFAULT_APS_OPTIONS));

    }
    break;

    case SIMPLE_DESCRIPTOR_RESPONSE:
    {
        yhTemp[0] = PROTOCOL_SIMPLE_DESCRIPTOR_RESPONSE>>8;
        yhTemp[1] = PROTOCOL_SIMPLE_DESCRIPTOR_RESPONSE;
        yhTemp[2] = message[1];
        yhTemp[3] = message[3];
        yhTemp[4] = message[2];
        yhTemp[5] = message[4];
        yhTemp[6] = message[5];
        yhTemp[7] = message[7];
        yhTemp[8] = message[6];
        yhTemp[9] = message[9];
        yhTemp[10] = message[8];
        memmove(&yhTemp[11],&message[10],message[4]-5);
        protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,message[4]+6,yhTemp);

        //send bind
        yhBind.deviceId = message[8] | message[9]<<8;
        if(yhBind.deviceId == 0x0402)
        {
            emberAfCorePrint("\r\n device mac = ");
            for(i=0;i<8;i++){
                emberAfCorePrint("%x ",yhBind.ieee[i]);
            }
            emberAfCorePrint("");
            yhZdoBindCommand(emberNodeId, 1, 1, 0x0500, yhBind.ieee);
        }

    }
    break;

    case IEEE_ADDRESS_RESPONSE:
    {
        yhTemp[0] = PROTOCOL_IEEE_ADDRESS_RESPONSE>>8;
        yhTemp[1] = PROTOCOL_IEEE_ADDRESS_RESPONSE;
        yhTemp[2] = apsFrame->sourceEndpoint;
        yhTemp[3] = emberNodeId>>8;
        yhTemp[4] = emberNodeId;
        yhTemp[5] = apsFrame->clusterId>>8;
        yhTemp[6] = apsFrame->clusterId;
        yhTemp[7] = message[1];
        for(i=8;i>0;i--)
        {
            yhTemp[8+8-i] = message[1+i];
        }

        yhTemp[16] = message[11];
        yhTemp[17] = message[10];
        protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,18,yhTemp);
        //add by yh
        emberAfAppPrintln("ZDO simple desc req %x", emberSimpleDescriptorRequest(emberNodeId,\
                                                                                 1,\
                                                                                 EMBER_AF_DEFAULT_APS_OPTIONS));
    }
    break;

    case NETWORK_ADDRESS_RESPONSE:
    {
        yhTemp[0] = PROTOCOL_IEEE_ADDRESS_RESPONSE>>8;
        yhTemp[1] = PROTOCOL_IEEE_ADDRESS_RESPONSE;
        yhTemp[2] = apsFrame->sourceEndpoint;
        yhTemp[3] = emberNodeId>>8;
        yhTemp[4] = emberNodeId;
        yhTemp[5] = apsFrame->clusterId>>8;
        yhTemp[6] = apsFrame->clusterId;
        yhTemp[7] = message[1];
        for(i=8;i>0;i--)
        {
            yhTemp[8+8-i] = message[1+i];
        }

        yhTemp[16] = message[11];
        yhTemp[17] = message[10];
        protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,18,yhTemp);
    }
    break;

    case BIND_RESPONSE:
    {
      //uint8_t send[] = "send 0x0000 1 1";
      EmberStatus status;
      //yhZclGlobalSendMeAReportCommand();
        emberProcessCommandString((uint8_t*)"zcl global send-me-a-report 0x0500 0x0002 0x19 0 2400 {0000}", strlen("zcl global send-me-a-report 0x0500 0x0002 0x19 0 2400 {0000}"));
        emberProcessCommandString((uint8_t*)"\n", strlen("\n"));
        //emberAfSetCommandEndpoints(1,1);
        emAfApsFrameEndpointSetup(1,1);
        status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                    emberNodeId,
                                    &globalApsFrame,
                                    appZclBufferLen,
                                    appZclBuffer);
        if (status != EMBER_SUCCESS)
        {
            emberAfCorePrintln("send me report error");
        }
        //emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberNodeId);
        emberAfCorePrintln("yh send-me-a-report ######## ");
    }
    break;

  }

#if 0         //cut lu
    if (apsFrame->clusterId == LEAVE_REQUEST)
    {
         if(message[9] & LEAVE_REQUEST_REJOIN_FLAG)//rejoin flag
         {

             emberFindAndRejoinNetworkWithReason(1,
                                                  EMBER_ALL_802_15_4_CHANNELS_MASK,
                                                  EMBER_AF_REJOIN_DUE_TO_END_DEVICE_MOVE);
         }
         else
         {

         }
    }
#endif

    return false;
}

/** @brief Trust Center Join
 *
 * This callback is called from within the application framework's
 * implementation of emberTrustCenterJoinHandler or ezspTrustCenterJoinHandler.
 * This callback provides the same arguments passed to the
 * TrustCenterJoinHandler. For more information about the TrustCenterJoinHandler
 * please see documentation included in stack/include/trust-center.h.
 *
 * @param newNodeId   Ver.: always
 * @param newNodeEui64   Ver.: always
 * @param parentOfNewNode   Ver.: always
 * @param status   Ver.: always
 * @param decision   Ver.: always
 */
void emberAfTrustCenterJoinCallback(EmberNodeId newNodeId,
                                    EmberEUI64 newNodeEui64,
                                    EmberNodeId parentOfNewNode,
                                    EmberDeviceUpdate status,
                                    EmberJoinDecision decision)
{
  uint8_t yhDeviceLeave[12];
  uint8_t i;


  switch(status){
    case EMBER_STANDARD_SECURITY_SECURED_REJOIN:
      emberAfCorePrintln("yh test emberAfTrustCenterJoinCallback EMBER_STANDARD_SECURITY_SECURED_REJOIN ");
      break;

    case EMBER_STANDARD_SECURITY_UNSECURED_JOIN:
      emberAfCorePrintln("yh test emberAfTrustCenterJoinCallback EMBER_STANDARD_SECURITY_UNSECURED_JOIN ");
      break;

    case EMBER_DEVICE_LEFT:
      yhDeviceLeave[0] = PROTOCOL_NODE_LEAVE_NET_CMD>>8;
      yhDeviceLeave[1] = PROTOCOL_NODE_LEAVE_NET_CMD;
      yhDeviceLeave[2] = newNodeId>>8;
      yhDeviceLeave[3] = newNodeId;
      for(i=8;i>0;i--)
      {
          yhDeviceLeave[4+8-i] = newNodeEui64[i-1];
      }

      emberAfCorePrintln("yh node leave network data:");
      for(uint8_t i=0;i<12;i++){
          emberAfCorePrint("%x",yhDeviceLeave[i]);
      }
      protocolDataPushRingBuffer(PROTOCOL_VERSION,sendSeq, PROTOCOL_NORMAL_FRAM_TYPE,12,yhDeviceLeave);
      emberAfCorePrintln("yh test emberAfTrustCenterJoinCallback EMBER_DEVICE_LEFT ");
      break;

    case EMBER_STANDARD_SECURITY_UNSECURED_REJOIN:
      emberAfCorePrintln("yh test emberAfTrustCenterJoinCallback EMBER_STANDARD_SECURITY_UNSECURED_REJOIN ");
      break;

    default:
      emberAfCorePrintln("yh test emberAfTrustCenterJoinCallback default ");
      break;
  }

}

//yh app
extern  void emAfCliServiceDiscoveryCallback(const EmberAfServiceDiscoveryResult* result);
void yhZdoIeeeAddressRequestCommand(EmberNodeId nodeId)
{
  emberAfFindIeeeAddress(nodeId,
                         emAfCliServiceDiscoveryCallback);
}

void yhZdoLeaveRequestCommand(EmberNodeId target)
{
  //EmberNodeId target = (EmberNodeId)emberUnsignedCommandArgument(0);
  // CCB 2047
  // - CCB makes the first step to depracate the 'leave and remove children' functionality.
  // - We were proactive here and deprecated it right away.
  // bool removeChildren = (bool)emberUnsignedCommandArgument(1);
  //bool rejoin = (bool)emberUnsignedCommandArgument(2); cut by yh
  bool rejoin = 0;
  EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
  EmberStatus status;

  uint8_t options = 0;
  if (rejoin) {
    options |= EMBER_ZIGBEE_LEAVE_AND_REJOIN;
  }

  status = emberLeaveRequest(target,
                             nullEui64,
                             options,
                             EMBER_APS_OPTION_RETRY);

  emberAfAppPrintln("Leave %p0x%X", "Request: ", status);
}

//

//end of yh app


//yh
void appPowerOnHandler(void){

  //EmberNodeType nodeType;

  emberEventControlSetInactive(appPowerOnEventControl);
  modeInitialFromToken();
  powerOnSuccess = true;

#ifdef RF_SCHEDULE_TEST
  emberEventControlSetDelayMS(scheduleRfTestEventControl,15000);
#else
  //emberEventControlSetInactive(scheduleRfTestEventControl);
  emberEventControlSetDelayMS(scheduleRfTestEventControl,15000);
#endif

}


void scheduleRfTestHandler(void){

  uint8_t dat[65];
      uint8_t res;
      uint8_t m;




    // res =  emberAfPluginEepromRead(0x0, dat, 64);
     //emberSerialPrintf(1,"\r\nres is %d\n,",res);
     //for(m=0;m<64;m++)
     //  emberSerialPrintf(1,"%x ",dat[m]);

    //  halCommonGetToken(&otaInfo,TOKEN_MODULE_OTA_INFO);
    //  emberSerialPrintf(1,"\r\nversion is %d\n,",otaInfo.version);
      //halCommonGetToken(&otaDownloadInfo,TOKEN_MODULE_OTA_DOWNLOAD_INFO);
     // emberSerialPrintf(1,"\r\n download info is %d\n,",otaDownloadInfo.statusPerPack[100]);


    #ifdef RF_SCHEDULE_TEST
      static uint8_t i = 1;
      //EmberStatus status;
      if(0 == GPIO_PinInGet(gpioPortB,14))
      {
          emberEventControlSetDelayMS(scheduleRfTestEventControl,100);
          rfSendCommandDirectTxTestCommand(i);
          //emberSerialPrintf(1,"\r\nschedule %d,status :0x%x",i,status);
          i++;
          if(i > RF_SCHEDULE_END_INDEX)
          {
              i = 1;
              emberEventControlSetDelayMS(scheduleRfTestEventControl,15000);
          }
      }
      else
      {
          emberEventControlSetDelayMS(scheduleRfTestEventControl,15000);
          i = 1;
      }
  #else
      emberEventControlSetInactive(scheduleRfTestEventControl);
  #endif

}

void otaScheduleDelayHandler(void){

  emberEventControlSetInactive(otaScheduleDelayEventControl);
  deviceQueryImage = false;

}

void yhZdoBindCommand(EmberNodeId target, uint8_t sourceEndpoint, uint8_t destinationEndpoint, uint16_t clusterId, EmberEUI64 sourceEui)
{
  EmberStatus status;
  EmberEUI64  destEui;  // names relative to binding sent over-the-air
#if 0
  //EmberNodeId target = (EmberNodeId)emberUnsignedCommandArgument(0);
  //uint8_t sourceEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  //uint8_t destinationEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  //uint16_t clusterId = (uint16_t)emberUnsignedCommandArgument(3);

  // NOTE:  The source/dest EUI is relative to the context.
  // In the case of the syntax of the CLI, we take "THEIR EUI64" to mean
  // the recipient of the binding request message.  However, when sending
  // the bind request that EUI64 used by the ZDO command is
  // actually the source for the binding because we are telling the remote
  // device (the destination) to create a binding with a source of itself.
  // And the destination for that binding will be this local device.
  // This is also not to be confused with the (short) destination of the ZDO
  // request itself.
  if (EMBER_SUCCESS != copyOrLookupEui64(4, target, sourceEui)) {
    return;
  }

  // If the last argument is empty, assume an EUI64 of the local device.
  // This allows for the simple case.  If an EUI64 is specified, it will
  // be used instead of the local EUI.  This is used for setting
  // multiple bindings on the same remote device.
  if (0 == emberCopyBigEndianEui64Argument(5, destEui)) {
    emberAfAppPrintln("Using my local EUI64 for dest EUI64 in binding");
    emberAfGetEui64(destEui);
  }
#endif
  emberAfGetEui64(destEui);
  status = emberBindRequest(target,           // who gets the bind req
                            sourceEui,       // source eui IN the binding
                            sourceEndpoint,
                            clusterId,
                            UNICAST_BINDING, // binding type
                            destEui,         // destination eui IN the binding
                            0,               // groupId for new binding
                            destinationEndpoint,
                            EMBER_AF_DEFAULT_APS_OPTIONS);
  UNUSED_VAR(status);
  emberAfAppPrintln("ZDO bind req %x", status);
}

//send me a report
void yhZclGlobalSendMeAReportCommand(void)
{
  uint8_t type = 0x19;//(uint8_t)emberUnsignedCommandArgument(2);

  zclBufferSetup(ZCL_GLOBAL_COMMAND ,
                 (EmberAfClusterId)0x0500,//emberUnsignedCommandArgument(0), // cluster id
                 ZCL_CONFIGURE_REPORTING_COMMAND_ID);
  zclBufferAddByte(EMBER_ZCL_REPORTING_DIRECTION_REPORTED);
  //zclBufferAddWordFromArgument(1);  // attribute id
  zclBufferAddWord(0x0002);
  zclBufferAddByte(type);           // type
  //zclBufferAddWordFromArgument(3);  // minimum reporting interval
  zclBufferAddWord(0x0000);
  //zclBufferAddWordFromArgument(4);  // maximum reporting interval
  //zclBufferAddWord(0x0708);
  zclBufferAddWord(0x0010);
  // If the data type is analog, then the reportable change field is the same
  // size as the data type.  Otherwise, it is omitted.
#if 0
  if (emberAfGetAttributeAnalogOrDiscreteType(type)
      == EMBER_AF_DATA_TYPE_ANALOG) {
    uint8_t dataSize = emberAfGetDataSize(type);
    emberCopyStringArgument(5,
                            appZclBuffer + appZclBufferLen,
                            dataSize,
                            false);
    appZclBufferLen += dataSize;
  }
#endif
  //zclBufferAddWord(0x0000);
  //appZclBufferLen += 2;
  cliBufferPrint();
}

//zcl write command
yhWriteCommandtype yhInfraredData;
void yhZclGlobalWriteCommand(uint8_t type, uint8_t commandId, uint16_t clusterId, uint16_t attrId, uint8_t* data, uint8_t length)
{
  if (commandId != ZCL_WRITE_ATTRIBUTES_COMMAND_ID)
    return;
#if 0
  uint8_t type = (uint8_t)emberUnsignedCommandArgument(2);
  uint8_t commandChar = emberCurrentCommand->name[0];
  uint8_t commandId = (commandChar == 'u'
                       ? ZCL_WRITE_ATTRIBUTES_UNDIVIDED_COMMAND_ID
                       : (commandChar == 'n'
                          ? ZCL_WRITE_ATTRIBUTES_NO_RESPONSE_COMMAND_ID
                          : ZCL_WRITE_ATTRIBUTES_COMMAND_ID));
#endif

  //zclGlobalSetup(commandId);
  zclBufferSetup(ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 clusterId,
                 commandId);

  //zclBufferAddWordFromArgument(1);  // attr id
  zclBufferAddWord((uint16_t)attrId);

  zclBufferAddByte(type);

  if (emberAfIsThisDataTypeAStringType(type)) {
    //bufferAddLengthAndStringFromArgument(3, emberAfIsLongStringAttributeType(type));
    uint8_t prefixSize = 1;
    uint16_t maxLength = APP_ZCL_BUFFER_SIZE - (appZclBufferLen + prefixSize);
    if (maxLength > MAX_INT8U_VALUE) {
      maxLength = MAX_INT8U_VALUE;
    }

    MEMSET(appZclBuffer + appZclBufferLen + prefixSize, 0, length);
    MEMMOVE(appZclBuffer + appZclBufferLen + prefixSize, data, length);

    appZclBuffer[appZclBufferLen] = length;
    if (prefixSize == 2) {
      // CLI max string length is 255, so LONG string upper length byte is zero.
      appZclBuffer[appZclBufferLen + 1] = 0;
    }
    appZclBufferLen += length + prefixSize;
  } else {
    uint8_t length = emberAfGetDataSize(type);
    emberCopyStringArgument(3,
                            appZclBuffer + appZclBufferLen,
                            length,
                            true);  // pad with zeroes
    appZclBufferLen += length;
  }

  cliBufferPrint();
}


uint8_t yhNwkSendRfPacket(uint8_t srcEp, uint8_t dstEp, uint16_t dstAddr, uint8_t frameControl, uint16_t clusterID, uint8_t command, uint8_t *buf, uint8_t len)
{

    EmberApsFrame *apsFrame = emberAfGetCommandApsFrame();
    apsFrame->profileId           = emberAfPrimaryProfileId();
    apsFrame->sourceEndpoint      = srcEp;
    apsFrame->destinationEndpoint = dstEp;

    if (frameControl&ZCL_MANUFACTURER_SPECIFIC_MASK)
    {
        emberAfFillExternalManufacturerSpecificBuffer(frameControl,
                                                      clusterID,
                                                      0x1002,
                                                      command,
                                                      "b",
                                                      buf,
                                                      len);
    }
    else
    {
        emberAfFillExternalBuffer(frameControl,
                                  clusterID,
                                  command,
                                  "b",
                                  buf,
                                  len);
    }


    return emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, dstAddr);
}

