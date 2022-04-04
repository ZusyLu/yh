#ifndef _RF_RX_UART_TX_H_

#define _RF_RX_UART_TX_H_

extern uint16_t  lastPopRfTime;

extern  void rfRingBufferInit(void);
extern  uint8_t rfWriteRingBuffer(uint8_t data);
extern  uint8_t rfReadRingBuffer(uint8_t *rData);
extern  uint8_t rfPushBufferToRing(uint8_t *buffer,uint8_t length);
extern  uint8_t rfPopRingBufferData(uint8_t *buffer,uint8_t length);
extern  void rfPopRingBufferOneFrame(void);

#endif