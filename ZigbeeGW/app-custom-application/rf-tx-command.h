#ifndef _RF_TX_COMMAND_H_
#define _RF_TX_COMMAND_H_


extern  void rfSendCommandQueryControlPointValue(void);
extern  void rfSendCommandCtrlControlPoint(void);
extern void rfSendBroadcastCommandCtrlControlPoint(void);
extern  void rfSendCommandUpdatePannel(void);
extern  void rfSendCommandUpdatePannelScene(void);
extern  void rfSendCommandDirectTx(void);
extern  EmberStatus rfSendCommandDirectTxTestCommand(uint16_t dstAddr);
extern  void rfNotifyDeviceOTA(void);

#endif
