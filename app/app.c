#include "app.h"
#include "OS_System.h"
#include "hal_key.h"
#include "hal_led.h"
#include "hal_oled.h"
#include "hal_rfd.h"

static void KeyEventHandle(KEY_VALUE_TYPEDEF keys);
static void RfdRcvHandle(uint8_t *pBuff);

Queue8 RFDRcvMsg; // RFD���ն���

void AppInit(void) {
  QueueEmpty(RFDRcvMsg);
  hal_KeyScanCBSRegister(KeyEventHandle);
  hal_OledInit();
  hal_RFCRcvCBSRegister(RfdRcvHandle);
  hal_Oled_ShowString(40, 2, "Recode", 16, 1);
  // hal_Oled_ShowString(16, 20, "Smart alarm", 16, 0);
  // hal_Oled_ShowString(40, 40, "system", 16, 1);
  hal_Oled_Refresh();
}

void AppProc(void) {
  unsigned char tBuff[3], dat;
  if (QueueDataLen(RFDRcvMsg)) {
    QueueDataOut(RFDRcvMsg, &dat);
    if (dat == '#') {
      QueueDataOut(RFDRcvMsg, &tBuff[2]); // ��ַ��
      QueueDataOut(RFDRcvMsg, &tBuff[1]); // ��ַ��
      QueueDataOut(RFDRcvMsg, &tBuff[0]); // ������

      dat = (tBuff[2] >> 4) & 0x0F;
      if (dat > 9) {
        hal_Oled_ShowChar(20, 40, (dat - 10) + 'A', 16, 1);
      } else {
        hal_Oled_ShowChar(20, 40, dat + '0', 16, 1);
      }

      dat = tBuff[2] & 0x0F;
      if (dat > 9) {
        hal_Oled_ShowChar(28, 40, (dat - 10) + 'A', 16, 1);
      } else {
        hal_Oled_ShowChar(28, 40, dat + '0', 16, 1);
      }

      hal_Oled_ShowChar(36, 40, ' ', 16, 1);

      dat = (tBuff[1] >> 4) & 0x0F;
      if (dat > 9) {
        hal_Oled_ShowChar(44, 40, (dat - 10) + 'A', 16, 1);
      } else {
        hal_Oled_ShowChar(44, 40, dat + '0', 16, 1);
      }

      dat = tBuff[1] & 0x0F;
      if (dat > 9) {
        hal_Oled_ShowChar(52, 40, (dat - 10) + 'A', 16, 1);
      } else {
        hal_Oled_ShowChar(52, 40, dat + '0', 16, 1);
      }

      hal_Oled_ShowChar(60, 40, ' ', 16, 1);

      dat = (tBuff[0] >> 4) & 0x0F;
      if (dat > 9) {
        hal_Oled_ShowChar(68, 40, (dat - 10) + 'A', 16, 1);
      } else {
        hal_Oled_ShowChar(68, 40, dat + '0', 16, 1);
      }

      dat = tBuff[0] & 0x0F;
      if (dat > 9) {
        hal_Oled_ShowChar(76, 40, (dat - 10) + 'A', 16, 1);
      } else {
        hal_Oled_ShowChar(76, 40, dat + '0', 16, 1);
      }

      hal_Oled_Refresh();
    }
  }
}

//-----------------������ص�������------------------------

// �����ص�����
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

// RFD���ջص�����
static void RfdRcvHandle(uint8_t *pBuff) {
  uint8_t temp;
  temp = '#';
  QueueDataIn(RFDRcvMsg, &temp, 1);
  QueueDataIn(RFDRcvMsg, &pBuff[0], 3);
}