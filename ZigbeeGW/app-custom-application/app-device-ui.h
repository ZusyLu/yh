
#ifndef __APP_DEVICE_UI_H__
#define __APP_DEVICE_UI_H__

//----------------------------------------------------------
#define CUSTOM_RED_LED      0
#define CUSTOM_GREEN_LED    1
//-----------------LED PATTERN-----------------------------
#define STATE_RED_LED_STAEDY_ON_XS      0x01
#define STATE_RED_LED_BLINK_1S          0x02
#define STATE_GREEN_LED_BLINK_1S        0x03
#define STATE_RED_LED_ON_ONE_TIME       0x04
#define STATE_GREEN_LED_ON_ONE_TIME     0x05
#define STATE_RED_LED_QUICK_FLASH       0x06
#define STATE_RED_LED_IDENTIFY          0x07
#define STATE_RED_LED_PRD_TEST          0x08
#define STATE_RED_LED_INDICATION        0x09
#define STATE_RED_LED_ALARM             0x0A
//-------------------Action---------------------------------
#define  ACTION_NONE             0x00
#define  ACTION_JOINING_NETWORK  0x01
#define  ACTION_PRESS_BUTTON     0x02
#define  ACTION_HUB_RESPONSE     0x03
#define  ACTION_TEST_BUTTON      0X04
#define  ACTION_STEADY_ON_XS     0X05
#define  ACTION_IDENTIFY         0X06
#define  ACTION_LONG_PRESSED     0x07
#define  ACTION_PRD_TEST         0x08
#define  ACTION_ALARM            0X09
//---------------------time -------------------------------
#define  LED_BLINK_MS            100
//---------------------water sensor mode------------------
#define  LED_OFF_BUZZER_OFF_MODE  0X00
#define  LED_ON_BUZZER_OFF_MODE  0X01
#define  LED_OFF_BUZZER_ON_MODE  0X02
#define  LED_ON_BUZZER_ON_MODE  0X03
//
//
extern  void  halPowerOnReset(void);
extern  void  greenLedOnOff(bool on);
extern  void  redLedOnOff(bool on);
extern  void  customDeviceLedActionShow(uint8_t actionType,uint8_t steadyOnSeconds);
extern  void  customSensorLedActionUpdate(void);
extern uint8_t  getSensorLedActionType(void);
extern  void  ledAlarmStatusMatainningCallback(void);
extern bool sensorAlarmStateTrigged(void);

#endif //__APP_DEVICE_UI_H__
