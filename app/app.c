#include "app.h"
#include "hal_key.h"
#include "hal_led.h"
#include "hal_oled.h"

static void KeyEventHandle(KEY_VALUE_TYPEDEF keys);

void AppInit(void) {
  hal_KeyScanCBSRegister(KeyEventHandle);
  hal_OledInit();
  hal_Oled_ShowString(16, 20, "Smart alarm", 16, 1);
  hal_Oled_ShowString(40, 40, "system", 16, 0);
  hal_Oled_Refresh();
  LedMsgInput(LED1, LED_LIGHT, 1);
}

void AppProc(void) {}

//-----------------驱动层回调处理函数------------------------

// 按键回调函数
static void KeyEventHandle(KEY_VALUE_TYPEDEF keys) {

  if ((keys == KEY1_CLICK) || (keys == KEY2_CLICK) || (keys == KEY3_CLICK) ||
      (keys == KEY4_CLICK) || (keys == KEY5_CLICK) || (keys == KEY6_CLICK)) {
    LedMsgInput(LED1, LED_LIGHT, 1);
  } else if ((keys == KEY1_CLICK_RELEASE) || (keys == KEY2_CLICK_RELEASE) ||
             (keys == KEY3_CLICK_RELEASE) || (keys == KEY4_CLICK_RELEASE) ||
             (keys == KEY5_CLICK_RELEASE) || (keys == KEY6_CLICK_RELEASE)) {
    LedMsgInput(LED1, LED_BLINK4, 1);
  } else if ((keys == KEY1_LONG_PRESS) || (keys == KEY2_LONG_PRESS) ||
             (keys == KEY3_LONG_PRESS) || (keys == KEY4_LONG_PRESS) ||
             (keys == KEY5_LONG_PRESS) || (keys == KEY6_LONG_PRESS)) {
    LedMsgInput(LED1, LED_DARK, 1);
  }
}
