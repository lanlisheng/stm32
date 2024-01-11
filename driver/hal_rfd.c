#include "hal_rfd.h"
#include "OS_System.h"
#include "hal_oled.h"
#include "hal_timer.h"
#include "stm32F10x.h"
#include <stdbool.h>
#include <string.h>

volatile uint8_t RFD_ResendNum;
uint8_t RFD_Addr[3];      // RFD数据码存储
volatile Queue32 RFDBuff; // RFD数据接收队列
volatile Queue32 RFDTxMsg;
volatile uint8_t RFD_DecodeFltTimerOk; // RFD接码定时时间到标志

Queue8 RFDCodeBuff;

RFD_RcvCallBack_t RFCRcvCBS;

// 接码流程，总共分为2个步骤，先解数据头，再解数据体
enum {
  RFD_READ_CLKLEN, // ev1527 RFD编码数据头
  RFD_READ_DATA    // ev1527 RFD编码数据体
};

uint8_t RFDRcvSteps; // RFD编码接收流程变量

void RFD_CodeHandle(uint8_t *pCode);
static void hal_RFDConfig(void);
static uint8_t hal_GetRFDIOSta(void);
static void hal_PulseAQT_Handle(void);
static void hal_RFDDecodeFlt_Handle(void);

static void hal_RFDConfig(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RFD_RX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(RFD_RX_PORT, &GPIO_InitStructure);
}

void hal_RFDInit(void) {
  RFD_DecodeFltTimerOk = 0;
  RFCRcvCBS = 0;
  hal_RFDConfig();

  QueueEmpty(RFDBuff);
  QueueEmpty(RFDCodeBuff);
  QueueEmpty(RFDTxMsg);

  RFDRcvSteps = RFD_READ_CLKLEN;
  for (int i = 0; i < 3; i++) {
    RFD_Addr[i] = 0;
  }

  hal_CreatTimer(T_RFD_PULSH_RCV, hal_PulseAQT_Handle, 1, T_STA_START);
  hal_CreatTimer(T_RFD_RECODEFLT, hal_RFDDecodeFlt_Handle, 20000,
                 T_STA_STOP); // 1s
}

void hal_RFCRcvCBSRegister(RFD_RcvCallBack_t pCBS) {
  if (RFCRcvCBS == 0) {
    RFCRcvCBS = pCBS;
  }
}

