#ifndef _SERIAL_RX_QUEUE_H_
#define _SERIAL_RX_QUEUE_H_

#ifdef OWN_SERIAL_RX_QUEUE
    #define  EXTERN_Q
#else
    #define  EXTERN_Q  extern
#endif

#define  RINGBUFF_LEN  2400

typedef struct
{
    uint16_t Head;
    uint16_t Tail;
    uint16_t Length;
    uint8_t Ring_Buff[RINGBUFF_LEN];
}RingBuff_t;

EXTERN_Q  void ringBufferInit(void);
EXTERN_Q  uint8_t writeRingBuffer(uint8_t data);
EXTERN_Q  uint8_t readRingBuffer(uint8_t *rData);
EXTERN_Q  uint8_t  pushBufferToRing(uint8_t *buffer,uint8_t length);
EXTERN_Q  uint8_t popRingBufferData(uint8_t *buffer,uint8_t length);
EXTERN_Q  uint8_t yhChangeChannel(uint8_t channel);

uint16_t getCRC(uint8_t* buf,int len);


#endif
