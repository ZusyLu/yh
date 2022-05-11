// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef __AF_GEN_EVENT__
#define __AF_GEN_EVENT__


// Code used to configure the cluster event mechanism
#define EMBER_AF_GENERATED_EVENT_CODE \
  EmberEventControl emberAfIasZoneClusterServerTickCallbackControl1; \
  extern EmberEventControl emberAfPluginBatteryMonitorReadADCEventControl; \
  extern EmberEventControl emberAfPluginIasZoneServerManageQueueEventControl; \
  extern EmberEventControl emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl; \
  extern EmberEventControl emberAfPluginNetworkSteeringFinishSteeringEventControl; \
  extern EmberEventControl emberAfPluginReportingTickEventControl; \
  extern EmberEventControl emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl; \
  extern EmberEventControl xxBlnkLed0Event; \
  extern EmberEventControl xxInitEvent; \
  extern EmberEventControl xxIrqButtonHandleButton0PressedEvent; \
  extern EmberEventControl xxIrqButtonHandleButton0ReleasedEvent; \
  extern EmberEventControl xxIrqButtonHandleButton1PressedEvent; \
  extern EmberEventControl xxIrqButtonHandleButton1ReleasedEvent; \
  extern EmberEventControl xxIrqButtonHandleButton2PressedEvent; \
  extern EmberEventControl xxIrqButtonHandleButton2ReleasedEvent; \
  extern EmberEventControl xxIrqButtonHandleButtonTimeOut; \
  extern EmberEventControl xx_project_button_interval_time_event; \
  extern EmberEventControl xx_project_heart_beat_event; \
  extern EmberEventControl xx_project_scan_network_event; \
  extern EmberEventControl xx_project_wait_cfg_cmd_event; \
  extern EmberEventControl xxRebootEventControl; \
  extern EmberEventControl xx_rejoin_control_event; \
  extern void emberAfPluginBatteryMonitorReadADCEventHandler(void); \
  extern void emberAfPluginIasZoneServerManageQueueEventHandler(void); \
  extern void emberAfPluginManufacturingLibraryCliCheckSendCompleteEventHandler(void); \
  extern void emberAfPluginNetworkSteeringFinishSteeringEventHandler(void); \
  extern void emberAfPluginReportingTickEventHandler(void); \
  extern void emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventHandler(void); \
  extern void xxBlnkLed0EventHandle(void); \
  extern void xxInitEventHandler(void); \
  extern void xxIrqButtonHandleButton0PressedEventHandler(void); \
  extern void xxIrqButtonHandleButton0ReleasedEventHandler(void); \
  extern void xxIrqButtonHandleButton1PressedEventHandler(void); \
  extern void xxIrqButtonHandleButton1ReleasedEventHandler(void); \
  extern void xxIrqButtonHandleButton2PressedEventHandler(void); \
  extern void xxIrqButtonHandleButton2ReleasedEventHandler(void); \
  extern void xxIrqButtonHandleButtonTimeOutHandler(void); \
  extern void xxProjectButtonIntervalTimeFuction(void); \
  extern void xxProjectHeartBeatEventHandler(void); \
  extern void xxProjectScanNetworkFuction(void); \
  extern void xxProjectWaitCfgCmdFuction(void); \
  extern void xxRebootEventHandler(void); \
  extern void xxRejoinControlEventHandler(void); \
  static void networkEventWrapper(EmberEventControl *control, EmberAfNetworkEventHandler handler, uint8_t networkIndex) \
  { \
    emberAfPushNetworkIndex(networkIndex); \
    emberEventControlSetInactive(*control); \
    (*handler)(); \
    emberAfPopNetworkIndex(); \
  } \
  EmberEventControl emberAfPluginEndDeviceSupportMoveNetworkEventControls[1]; \
  extern void emberAfPluginEndDeviceSupportMoveNetworkEventHandler(void); \
  void emberAfPluginEndDeviceSupportMoveNetworkEventWrapper0(void) { networkEventWrapper(&emberAfPluginEndDeviceSupportMoveNetworkEventControls[0], emberAfPluginEndDeviceSupportMoveNetworkEventHandler, 0); } \
  EmberEventControl emberAfPluginEndDeviceSupportPollingNetworkEventControls[1]; \
  extern void emberAfPluginEndDeviceSupportPollingNetworkEventHandler(void); \
  void emberAfPluginEndDeviceSupportPollingNetworkEventWrapper0(void) { networkEventWrapper(&emberAfPluginEndDeviceSupportPollingNetworkEventControls[0], emberAfPluginEndDeviceSupportPollingNetworkEventHandler, 0); } \
  EmberEventControl emberAfPluginScanDispatchScanNetworkEventControls[1]; \
  extern void emberAfPluginScanDispatchScanNetworkEventHandler(void); \
  void emberAfPluginScanDispatchScanNetworkEventWrapper0(void) { networkEventWrapper(&emberAfPluginScanDispatchScanNetworkEventControls[0], emberAfPluginScanDispatchScanNetworkEventHandler, 0); } \
  static void clusterTickWrapper(EmberEventControl *control, EmberAfTickFunction callback, uint8_t endpoint) \
  { \
    emberAfPushEndpointNetworkIndex(endpoint); \
    emberEventControlSetInactive(*control); \
    (*callback)(endpoint); \
    emberAfPopNetworkIndex(); \
  } \
  void emberAfIasZoneClusterServerTickCallbackWrapperFunction1(void) { clusterTickWrapper(&emberAfIasZoneClusterServerTickCallbackControl1, emberAfIasZoneClusterServerTickCallback, 1); } \


