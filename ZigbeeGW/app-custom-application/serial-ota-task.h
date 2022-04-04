#ifndef _SERIAL_OTA_TASK_H_
#define _SERIAL_OTA_TASK_H_

#ifdef OWN_SERIAL_OTA_TASK
	#define EXTERN
#else
	#define EXTERN extern
#endif

typedef enum {
  OTA_STATE_IDLE = 0x01,
  OTA_STATE_START = 0x02,
  OTA_STATE_IMAGE_REQUEST = 0x03,
  OTA_STATE_IMAGE_WAIT_RSP = 0x04,
  OTA_STATE_FINISH = 0X05,
} SerialOtaStateEnum;

extern uint8_t  validImageType[2];
extern uint8_t  imageType[2];
extern uint8_t  imageVersion[4];
extern uint8_t  imageBuffer[64];
extern uint32_t currentImageOffset;
extern uint32_t imageTotalSize;

EXTERN void setSerialOtaState(SerialOtaStateEnum state);
EXTERN void serialOtaTaskRun(void);


#endif