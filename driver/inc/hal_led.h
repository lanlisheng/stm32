#ifndef HAL_LED_H
#define HAL_LED_H

#include <stdint.h>

#define LED1_PORT GPIOA
#define LED1_PIN GPIO_Pin_1

#define BUZ_PORT GPIOB
#define BUZ_PIN GPIO_Pin_0

#define LED_EFFECT_END 0xFFFE
#define LED_EFFECT_AGN 0xFFFF

typedef enum {
  LED1, // 0
  BUZ,  // 1
  LED_SUM
} LED_TYPEDEF;

typedef enum {
  LED_DARK,
  LED_LIGHT,
  LED_LIGHT_100MS,
  LED_BLINK1,
  LED_BLINK2,
  LED_BLINK3,
  LED_BLINK4,
} LED_EFFECT_TYPEDEF;

void hal_LedInit(void);
void hal_LedProc(void);
void LedMsgInput(uint8_t type, LED_EFFECT_TYPEDEF cmd, uint8_t clr);
#endif
