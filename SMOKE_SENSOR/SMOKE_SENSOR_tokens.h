// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// This file contains the tokens for attributes stored in flash


// Identifier tags for tokens
// Creator for attribute: model identifier, endpoint: 1
#define CREATOR_MODEL_IDENTIFIER_1 0xB000
#define NVM3KEY_MODEL_IDENTIFIER_1 ( NVM3KEY_DOMAIN_ZIGBEE | 0xB000 )
// Creator for attribute: battery voltage, endpoint: 1
#define CREATOR_BATTERY_VOLTAGE_1 0xB001
#define NVM3KEY_BATTERY_VOLTAGE_1 ( NVM3KEY_DOMAIN_ZIGBEE | 0xB001 )
// Creator for attribute: battery percentage remaining, endpoint: 1
#define CREATOR_BATTERY_PERCENTAGE_REMAINING_1 0xB002
#define NVM3KEY_BATTERY_PERCENTAGE_REMAINING_1 ( NVM3KEY_DOMAIN_ZIGBEE | 0xB002 )
// Creator for attribute: zone type, endpoint: 1
#define CREATOR_ZONE_TYPE_1 0xB003
#define NVM3KEY_ZONE_TYPE_1 ( NVM3KEY_DOMAIN_ZIGBEE | 0xB003 )
// Creator for attribute: zone status, endpoint: 1
#define CREATOR_ZONE_STATUS_1 0xB004
#define NVM3KEY_ZONE_STATUS_1 ( NVM3KEY_DOMAIN_ZIGBEE | 0xB004 )
// Creator for attribute: IAS CIE address, endpoint: 1
#define CREATOR_IAS_CIE_ADDRESS_1 0xB005
#define NVM3KEY_IAS_CIE_ADDRESS_1 ( NVM3KEY_DOMAIN_ZIGBEE | 0xB005 )
// Creator for attribute: Zone ID, endpoint: 1
#define CREATOR_ZONE_ID_1 0xB006
#define NVM3KEY_ZONE_ID_1 ( NVM3KEY_DOMAIN_ZIGBEE | 0xB006 )


// Types for the tokens
#ifdef DEFINETYPES
typedef uint8_t  tokType_model_identifier[33];
typedef uint16_t  tokType_zone_status;
typedef uint8_t  tokType_ias_cie_address[8];
typedef uint8_t  tokType_zone_id;
typedef uint16_t  tokType_zone_type;
typedef uint8_t  tokType_battery_voltage;
typedef uint8_t  tokType_battery_percentage_remaining;
#endif // DEFINETYPES


