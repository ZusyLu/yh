/**************************************************************************************************
  Filename:       xx_example.c
  Revised:        $Date: 2021-3-27 18:29
  Revision:       $Revision: V1.0.0

  Description:    (no Profile). project base on EmberZNet SDK 6.10.1.0(2 stacks)

  Copyright 2022 lu.
                       
***************************************************************************************************/
#include "xxapp/xx_main.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _XX_IRQ_BUTTON_H
/***************************************************************************************************
*                                               NOTE
*                                               说明
***************************************************************************************************/
//-- source insight使用UTF-8解码，配合SimpliCity Studio开发环境，File->Reload As Encoding...->UTF-8
//-- source insight设置默认解码格式，Option->Preferences->Files->Default encoding->UTF-8

/***************************************************************************************************
*                                             INCLUDES
*                                            系统头文件
***************************************************************************************************/

/***************************************************************************************************
*                                               MACRO
*                                            本地宏定义
***************************************************************************************************/
#ifndef DEBOUNCE
#define DEBOUNCE                                            5
#endif//DEBOUNCE

#define XX_IRQ_BUTTON_INVALID_INDEX                         XX_IRQ_BSP_BUTTON_CUNT

/***************************************************************************************************
*                                          CONSTANT DEFINED
*                                             常量定义
***************************************************************************************************/
//这里说明，如果按键中断有问题，请查看中断号是否配置正确。
/***************************************************************************************************
*                                           STRUCT DEFINED
*                                             结构体定义
***************************************************************************************************/
typedef struct {
      GPIO_Port_TypeDef   port;
      unsigned int        pin;
      unsigned char       button;
} xxButton_t;

/***************************************************************************************************
*                                          GLOBAL VARIABLES
*                                             全局变量
***************************************************************************************************/

/***************************************************************************************************
*                                          LOCAL VARIABLES
*                                             局部变量
***************************************************************************************************/
static const xxButton_t xx_irq_button_rray[XX_IRQ_BSP_BUTTON_CUNT] = XX_IRQ_BSP_BUTTON_INIT;
#if (XX_IRQ_BUTTON_COUNT > 0)
static uint8_t xx_irq_button_state[XX_IRQ_BSP_BUTTON_CUNT];
#endif


/***************************************************************************************************
*                                          EXTERN FUNCTIONS
*                                             外部函数
***************************************************************************************************/

/***************************************************************************************************
*                                            FUNCTIONS
*                                               函数
***************************************************************************************************/

#if (XX_IRQ_BUTTON_COUNT > 0)
static uint8_t xxIrqButtonIndex( uint8_t button )
{
    uint8_t i;
    for ( i = 0; i < XX_IRQ_BSP_BUTTON_CUNT; i++ ) {
        if ( xx_irq_button_rray[i].button == button ) {
            break;
        }
    }
    return i;
}
#endif

void xxIrqButtonStateSet(uint8_t button, uint8_t state)
{
  // Note: this sets the "soft" state
  //  so it jives with the interrupts and their callbacks
#if (XX_IRQ_BUTTON_COUNT > 0)
  uint8_t index = xxIrqButtonIndex(button);
  if ( index != XX_IRQ_BUTTON_INVALID_INDEX ) {
    xx_irq_button_state[index] = state;
  }
#endif
}

void xxIrqButtonToggleState(uint8_t button)
{
  // Note: this toggles the "soft" state
  //  so it jives with the interrupts and their callbacks
#if (HAL_BUTTON_COUNT > 0)
  uint8_t index = xxIrqButtonIndex(button);
  if ( index != XX_IRQ_BUTTON_INVALID_INDEX ) {
    xx_irq_button_state[index] = !xx_irq_button_state[index];
  }
#endif
}


