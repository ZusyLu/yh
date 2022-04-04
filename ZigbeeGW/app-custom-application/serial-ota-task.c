#include <stdint.h>
#include <stdbool.h>
#include "serial-ota-task.h"
/*------------------------------------------------
 ��C�ļ��Ĺ��������ػ�ȡ�豸������OTA�ļ���״̬��
���̣��ֱ��п��У�����������֡���ȴ�֡���䣬����֡����
��״̬
__________________________________________________*/
#define OWN_SERIAL_OTA_TASK

/*typedef enum {
  OTA_STATE_IDLE = 0x01,
  OTA_STATE_START = 0x02,
  OTA_STATE_IMAGE_REQUEST = 0x03,
  OTA_STATE_IMAGE_WAIT_RSP = 0x04,
  OTA_STATE_FINISH = 0X05,
} SerialOtaStateEnum;*/

SerialOtaStateEnum serialOtaState = OTA_STATE_IDLE;
//ota define variable
uint8_t  validImageType[2] = {0x02,0x50}; //��Ч
uint8_t  imageType[2] = {0x02,0x50};
uint8_t  imageVersion[4] = {0x02,0x00,0x01,0x11};
uint8_t  imageBuffer[64];
uint32_t currentImageOffset;
uint32_t imageTotalSize;
uint32_t imagePackIndex;

void setSerialOtaState(SerialOtaStateEnum state)
{
    serialOtaState =  state;
}

uint8_t getSerialOtaState(void)
{
    return (serialOtaState);
}
void serialOtaTaskRun(void)
{

    switch(serialOtaState)
    {

        case OTA_STATE_IDLE:

             break;
        case OTA_STATE_START:
              serialSendImageRequest(0);
              serialOtaState = OTA_STATE_IMAGE_REQUEST;
              imagePackIndex = 0;
             
              break;
        case OTA_STATE_IMAGE_REQUEST:
               // serialSendImagePacketRequest(packIndex,64);
               // serialOtaState = OTA_STATE_IMAGE_WAIT_RSP;
              break;
        case  OTA_STATE_IMAGE_WAIT_RSP:

              break;
        case  OTA_STATE_FINISH:
              setSerialOtaState(OTA_STATE_IDLE);
              break;
        default:
              break;
    }
}

