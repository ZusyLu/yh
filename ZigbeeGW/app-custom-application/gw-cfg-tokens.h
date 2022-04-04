/*******************************************************************************
*CUSTOM TOKEN DEFINE FOR PRODUCT TEST
 ******************************************************************************/
// Creator for rf test flag token
#if 0        //cut by ll
#define CREATOR_MODULE_CFG_INFO  0x0000
#define CREATOR_MODULE_USER_DATA 0x0002
#define CREATOR_MODULE_OTA_INFO 0x0003
#define CREATOR_MODULE_OTA_DOWNLOAD_INFO 0x0004
#endif

//add for EFR32MG21
#define CREATOR_TEST_RESET                      0x00dd
#define CREATOR_MODULE_CFG_INFO                 0x000f
#define CREATOR_MODULE_USER_DATA                0x003f
#define CREATOR_MODULE_OTA_INFO                 0x006f
#define CREATOR_MODULE_OTA_DOWNLOAD_INFO        0x009f

#define NVM3KEY_TEST_RESET                    ( NVM3KEY_DOMAIN_USER | CREATOR_TEST_RESET )
#define NVM3KEY_MODULE_CFG_INFO               ( NVM3KEY_DOMAIN_USER | CREATOR_MODULE_CFG_INFO )
#define NVM3KEY_MODULE_USER_DATA              ( NVM3KEY_DOMAIN_USER | CREATOR_MODULE_USER_DATA )
#define NVM3KEY_MODULE_OTA_INFO               ( NVM3KEY_DOMAIN_USER | CREATOR_MODULE_OTA_INFO )
#define NVM3KEY_MODULE_OTA_DOWNLOAD_INFO      ( NVM3KEY_DOMAIN_USER | CREATOR_MODULE_OTA_DOWNLOAD_INFO )

// Types for the tokens
#ifdef DEFINETYPES
typedef union {
  uint32_t testReset;
  uint8_t testResetBuff[4];
}  tokType_test_reset;
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

typedef struct{
  int16u manuId;
  int16u imageType;
  int32u  version;
  int8u isComplete;
}tokTypeModuleOtaInfoStruct;


//typedef struct{
//  int32u totalPackNum;
//  int8u  isComplete;
//  int8u  statusPerPack[400];
  
//}tokTypeModuleOtaDownLoadInfoStruct;
#endif // DEFINETYPES
// Actual token definitions

#ifdef DEFINETOKENS

DEFINE_COUNTER_TOKEN(TEST_RESET,tokType_test_reset,0x00000000)

DEFINE_BASIC_TOKEN(MODULE_CFG_INFO, tokTypeModuleCfgStruct,
                   {{0x01},{0x13},{0x14},{0x0000},{0x6688},
                   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
                   {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
                   0x39,0x31,0x32,0x33,0x34,0x35,0x36}})

DEFINE_BASIC_TOKEN(MODULE_USER_DATA, tokTypeModuleUserDataStruct,
                   {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff})

DEFINE_BASIC_TOKEN(MODULE_OTA_INFO, tokTypeModuleOtaInfoStruct,
                   {{0x1002},{0x5002},{0x11010002},{1}})

//DEFINE_BASIC_TOKEN(MODULE_OTA_DOWNLOAD_INFO, tokTypeModuleOtaDownLoadInfoStruct,
 //                  {{0x00},{0x00},{0x00}})
#endif // DEFINETOKENS






