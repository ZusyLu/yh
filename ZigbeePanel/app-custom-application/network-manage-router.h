#ifndef __NETWORK_MANAGE_ROUTER_H__
#define __NETWORK_MANAGE_ROUTER_H__

extern uint16_t joinNetworkButtonPressedTimer;
extern bool joinNetworkButtonRelesed;
extern bool joinNetworkButtonLongPressed;
extern bool joiningNetworkRuuning;
extern EmberEventControl appNetworkJoinEventControl;
extern EmberEventControl appMatchOtaServerEventControl;

void rfTestForceNetworkScanStop(void);
void resetToFactoryState(void);
void resetToFactoryStateAndReboot(void);
void setupDefaultReportTableEntry(void);

#endif