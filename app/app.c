#include "app.h"
#include "OS_System.h"
#include "hal_beep.h"
#include "hal_eeprom.h"
#include "hal_key.h"
#include "hal_led.h"
#include "hal_oled.h"
#include "hal_rfd.h"
#include "stdint.h"

static void KeyEventHandle(KEY_VALUE_TYPEDEF keys);
static void RfdRcvHandle(uint8_t *pBuff);
static void menuInit(void);
static void gnlMenu_DesktopCBS(void);

Queue8 RFDRcvMsg; // RFD接收队列

stu_mode_menu *pModeMenu;      // 系统当前菜单
stu_system_time stuSystemtime; // 系统时间

// 初始化桌面菜单
stu_mode_menu generalModeMenu[GNL_MENU_SUM] = {
    {GNL_MENU_DESKTOP, DESKTOP_MENU_POS, "Desktop", gnlMenu_DesktopCBS,
     SCREEN_CMD_RESET, 0, 0xFF, 0, 0, 0, 0},
};

void AppInit(void) {
  hal_OledInit();
  hal_BeepInit();
  hal_eepromInit();
  // ParaInit();
  menuInit();
  QueueEmpty(RFDRcvMsg);
  hal_KeyScanCBSRegister(KeyEventHandle);
  hal_RFCRcvCBSRegister(RfdRcvHandle);

  stuSystemtime.year = 2024;
  stuSystemtime.mon = 3;
  stuSystemtime.day = 25;
  stuSystemtime.hour = 22;
  stuSystemtime.min = 50;
  stuSystemtime.week = 1;
}

void AppProc(void) { pModeMenu->action(); }

void showSystemTime(void) {
  // 2024-03-25 22:50 Mon
  hal_Oled_ShowChar(4, 54, (stuSystemtime.year / 1000) + '0', 8, 1);
  hal_Oled_ShowChar(10, 54, ((stuSystemtime.year % 1000) / 100) + '0', 8, 1);
  hal_Oled_ShowChar(16, 54, ((stuSystemtime.year % 1000 % 100) / 10) + '0', 8,
                    1);
  hal_Oled_ShowChar(22, 54, (stuSystemtime.year % 1000 % 100 % 10) + '0', 8, 1);

  hal_Oled_ShowString(28, 54, "-", 8, 1);

  hal_Oled_ShowChar(34, 54, (stuSystemtime.mon / 10) + '0', 8, 1);
  hal_Oled_ShowChar(40, 54, (stuSystemtime.mon % 10) + '0', 8, 1);

  hal_Oled_ShowString(46, 54, "-", 8, 1);

  hal_Oled_ShowChar(52, 54, (stuSystemtime.day / 10) + '0', 8, 1);
  hal_Oled_ShowChar(58, 54, (stuSystemtime.day % 10) + '0', 8, 1);

  hal_Oled_ShowChar(70, 54, (stuSystemtime.hour / 10) + '0', 8, 1);
  hal_Oled_ShowChar(76, 54, (stuSystemtime.hour % 10) + '0', 8, 1);

  hal_Oled_ShowString(82, 54, ":", 8, 1);

  hal_Oled_ShowChar(88, 54, (stuSystemtime.min / 10) + '0', 8, 1);
  hal_Oled_ShowChar(94, 54, (stuSystemtime.min % 10) + '0', 8, 1);

  hal_Oled_ShowString(100, 54, " ", 8, 1);

  switch (stuSystemtime.week) {
  case 1:
    hal_Oled_ShowString(106, 54, "Mon", 8, 1);
    break;
  case 2:
    hal_Oled_ShowString(106, 54, "Tue", 8, 1);
    break;
  case 3:
    hal_Oled_ShowString(106, 54, "Wed", 8, 1);
    break;
  case 4:
    hal_Oled_ShowString(106, 54, "Thu", 8, 1);
    break;
  case 5:
    hal_Oled_ShowString(106, 54, "Fir", 8, 1);
    break;
  case 6:
    hal_Oled_ShowString(106, 54, "Sat", 8, 1);
    break;
  case 7:
    hal_Oled_ShowString(106, 54, "Sun", 8, 1);
    break;
  default:
    break;
  }
  hal_Oled_Refresh();
}

static void menuInit(void) {
  pModeMenu =
      &generalModeMenu[GNL_MENU_DESKTOP]; // 设置上电显示的菜单界面为桌面显示
  pModeMenu->refreshScreenCmd =
      SCREEN_CMD_RESET; // 更新刷新界面标志，进入界面后刷新全界面UI
}

static void gnlMenu_DesktopCBS(void) {
  if (pModeMenu->refreshScreenCmd == SCREEN_CMD_RESET) {
    pModeMenu->refreshScreenCmd = SCREEN_CMD_NULL;
    pModeMenu->keyVal = 0xff;
    hal_Oled_Clear();
    hal_Oled_ShowString(0, 0, "N", 8, 1);

    hal_Oled_ShowString(16, 20, "Away arm", 24, 1);
    showSystemTime();

    QueueEmpty(RFDRcvMsg);

    hal_Oled_Refresh();
  }
}

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

// RFD接收回调函数
static void RfdRcvHandle(uint8_t *pBuff) {
  uint8_t temp;
  temp = '#';
  QueueDataIn(RFDRcvMsg, &temp, 1);
  QueueDataIn(RFDRcvMsg, &pBuff[0], 3);
}