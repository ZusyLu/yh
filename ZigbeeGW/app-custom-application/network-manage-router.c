#include "app/framework/include/af.h"
#include EMBER_AF_API_NETWORK_STEERING
#include "app/framework/util/attribute-storage.h"
#include "app/framework/plugin/ota-client/ota-client.h"
#include "app/framework/plugin/reporting/reporting.h"
#include "util/plugin/plugin-common/eeprom/eeprom.h"
#include "app-device-ui.h"
#include "app/framework/plugin/find-and-bind-target/find-and-bind-target.h"
#include "app/framework/plugin/network-creator/network-creator.h"

#ifdef EMBER_AF_PLUGIN_IAS_ZONE_SERVER
#include "app/framework/plugin/ias-zone-server/ias-zone-server.h"
#endif
//macro define
#define PRIMARY_ENDPOINT            1
#define NETWORK_JOIN_TIMER_OUT_TICK 60 //Per tick is 3 second

#define REBOOT_NETWORK_LEAVE_DELAY  1500   //ms
#define REBOOT_ERASE_FLASH_DELAY    4500   //ms
typedef enum {
  REBOOT_STATE_NONE = 0x01,
  REBOOT_STATE_LEAVE_NETWORK = 0x02,
  REBOOT_STATE_ERASE_FLASH = 0x03,
} RebootState;
RebootState rebootState = REBOOT_STATE_NONE;
//event
EmberEventControl appNetworkJoinEventControl;
EmberEventControl halRebootEventControl;
//
uint16_t joinNetworkTickTimer = 0;  //per tick is 3 seconds
uint16_t joinNetworkButtonPressedTimer = 0;
bool     joinNetworkButtonRelesed = true;
bool     joinNetworkButtonLongPressed = false;//0-false  1-true
bool     joiningNetworkRuuning = false;
/** @brief halRebootEventHandler
 *
 * This function is used for reboot event
 * after device reset to factory
*/
void halRebootEventHandler(void)
{
    emberEventControlSetInactive(halRebootEventControl);
    if(rebootState == REBOOT_STATE_LEAVE_NETWORK)
    {
       emberEventControlSetDelayMS(halRebootEventControl,REBOOT_ERASE_FLASH_DELAY);;
       rebootState = REBOOT_STATE_ERASE_FLASH;
       //emberAfPluginEepromErase(0,EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_STORAGE_END);
    }
    else if(rebootState == REBOOT_STATE_ERASE_FLASH)
    {
       halReboot();
    }
}
 /** @brief appNetworkJoinHandler
 *
 * This function is used for device join network procedure
 * and product test rejoin procedure
*/
void appNetworkJoinHandler(void)
{
    EmberNetworkStatus networkStatus;
    networkStatus = emberAfNetworkState();
#if PRODUCT_TEST_MODE_ENABLE
    if(mfgProductTestFlagGet())
    {
       customDeviceLedActionShow(ACTION_PRD_TEST,0);
       emberEventControlSetInactive(appNetworkJoinEventControl);
        return;
    }
    if(mfgGetRFTestMode())
    return;
#endif
    joinNetworkTickTimer++;
    if(joinNetworkTickTimer >= NETWORK_JOIN_TIMER_OUT_TICK )
    {
         emberEventControlSetInactive(appNetworkJoinEventControl);
         emberAfPluginNetworkSteeringStop();
         customDeviceLedActionShow(ACTION_NONE,0);
         joiningNetworkRuuning = false;
         joinNetworkTickTimer = 0;
         //timeout creat distribute netowrk here
         emberAfPluginNetworkCreatorStart(false); // distributed
    }
    else if(networkStatus == EMBER_NO_NETWORK)
    {
          if(joinNetworkTickTimer <= 1)
          {
             emberAfPluginNetworkSteeringRest();
             customDeviceLedActionShow(ACTION_JOINING_NETWORK,0);
             joiningNetworkRuuning = true;
          }
          emberAfPluginNetworkSteeringStart();
          emberEventControlSetDelayQS(appNetworkJoinEventControl,12);//
    }
    else if(networkStatus == EMBER_JOINED_NETWORK)
    {
          emberEventControlSetInactive(appNetworkJoinEventControl);
          joinNetworkTickTimer = 0;
    }
    else
    {
        emberEventControlSetDelayQS(appNetworkJoinEventControl,12);//
    }
}
/** @brief rfTestForceNetworkScanStop
 *
 * This function is called when go to RF test and PCBA test
 * this function will cause the device leave network
 * @param none
 */
void rfTestForceNetworkScanStop(void)
{
   //leave network and disable join
    emberAfPluginNetworkSteeringStop();
    emberEventControlSetInactive(appNetworkJoinEventControl);
    emberClearBindingTable();
    joiningNetworkRuuning = false;
    joinNetworkButtonPressedTimer = 0;
    emberLeaveNetwork();
}
/** @brief resetToFactoryState
 *
 * This function is called when force to leave network
 * clear binding-table report table reset attribute to default value
 * clear group table and scene table if the device have these
 * @param none
 */
void resetToFactoryState(void)
{
     uint8_t endpointIndex;
     uint8_t endpoint;
     emberLeaveNetwork();
    // emberAfPluginNetworkSteeringRest();
     emberClearBindingTable();
     emberAfClearReportTableCallback();
     for (endpointIndex = 0; endpointIndex < emberAfEndpointCount();
           endpointIndex++) {
        endpoint = emberAfEndpointFromIndex(endpointIndex);
        emberAfResetAttributes(endpoint);
        emberAfGroupsClusterClearGroupTableCallback(endpoint);
        emberAfScenesClusterClearSceneTableCallback(endpoint);
     }
}
/** @brief resetToFactoryStateAndReboot
 *
 * This function is called when force to leave network
 * clear binding-table report table reset attribute to default value
 * clear group table and scene table if the device have these
 * @param none
 */
void resetToFactoryStateAndReboot(void)
{
     resetToFactoryState();
     emberEventControlSetDelayMS(halRebootEventControl,REBOOT_NETWORK_LEAVE_DELAY);
     rebootState = REBOOT_STATE_LEAVE_NETWORK;
}
/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 *
 *
 */
#if 0
boolean emberAfStackStatusCallback(EmberStatus status)
{

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
            //led steady on 10s
            customDeviceLedActionShow(ACTION_NONE,0);
            customDeviceLedActionShow(ACTION_STEADY_ON_XS,10);
            joiningNetworkRuuning = false;
        }
    }
    return true;
}
#endif
