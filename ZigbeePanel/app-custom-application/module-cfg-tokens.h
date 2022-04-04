/*******************************************************************************
*CUSTOM TOKEN DEFINE FOR PRODUCT TEST
 ******************************************************************************/
// Creator for rf test flag token
#define CREATOR_MODULE_CFG_INFO  0x0000
#define CREATOR_MODULE_USER_DATA 0x0022

//add by lu
#define NVM3KEY_MODULE_CFG_INFO                    ( NVM3KEY_DOMAIN_USER | CREATOR_MODULE_CFG_INFO )
#define NVM3KEY_MODULE_USER_DATA                    ( NVM3KEY_DOMAIN_USER | CREATOR_MODULE_USER_DATA )

// Types for the tokens
#ifdef DEFINETYPES
typedef struct{
 int8u  deviceType;
 int8u  channel;
 int8u  radioTxPower;
 int16u shortAddress;
 int16u panId;
 int8u  extendPanId[8];
 int8u  nwkKey[16];
}tokTypeModuleCfgStruct;
typedef struct{
 int8u  usData[16];
}tokTypeModuleUserDataStruct;
#endif // DEFINETYPES
// Actual token definitions
#ifdef DEFINETOKENS
#if 0
DEFINE_BASIC_TOKEN(MODULE_CFG_INFO, tokTypeModuleCfgStruct,
                   {{0x02},{0x13},{0x14},{0xffff},{0x0064},
                   {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08},
                   {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                   0x09,0x0A,0x0b,0x0c,0x0d,0x0e,0x0f}})
#endif

DEFINE_BASIC_TOKEN(MODULE_CFG_INFO, tokTypeModuleCfgStruct,
                   {{0x02},{0x13},{0x14},{0xFFFF},{0xFFFF},
                   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
                   {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
                   0x39,0x31,0x32,0x33,0x34,0x35,0x36}})

DEFINE_BASIC_TOKEN(MODULE_USER_DATA, tokTypeModuleUserDataStruct,
                   {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff})
#endif // DEFINETOKENS