// Actual token definitions
#ifdef DEFINETOKENS
DEFINE_BASIC_TOKEN(MODEL_IDENTIFIER_1, tokType_model_identifier, {14,'x','x','_','d','o','o','r','_','s','e','n','s','o','r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0})
DEFINE_BASIC_TOKEN(BATTERY_VOLTAGE_1, tokType_battery_voltage, 0x00UL)
DEFINE_BASIC_TOKEN(BATTERY_PERCENTAGE_REMAINING_1, tokType_battery_percentage_remaining, 0x00)
DEFINE_BASIC_TOKEN(ZONE_TYPE_1, tokType_zone_type, 0x0000)
DEFINE_BASIC_TOKEN(ZONE_STATUS_1, tokType_zone_status, 0x0000)
DEFINE_BASIC_TOKEN(IAS_CIE_ADDRESS_1, tokType_ias_cie_address, {0,0,0,0,0,0,0,0})
DEFINE_BASIC_TOKEN(ZONE_ID_1, tokType_zone_id, 0xFF)
#endif // DEFINETOKENS


// Macro snippet that loads all the attributes from tokens
#define GENERATED_TOKEN_LOADER(endpoint) do {\
  uint8_t ptr[33]; \
  uint8_t curNetwork = emberGetCurrentNetwork(); \
  uint8_t epNetwork; \
  epNetwork = emberAfNetworkIndexFromEndpoint(1); \
  if((endpoint) == 1 || ((endpoint) == EMBER_BROADCAST_ENDPOINT && epNetwork == curNetwork)) { \
    halCommonGetToken((tokType_model_identifier *)ptr, TOKEN_MODEL_IDENTIFIER_1); \
    emberAfWriteServerAttribute(1, ZCL_BASIC_CLUSTER_ID, ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_CHAR_STRING_ATTRIBUTE_TYPE); \
    halCommonGetToken((tokType_battery_voltage *)ptr, TOKEN_BATTERY_VOLTAGE_1); \
    emberAfWriteServerAttribute(1, ZCL_POWER_CONFIG_CLUSTER_ID, ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE); \
    halCommonGetToken((tokType_battery_percentage_remaining *)ptr, TOKEN_BATTERY_PERCENTAGE_REMAINING_1); \
    emberAfWriteServerAttribute(1, ZCL_POWER_CONFIG_CLUSTER_ID, ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE); \
    halCommonGetToken((tokType_zone_type *)ptr, TOKEN_ZONE_TYPE_1); \
    emberAfWriteServerAttribute(1, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_TYPE_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_ENUM16_ATTRIBUTE_TYPE); \
    halCommonGetToken((tokType_zone_status *)ptr, TOKEN_ZONE_STATUS_1); \
    emberAfWriteServerAttribute(1, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_STATUS_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_BITMAP16_ATTRIBUTE_TYPE); \
    halCommonGetToken((tokType_ias_cie_address *)ptr, TOKEN_IAS_CIE_ADDRESS_1); \
    emberAfWriteServerAttribute(1, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_IAS_CIE_ADDRESS_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE); \
    halCommonGetToken((tokType_zone_id *)ptr, TOKEN_ZONE_ID_1); \
    emberAfWriteServerAttribute(1, ZCL_IAS_ZONE_CLUSTER_ID, ZCL_ZONE_ID_ATTRIBUTE_ID, (uint8_t*)ptr, ZCL_INT8U_ATTRIBUTE_TYPE); \
  } \
} while(false)


// Macro snippet that saves the attribute to token
#define GENERATED_TOKEN_SAVER do {\
  uint8_t allZeroData[33]; \
  MEMSET(allZeroData, 0, 33); \
  if ( data == NULL ) data = allZeroData; \
  if ( endpoint == 1 ) { \
    if ( clusterId == 0x00 ) { \
      if ( metadata->attributeId == 0x0005 && 0x0000 == emberAfGetMfgCode(metadata) &&!emberAfAttributeIsClient(metadata) ) \
        halCommonSetToken(TOKEN_MODEL_IDENTIFIER_1, data); \
    } else if ( clusterId == 0x01 ) { \
      if ( metadata->attributeId == 0x0020 && 0x0000 == emberAfGetMfgCode(metadata) &&!emberAfAttributeIsClient(metadata) ) \
        halCommonSetToken(TOKEN_BATTERY_VOLTAGE_1, data); \
      if ( metadata->attributeId == 0x0021 && 0x0000 == emberAfGetMfgCode(metadata) &&!emberAfAttributeIsClient(metadata) ) \
        halCommonSetToken(TOKEN_BATTERY_PERCENTAGE_REMAINING_1, data); \
    } else if ( clusterId == 0x0500 ) { \
      if ( metadata->attributeId == 0x0001 && 0x0000 == emberAfGetMfgCode(metadata) &&!emberAfAttributeIsClient(metadata) ) \
        halCommonSetToken(TOKEN_ZONE_TYPE_1, data); \
      if ( metadata->attributeId == 0x0002 && 0x0000 == emberAfGetMfgCode(metadata) &&!emberAfAttributeIsClient(metadata) ) \
        halCommonSetToken(TOKEN_ZONE_STATUS_1, data); \
      if ( metadata->attributeId == 0x0010 && 0x0000 == emberAfGetMfgCode(metadata) &&!emberAfAttributeIsClient(metadata) ) \
        halCommonSetToken(TOKEN_IAS_CIE_ADDRESS_1, data); \
      if ( metadata->attributeId == 0x0011 && 0x0000 == emberAfGetMfgCode(metadata) &&!emberAfAttributeIsClient(metadata) ) \
        halCommonSetToken(TOKEN_ZONE_ID_1, data); \
    } \
  } \
} while(false)