void hal_RFDProc(void) {
  Queue256 ClkTimeBuff;
  static uint16_t Time1 = 0, Time2 = 0;
  static uint8_t ReadDataFlag = 0;
  static uint8_t Len, Code[3];
  static uint8_t CodeTempBuff[3];

  /* receive code */
  switch (RFDRcvSteps) {
  case RFD_READ_CLKLEN: {
    uint8_t Temp, Num;
    static uint8_t Dsta = 0;
    static uint16_t Count = 0;
    QueueEmpty(ClkTimeBuff);
    while (QueueDataOut(RFDBuff, &Temp)) {
      Num = 8;
      while (Num--) {
        if (Dsta) {
          if (!(Temp & 0x80)) {
            uint8_t Data;
            Data = Count / 256; // 存储高电平脉宽高字节
            Data |= 0x80;       // 电平标志 1为高 0为低
            QueueDataIn(ClkTimeBuff, &Data, 1);
            Data = Count % 256; // 存储高电平脉宽低字节
            QueueDataIn(ClkTimeBuff, &Data, 1);
            // 注意这里Data的值x50us等于脉宽时间
            Dsta = 0;
            Count = 0;
          }
        } else {
          // 0
          if (Temp & 0x80) // 这里检测电平是否为高，如果为高代表低电平脉宽结束
          {
            uint8_t Data;
            Data = Count / 256; // 存储低电平脉宽高字节
            // Data &= 0xFF7F;				//电平标志 1为高 0为低
            Data &= 0x7F; // Bit7为脉宽电平标志,1为高 0为低
            QueueDataIn(ClkTimeBuff, &Data, 1);
            Data = Count % 256; // 存储低电平脉宽低字节
            QueueDataIn(ClkTimeBuff, &Data, 1);
            // 注意这里Data的值x50us等于脉宽时间
            Dsta = 1;
            Count = 0;
          }
        }
        Count++;
        Temp <<= 1;
      }
    }
  }
  case RFD_READ_DATA: {
    while (QueueDataLen(ClkTimeBuff)) // 判断有没波形数据
    {

      if (!ReadDataFlag) {
        unsigned char Temp;
        while (!Time1 || !Time2) {

          if (!Time1) {
            // 获取第一个波形数据
            while (QueueDataOut(ClkTimeBuff, &Temp)) {
              // Driver_GUART1SendByByter('*');
              if (Temp & 0x80) // 先获取高电平波形
              {
                Temp &= 0xFF7F;
                Time1 = Temp * 256;
                QueueDataOut(ClkTimeBuff, &Temp);
                Time1 += Temp;
                Time2 = 0;
                break;
              } else
                QueueDataOut(ClkTimeBuff, &Temp);
            }
            if (!QueueDataLen(ClkTimeBuff))
              break;
          }

          if (!Time2) {
            // 获取低电平波形
            QueueDataOut(ClkTimeBuff, &Temp);
            Time2 = Temp * 256;
            QueueDataOut(ClkTimeBuff, &Temp);
            Time2 += Temp;
            // 判断高电平*20时间，和高电平*44的时间
            if ((Time2 >= (Time1 * RFD_TITLE_CLK_MINL)) &&
                (Time2 <= (Time1 * RFD_TITLE_CLK_MAXL))) {
              Time1 = 0;
              Time2 = 0;
              Len = 0;
              ReadDataFlag = 1;
              break;
            } else {
              Time1 = 0;
              Time2 = 0;
            }
          }
        }
      }

      if (ReadDataFlag) {
        unsigned char Temp;

        if (!Time1) {
          if (QueueDataOut(ClkTimeBuff, &Temp)) {
            // 这里经过头验证后，第一个字节肯定是高电平，所以要&0xFF7F把高电平标志位清除
            Temp &= 0xFF7F;
            Time1 = Temp * 256;
            QueueDataOut(ClkTimeBuff, &Temp);
            Time1 += Temp;
            Time2 = 0;
          } else
            break;
        }

        if (!Time2) {
          if (QueueDataOut(ClkTimeBuff, &Temp)) {
            bool RecvSuccFlag;
            Time2 = Temp * 256;
            QueueDataOut(ClkTimeBuff, &Temp);
            Time2 += Temp;

            if ((Time1 > (Time2 * RFD_DATA_CLK_MINL)) &&
                (Time1 <= (Time2 * RFD_DATA_CLK_MAXL))) {
              unsigned char i, c = 0x80;
              //'1'
              for (i = 0; i < Len % 8; i++) {
                c >>= 1;
                c &= 0x7F;
              }
              Code[Len / 8] |= c;
              RecvSuccFlag = 1;
            } else if ((Time2 > (Time1 * RFD_DATA_CLK_MINL)) &&
                       (Time2 <= (Time1 * RFD_DATA_CLK_MAXL))) {
              unsigned char i, c = (unsigned char)0xFF7F;
              //'0'
              for (i = 0; i < Len % 8; i++) {
                c >>= 1;
                c |= 0x0080;
              }
              Code[Len / 8] &= c;
              RecvSuccFlag = 1;
            } else {
              // error
              RecvSuccFlag = 0;
              ReadDataFlag = 0;
            }
            Time1 = 0;
            Time2 = 0;
            if ((++Len == 24) && RecvSuccFlag) {
              ReadDataFlag = 0;
              if ((CodeTempBuff[0] == Code[0]) &&
                  (CodeTempBuff[1] == Code[1]) &&
                  (CodeTempBuff[2] == Code[2])) {
                RFD_CodeHandle(Code);
              } else {
                memcpy(CodeTempBuff, Code, 3);
              }
            }
          } else
            break;
        }
      }
    }
  }
  }
}

void RFD_CodeHandle(uint8_t *pCode) {
  static uint8_t tBuff[3];
  uint8_t temp;

  // pData = pCode;
  if ((hal_GetTimerState(T_RFD_RECODEFLT) == T_STA_START) &&
      (!RFD_DecodeFltTimerOk)) {
    // 1-定时器正在运行
    // 2-定时器计数未完成
    return;
  }

  hal_ResetTimer(T_RFD_RECODEFLT, T_STA_START); // 复位并重启定时器
  memcpy(tBuff, pCode, 3);
  RFD_DecodeFltTimerOk = 0;
  temp = '#';
  QueueDataIn(RFDCodeBuff, &temp, 1);
  QueueDataIn(RFDCodeBuff, &tBuff[0], 3);
  if (RFCRcvCBS) {
    RFCRcvCBS(tBuff);
  }
}

// 获取RFD接收IO口电平状态
static unsigned char hal_GetRFDIOSta(void) {
  return (GPIO_ReadInputDataBit(RFD_RX_PORT, RFD_RX_PIN));
}

// RFD脉冲采集，这个作为回调函数放在50us定时器中断里
static void hal_PulseAQT_Handle(void) {
  static unsigned char Temp, Count1 = 0;
  Temp <<= 1;
  if (hal_GetRFDIOSta())
    Temp |= 0x01;
  else
    Temp &= 0xFE;
  if (++Count1 == 8) {
    Count1 = 0;
    QueueDataIn(RFDBuff, &Temp, 1);
  }
  hal_ResetTimer(T_RFD_PULSH_RCV, T_STA_START);
}

// RFD重复码过滤处理，700ms内有重复码的话不做重复处理，这个作为回调函数放在700ms定时器中断里
static void hal_RFDDecodeFlt_Handle(void) { RFD_DecodeFltTimerOk = 1; }
