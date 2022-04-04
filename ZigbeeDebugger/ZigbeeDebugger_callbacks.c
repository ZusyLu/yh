// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include "hal/hal.h"
#include EMBER_AF_API_NETWORK_STEERING
#include "app/framework/include/af.h"
#include "app/util/ezsp/ezsp-enum.h"

//yh                                                        help   EVENT

#include "serial-rx-task.h"
#include "serial-rx-queue.h"
#include "rf-rx-uart-tx-task.h"
//#include "rf-tx-command.h"
EmberEventControl yhInitEvent;
void yhInitEventHandler(void);

void HTemAfInterpanFragmentTestCommand(EmberPanId htPanId,uint8_t *htMacId , \
                                       uint16_t HTclusterId,uint8_t *data,uint16_t HTmessageLen);
uint8_t yhChangeChannel(uint8_t channel);
void yhNetworkCreatorFormFunction(void);

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
  if(status == EMBER_CHANNEL_CHANGED)//0x9b
  {
      emberAfCorePrintln("yh change channel ok");
  }


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
  clearSerialBuffer();
  ringBufferInit();
  rfRingBufferInit();

  emberEventControlSetDelayMS(yhInitEvent,200);

  lastSerialRxTime = halCommonGetInt16uMillisecondTick();
  lastPopRfTime = halCommonGetInt16uMillisecondTick();
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

  emberAfCorePrintln("interpan data = ");
  for(uint8_t i=0;i<msgLen;i++)
  {
      emberAfCorePrint(" %x ",message[i]);
  }
  emberAfCorePrintln(" ");

  if(header->clusterId == 0xfc99)
  {
      emberSerialWriteData(HAL_SERIAL_PORT_USART2, message, msgLen);
      emberSerialWaitSend(HAL_SERIAL_PORT_USART2);

  }else{
      rfWriteRingBuffer(msgLen);
      rfPushBufferToRing(message, msgLen);
  }

  return false;
}


//interpan
void HTemAfInterpanFragmentTestCommand(EmberPanId htPanId,uint8_t *htMacId , \
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

//change
uint8_t yhChangeChannel(uint8_t channel)
{
  EmberStatus status = emberChannelChangeRequest(channel);
  emberAfAppPrintln("Changing to channel %d: 0x%X",
                    channel,
                    status);
  return (uint8_t)status;
}

//  form <centralized:1> <panId:1> <radioTxPower:1> <channel:1>
void yhNetworkCreatorFormFunction(void)
{
  bool centralizedSecurity = (bool)1;
  EmberPanId panId = (EmberPanId)0x6868;
  int8_t radioTxPower = (int8_t)0x14;
  uint8_t channel = (uint8_t)0x0b;

  EmberStatus status = emberAfPluginNetworkCreatorNetworkForm(centralizedSecurity,
                                                              panId,
                                                              radioTxPower,
                                                              channel);

  emberAfCorePrintln("%p: %p: 0x%X",
                     "NWK Creator",
                     "Form",
                     status);
}

void yhInitEventHandler(void){

  emberEventControlSetInactive(yhInitEvent);
  emberAfCorePrintln("yh emberNetworkState =%x ",emberNetworkState());

  if(emberNetworkState() == EMBER_NO_NETWORK)
  {
      emberAfCorePrintln("yh this no net and will from a net ");
      yhNetworkCreatorFormFunction();
  }

}