uint8_t xxIrqButtonPinState( uint8_t button )
{
#if (XX_IRQ_BUTTON_COUNT > 0)
  uint8_t index = xxIrqButtonIndex(button);
  if ( index != XX_IRQ_BUTTON_INVALID_INDEX ) {
    return (GPIO_PinInGet(xx_irq_button_rray[index].port, xx_irq_button_rray[index].pin)) ? XX_IRQ_BUTTON_RELEASED : XX_IRQ_BUTTON_PRESSED;
  }
#endif
  return XX_IRQ_BUTTON_RELEASED;
}

#if (XX_IRQ_BUTTON_COUNT > 0)

void xxIrqButtonIsr(uint8_t button, uint8_t state)
{
    xxProjectDisableJTAGPort();
	#ifdef XX_POWER_CONFIGURATION_READ_AD
		XX_POWER_CONFIGURATION_READ_AD;
	#endif
    xxIrqButtonHandleFunction(button, state);
}

extern uint8_t xx_irq_button_handle_button;
extern EmberEventControl xxTestEventControl;
void xxIrqButtonInternalIsr(uint8_t button)
{
  uint8_t buttonStateNow;
  #if (DEBOUNCE > 0)
  uint8_t buttonStatePrev;
  uint32_t debounce;
  #endif //(DEBOUNCE > 0)

  buttonStateNow = xxIrqButtonPinState(button);
  #if (DEBOUNCE > 0)
  //read button until get "DEBOUNCE" number of consistent readings
  for ( debounce = 0;
        debounce < DEBOUNCE;
        debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
    buttonStatePrev = buttonStateNow;
    buttonStateNow = xxIrqButtonPinState(button);
  }
  #endif //(DEBOUNCE > 0)

  if (xxIrqButtonPinState(button) != buttonStateNow) { //state changed, notify app
    xxIrqButtonStateSet(button, buttonStateNow);
    xxIrqButtonIsr(button, xxIrqButtonPinState(button));
  } else {  //state unchanged, then notify app of a double-transition
    xxIrqButtonToggleState(button); //invert temporarily for calling Isr
    xxIrqButtonIsr(button, xxIrqButtonPinState(button));
    xxIrqButtonToggleState(button); //and put it back to current state
    xxIrqButtonIsr(button, xxIrqButtonPinState(button));
  }
}

#endif

extern EmberEventControl xxTestEventControl;
extern void xxIrqButtonHandleFunction(uint8_t button, uint8_t state);

void xxIrqButtonInit( void )
{
#if (XX_IRQ_BUTTON_COUNT > 0)
    uint8_t enableButtons[XX_IRQ_BUTTON_COUNT] = XX_IRQ_BUTTON_ENABLE;
      /* Initialize GPIO interrupt dispatcher */
    GPIOINT_Init();
        /* Enable GPIO in CMU */
#if !defined(_SILICON_LABS_32B_SERIES_2)
    CMU_ClockEnable(cmuClock_HFPER, true);
#endif
    CMU_ClockEnable(cmuClock_GPIO, true);
    uint8_t i;
    for ( i = 0; i < XX_IRQ_BUTTON_COUNT; i++ ) 
    {
        /* Configure pin as input */
        GPIO_PinModeSet(xx_irq_button_rray[i].port,
                        xx_irq_button_rray[i].pin,
                        XX_IRQ_BSP_BUTTON_GPIO_MODE,
                        XX_IRQ_BSP_BUTTON_GPIO_DOUT);
        /* Register callbacks before setting up and enabling pin interrupt. */

        GPIOINT_CallbackRegister(xx_irq_button_rray[enableButtons[i]].button,
                                 xxIrqButtonInternalIsr);
        /* Set rising and falling edge interrupts */
        GPIO_ExtIntConfig(xx_irq_button_rray[enableButtons[i]].port,
                          xx_irq_button_rray[enableButtons[i]].pin,
                          xx_irq_button_rray[enableButtons[i]].button,
                          true,
                          true,
                          true);
}
#endif

}


/***************************************************************************************************
*                                                END
***************************************************************************************************/
#ifdef __cplusplus
}
#endif
#endif


