#include "hal_led.h"
#include "hal_timer.h"
#include "os_system.h"
#include "stm32f10x.h"

static void hal_ledConfig(void);
static void LedBlink_Handle(void);

void hal_LedInit(void) {
  hal_ledConfig();
  hal_CreatTimer(T_LED, LedBlink_Handle, 5000, T_STA_START);
}

static void hal_ledConfig(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_1);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void hal_LedProc(void) {
  static uint16_t i = 0;
  i++;
  if (i > 5) {
    i = 0;
    //    hal_LedTurn();
  }
}

void hal_Led1Drive(unsigned char sta) {
  if (sta) {
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
  } else {
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
  }
}

void hal_BuzDrive(unsigned char sta) {
  if (sta) {
    GPIO_SetBits(BUZ_PORT, BUZ_PIN);
  } else {
    GPIO_ResetBits(BUZ_PORT, BUZ_PIN);
  }
}

void hal_LedTurn(void) {
  GPIO_WriteBit(GPIOA, GPIO_Pin_1,
                (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1)));
}

static void LedBlink_Handle(void) {
  hal_LedTurn();
  hal_ResetTimer(T_LED, T_STA_START);
}
