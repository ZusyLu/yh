
//1
#define CREATOR_XX_RESET                                                         0x0001
#define CREATOR_XX_SCENE                                                         0x0002

//2
#define NVM3KEY_XX_SCENE                                                         ( NVM3KEY_DOMAIN_USER | CREATOR_XX_SCENE )

#define XX_KEY_FOR_SCENE_NUMBER                                                  6
#define XX_DEST_MAC_ID                                                           8
//3
#ifdef DEFINETYPES

typedef struct{
    uint8_t  triggerIndex;  //0Xff is off ,
    uint8_t  scenesid;
    uint16_t groupid;
    uint8_t  triggerMode;
    uint8_t  destEnpiont;
    uint8_t  zclCmd;
    uint16_t  clusterId;
    uint16_t destShortId;
    uint8_t  destMacId[XX_DEST_MAC_ID];
} tokType_xx_scene_t;

#endif //DEFINETYPES

//

#ifdef DEFINETOKENS


//DEFINE_INDEXED_TOKEN( XX_SCENE, tokType_xx_scene_t, sizeof(tokType_xx_scene_t), {0XFF})

DEFINE_INDEXED_TOKEN( XX_SCENE, tokType_xx_scene_t, sizeof(tokType_xx_scene_t), {0XFF, 0XFF, 0XFFFF, 0XFF, 0XFF, 0XFF,  0XFFFF, 0XFFFF, 0XFF, 0XFF, 0XFF, 0XFF,0XFF, 0XFF, 0XFF, 0XFF } )

#endif //DEFINETOKENS