// EmberEventData structs used to populate the EmberEventData table
#define EMBER_AF_GENERATED_EVENTS   \
  { &emberAfIasZoneClusterServerTickCallbackControl1, emberAfIasZoneClusterServerTickCallbackWrapperFunction1 }, \
  { &emberAfPluginBatteryMonitorReadADCEventControl, emberAfPluginBatteryMonitorReadADCEventHandler }, \
  { &emberAfPluginIasZoneServerManageQueueEventControl, emberAfPluginIasZoneServerManageQueueEventHandler }, \
  { &emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl, emberAfPluginManufacturingLibraryCliCheckSendCompleteEventHandler }, \
  { &emberAfPluginNetworkSteeringFinishSteeringEventControl, emberAfPluginNetworkSteeringFinishSteeringEventHandler }, \
  { &emberAfPluginReportingTickEventControl, emberAfPluginReportingTickEventHandler }, \
  { &emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl, emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventHandler }, \
  { &xxBlnkLed0Event, xxBlnkLed0EventHandle }, \
  { &xxInitEvent, xxInitEventHandler }, \
  { &xxIrqButtonHandleButton0PressedEvent, xxIrqButtonHandleButton0PressedEventHandler }, \
  { &xxIrqButtonHandleButton0ReleasedEvent, xxIrqButtonHandleButton0ReleasedEventHandler }, \
  { &xxIrqButtonHandleButton1PressedEvent, xxIrqButtonHandleButton1PressedEventHandler }, \
  { &xxIrqButtonHandleButton1ReleasedEvent, xxIrqButtonHandleButton1ReleasedEventHandler }, \
  { &xxIrqButtonHandleButton2PressedEvent, xxIrqButtonHandleButton2PressedEventHandler }, \
  { &xxIrqButtonHandleButton2ReleasedEvent, xxIrqButtonHandleButton2ReleasedEventHandler }, \
  { &xxIrqButtonHandleButtonTimeOut, xxIrqButtonHandleButtonTimeOutHandler }, \
  { &xx_project_button_interval_time_event, xxProjectButtonIntervalTimeFuction }, \
  { &xx_project_heart_beat_event, xxProjectHeartBeatEventHandler }, \
  { &xx_project_scan_network_event, xxProjectScanNetworkFuction }, \
  { &xx_project_wait_cfg_cmd_event, xxProjectWaitCfgCmdFuction }, \
  { &xxRebootEventControl, xxRebootEventHandler }, \
  { &xx_rejoin_control_event, xxRejoinControlEventHandler }, \
  { &emberAfPluginEndDeviceSupportMoveNetworkEventControls[0], emberAfPluginEndDeviceSupportMoveNetworkEventWrapper0 }, \
  { &emberAfPluginEndDeviceSupportPollingNetworkEventControls[0], emberAfPluginEndDeviceSupportPollingNetworkEventWrapper0 }, \
  { &emberAfPluginScanDispatchScanNetworkEventControls[0], emberAfPluginScanDispatchScanNetworkEventWrapper0 }, \


#define EMBER_AF_GENERATED_EVENT_STRINGS   \
  "IAS Zone Cluster Server EP 1",  \
  "Battery Monitor Plugin ReadADC",  \
  "IAS Zone Server Plugin ManageQueue",  \
  "Manufacturing Library CLI Plugin CheckSendComplete",  \
  "Network Steering Plugin FinishSteering",  \
  "Reporting Plugin Tick",  \
  "Update TC Link Key Plugin BeginTcLinkKeyUpdate",  \
  "Xx blnk led0 event",  \
  "Xx init event",  \
  "Xx irq button handle button0 pressed event",  \
  "Xx irq button handle button0 released event",  \
  "Xx irq button handle button1 pressed event",  \
  "Xx irq button handle button1 released event",  \
  "Xx irq button handle button2 pressed event",  \
  "Xx irq button handle button2 released event",  \
  "Xx irq button handle button time out",  \
  "Xx_project_button_interval_time_event",  \
  "Xx_project_heart_beat_event",  \
  "Xx_project_scan_network_event",  \
  "Xx_project_wait_cfg_cmd_event",  \
  "Xx reboot event control",  \
  "Xx_rejoin_control_event",  \
  "End Device Support Plugin Move NWK 0", \
  "End Device Support Plugin Polling NWK 0", \
  "Scan Dispatch Plugin Scan NWK 0", \


// The length of the event context table used to track and retrieve cluster events
#define EMBER_AF_EVENT_CONTEXT_LENGTH 1

// EmberAfEventContext structs used to populate the EmberAfEventContext table
#define EMBER_AF_GENERATED_EVENT_CONTEXT { 0x1, 0x500, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIasZoneClusterServerTickCallbackControl1}


#endif // __AF_GEN_EVENT__
