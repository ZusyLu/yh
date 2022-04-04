// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef SILABS_AF_ENDPOINT_CONFIG
#define SILABS_AF_ENDPOINT_CONFIG


// Fixed number of defined endpoints
#define FIXED_ENDPOINT_COUNT (1)


// Generated defaults
#if BIGENDIAN_CPU
#define GENERATED_DEFAULTS { \
14,'x','x','_','d','o','o','r','_','s','e','n','s','o','r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 /* 0,Default value: Basic,model identifier */, \
  }
#else // ! BIGENDIAN_CPU
#define GENERATED_DEFAULTS { \
14,'x','x','_','d','o','o','r','_','s','e','n','s','o','r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 /* 0,Default value: Basic,model identifier */, \
  }
#endif // BIGENDIAN_CPU




// Generated attributes
#define GENERATED_ATTRIBUTES { \
    { 0x0000, ZCL_INT8U_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)0x08 } }, /* 0 / Basic / ZCL version*/\
    { 0x0005, ZCL_CHAR_STRING_ATTRIBUTE_TYPE, 33, (ATTRIBUTE_MASK_TOKENIZE), { (uint8_t*)&(generatedDefaults[0]) } }, /* 1 / Basic / model identifier*/\
    { 0x0007, ZCL_ENUM8_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)0x00 } }, /* 2 / Basic / power source*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)3 } }, /* 3 / Basic / cluster revision*/\
    { 0x0020, ZCL_INT8U_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_TOKENIZE), { (uint8_t*)0x00UL } }, /* 4 / Power Configuration / battery voltage*/\
    { 0x0021, ZCL_INT8U_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_TOKENIZE), { (uint8_t*)0x00 } }, /* 5 / Power Configuration / battery percentage remaining*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_SINGLETON), { (uint8_t*)2 } }, /* 6 / Power Configuration / cluster revision*/\
    { 0x0000, ZCL_ENUM8_ATTRIBUTE_TYPE, 1, (0x00), { (uint8_t*)0x00 } }, /* 7 / IAS Zone / zone state*/\
    { 0x0001, ZCL_ENUM16_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_TOKENIZE), { (uint8_t*)0x0000 } }, /* 8 / IAS Zone / zone type*/\
    { 0x0002, ZCL_BITMAP16_ATTRIBUTE_TYPE, 2, (ATTRIBUTE_MASK_TOKENIZE), { (uint8_t*)0x0000 } }, /* 9 / IAS Zone / zone status*/\
    { 0x0010, ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE, 8, (ATTRIBUTE_MASK_WRITABLE|ATTRIBUTE_MASK_TOKENIZE), { NULL } }, /* 10 / IAS Zone / IAS CIE address*/\
    { 0x0011, ZCL_INT8U_ATTRIBUTE_TYPE, 1, (ATTRIBUTE_MASK_TOKENIZE), { (uint8_t*)0xFF } }, /* 11 / IAS Zone / Zone ID*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)2 } }, /* 12 / IAS Zone / cluster revision*/\
    { 0xFFFD, ZCL_INT16U_ATTRIBUTE_TYPE, 2, (0x00), { (uint8_t*)3 } }, /* 13 / Diagnostics / cluster revision*/\
  }


// Cluster function static arrays
#define GENERATED_FUNCTION_ARRAYS \
const EmberAfGenericClusterFunction emberAfFuncArrayPowerConfigClusterServer[] = { (EmberAfGenericClusterFunction)emberAfPowerConfigClusterServerPreAttributeChangedCallback}; \
const EmberAfGenericClusterFunction emberAfFuncArrayIasZoneClusterServer[] = { (EmberAfGenericClusterFunction)emberAfIasZoneClusterServerInitCallback,(EmberAfGenericClusterFunction)emberAfIasZoneClusterServerMessageSentCallback,(EmberAfGenericClusterFunction)emberAfIasZoneClusterServerPreAttributeChangedCallback}; \


// Clusters definitions
#define GENERATED_CLUSTERS { \
    { 0x0000, (EmberAfAttributeMetadata*)&(generatedAttributes[0]), 4, 33, (CLUSTER_MASK_SERVER), NULL,  },    \
    { 0x0001, (EmberAfAttributeMetadata*)&(generatedAttributes[4]), 3, 2, (CLUSTER_MASK_SERVER| CLUSTER_MASK_PRE_ATTRIBUTE_CHANGED_FUNCTION), emberAfFuncArrayPowerConfigClusterServer, },    \
    { 0x0500, (EmberAfAttributeMetadata*)&(generatedAttributes[7]), 6, 16, (CLUSTER_MASK_SERVER| CLUSTER_MASK_INIT_FUNCTION| CLUSTER_MASK_MESSAGE_SENT_FUNCTION| CLUSTER_MASK_PRE_ATTRIBUTE_CHANGED_FUNCTION), emberAfFuncArrayIasZoneClusterServer, },    \
    { 0x0B05, (EmberAfAttributeMetadata*)&(generatedAttributes[13]), 1, 2, (CLUSTER_MASK_SERVER), NULL,  },    \
  }


