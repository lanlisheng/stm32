/**
 ******************************************************************************
 * @file           : hal_key.c
 * @author         : lanlisheng
 * @brief          : None
 * @attention      : None
 * @date           : 2023/11/29
 ******************************************************************************
 */
#include "hal_key.h"
#include "stm32F10x.h"
#include <stdint.h>

KeyEvent_CallBack_t KeyScanCBS;

static uint8_t hal_getKey1Sta(void);
static uint8_t hal_getKey2Sta(void);
static uint8_t hal_getKey3Sta(void);
static uint8_t hal_getKey4Sta(void);
static uint8_t hal_getKey5Sta(void);
static uint8_t hal_getKey6Sta(void);

static void hal_keyConfig(void);
uint8_t (*getKeysState[KEY_NUM])() = {hal_getKey1Sta, hal_getKey2Sta,
                                      hal_getKey3Sta, hal_getKey4Sta,
                                      hal_getKey5Sta, hal_getKey6Sta};
uint8_t KeyStep[KEY_NUM];
uint16_t KeyScanTime[KEY_NUM];
uint16_t KeyPressLongTimer[KEY_NUM];
uint16_t KeyContPressTimer[KEY_NUM];

void hal_KeyInit(void) {
  hal_keyConfig();
  KeyScanCBS = 0;
  for (int i = 0; i < KEY_NUM; ++i) {
    KeyStep[i] = KEY_STEP_WAIT;
    KeyScanTime[i] = KEY_SCAN_TIME;
    KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
    KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
  }
}

void hal_KeyScanCBSRegister(KeyEvent_CallBack_t pCBS) {
  if (KeyScanCBS == 0) {
    KeyScanCBS = pCBS;
  }
}

void hal_KeyProc(void) {
  uint8_t keys = 0;
  uint8_t KeyState[KEY_NUM];
  for (int i = 0; i < KEY_NUM; ++i) {
    keys = 0;
    KeyState[i] = getKeysState[i]();
    switch (KeyStep[i]) {
    case KEY_STEP_WAIT: {
      if (KeyState[i]) {
        KeyStep[i] = KEY_STEP_CLICK;
      }
    } break;
    case KEY_STEP_CLICK: {
      if (KeyState[i]) {
        if (!(--KeyScanTime[i])) {
          KeyScanTime[i] = KEY_SCAN_TIME;
          KeyStep[i] = KEY_STEP_LONG_PRESS;
          keys = (i * 5) + 1;
        }
      } else {
        KeyScanTime[i] = KEY_SCAN_TIME;
        KeyStep[i] = KEY_STEP_WAIT;
      }
    } break;
    case KEY_STEP_LONG_PRESS: {
      if (KeyState[i]) {
        if (!(--KeyPressLongTimer[i])) {
          KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
          KeyStep[i] = KEY_STEP_CONTINUOUS_PRESS;
          keys = (i * 5) + 3;
        }
      } else {
        KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
        KeyStep[i] = KEY_STEP_WAIT;
        keys = (i * 5) + 2;
      }
    } break;
    case KEY_STEP_CONTINUOUS_PRESS: {
      if (KeyState[i]) {
        if (!(--KeyContPressTimer[i])) {
          KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
          keys = (i * 5) + 4;
        }
      } else {
        KeyStep[i] = KEY_STEP_WAIT;
        KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
        keys = (i * 5) + 5;
      }
    } break;
    default:
      break;
    }
    if (keys) {
      if (KeyScanCBS) {
        KeyScanCBS((KEY_VALUE_TYPEDEF)keys);
      }
    }
  }
}

static void hal_keyConfig(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

  GPIO_InitStructure.GPIO_Pin = K1_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(K1_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = K2_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(K2_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = K3_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(K3_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = K4_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(K4_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = K5_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(K5_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = K6_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(K6_PORT, &GPIO_InitStructure);
}

static uint8_t hal_getKey1Sta(void) {
  return (!GPIO_ReadInputDataBit(K1_PORT, K1_PIN));
}
static uint8_t hal_getKey2Sta(void) {
  return (!GPIO_ReadInputDataBit(K2_PORT, K2_PIN));
}
static uint8_t hal_getKey3Sta(void) {
  return (!GPIO_ReadInputDataBit(K3_PORT, K3_PIN));
}
static uint8_t hal_getKey4Sta(void) {
  return (!GPIO_ReadInputDataBit(K4_PORT, K4_PIN));
}
static uint8_t hal_getKey5Sta(void) {
  return (!GPIO_ReadInputDataBit(K5_PORT, K5_PIN));
}
static uint8_t hal_getKey6Sta(void) {
  return (!GPIO_ReadInputDataBit(K6_PORT, K6_PIN));
}