// Endpoint types
#define GENERATED_ENDPOINT_TYPES {        \
    { (EmberAfCluster*)&(generatedClusters[0]), 4, 53 }, \
  }


// Cluster manufacturer codes
#define GENERATED_CLUSTER_MANUFACTURER_CODES {      \
{0x00, 0x00} \
  }
#define GENERATED_CLUSTER_MANUFACTURER_CODE_COUNT (0)

// Attribute manufacturer codes
#define GENERATED_ATTRIBUTE_MANUFACTURER_CODES {      \
{0x00, 0x00} \
  }
#define GENERATED_ATTRIBUTE_MANUFACTURER_CODE_COUNT (0)


// Largest attribute size is needed for various buffers
#define ATTRIBUTE_LARGEST (33)
// Total size of singleton attributes
#define ATTRIBUTE_SINGLETONS_SIZE (6)

// Total size of attribute storage
#define ATTRIBUTE_MAX_SIZE 53

// Array of endpoints that are supported
#define FIXED_ENDPOINT_ARRAY { 1 }

// Array of profile ids
#define FIXED_PROFILE_IDS { 260 }

// Array of device ids
#define FIXED_DEVICE_IDS { 1026 }

// Array of device versions
#define FIXED_DEVICE_VERSIONS { 1 }

// Array of endpoint types supported on each endpoint
#define FIXED_ENDPOINT_TYPES { 0 }

// Array of networks supported on each endpoint
#define FIXED_NETWORKS { 0 }


#define EMBER_AF_GENERATED_PLUGIN_INIT_FUNCTION_DECLARATIONS \
  void emberAfPluginReportingInitCallback(void); \
  void emberAfPluginIdleSleepInitCallback(void); \
  void emberAfPluginCountersInitCallback(void); \
  void emberAfPluginBatteryMonitorInitCallback(void); \


#define EMBER_AF_GENERATED_PLUGIN_INIT_FUNCTION_CALLS \
  emberAfPluginReportingInitCallback(); \
  emberAfPluginIdleSleepInitCallback(); \
  emberAfPluginCountersInitCallback(); \
  emberAfPluginBatteryMonitorInitCallback(); \


#define EMBER_AF_GENERATED_PLUGIN_STACK_STATUS_FUNCTION_DECLARATIONS \
  void emberAfPluginEndDeviceSupportStackStatusCallback(EmberStatus status); \
  void emberAfPluginReportingStackStatusCallback(EmberStatus status); \
  void emberAfPluginNetworkSteeringStackStatusCallback(EmberStatus status); \
  void emberAfPluginIasZoneServerStackStatusCallback(EmberStatus status); \


#define EMBER_AF_GENERATED_PLUGIN_STACK_STATUS_FUNCTION_CALLS \
  emberAfPluginEndDeviceSupportStackStatusCallback(status); \
  emberAfPluginReportingStackStatusCallback(status); \
  emberAfPluginNetworkSteeringStackStatusCallback(status); \
  emberAfPluginIasZoneServerStackStatusCallback(status); \


// Generated data for the command discovery
#define GENERATED_COMMANDS { \
    { 0x0000, 0x00, COMMAND_MASK_INCOMING_SERVER }, /* Basic / ResetToFactoryDefaults */ \
    { 0x0500, 0x00, COMMAND_MASK_OUTGOING_SERVER }, /* IAS Zone / ZoneStatusChangeNotification */ \
    { 0x0500, 0x00, COMMAND_MASK_INCOMING_SERVER }, /* IAS Zone / ZoneEnrollResponse */ \
    { 0x0500, 0x01, COMMAND_MASK_OUTGOING_SERVER }, /* IAS Zone / ZoneEnrollRequest */ \
  }
#define EMBER_AF_GENERATED_COMMAND_COUNT (4)

// Command manufacturer codes
#define GENERATED_COMMAND_MANUFACTURER_CODES {      \
{0x00, 0x00} \
  }
#define GENERATED_COMMAND_MANUFACTURER_CODE_COUNT (0)


// Generated reporting configuration defaults
#define EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS {\
  { EMBER_ZCL_REPORTING_DIRECTION_REPORTED, 1, 0x0001, 0x0021, CLUSTER_MASK_SERVER, 0x0000, 1, 65534, 0 }, \
}
#define EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE (1)
#endif // SILABS_AF_ENDPOINT_CONFIG
