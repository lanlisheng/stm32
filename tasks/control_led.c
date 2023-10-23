#include "smart.h"

void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount) {
  while (nCount--) {
  }
}

int32_t Control_Led_Task_Init(void) { return 0; }

int32_t Control_Led_Task(void) {
  // GPIO_SetBits(GPIOA, GPIO_Pin_1);
  // GPIO_SetBits(GPIOB, GPIO_Pin_0);
  // Delay(0x0FFFFF);
  // GPIO_ResetBits(GPIOA, GPIO_Pin_1);
  // GPIO_ResetBits(GPIOB, GPIO_Pin_0);
  // Delay(0x0FFFFF);
  // static uint32_t lastTimer0 = 0;
  // static uint32_t Timer_cnt = 0;
  // if (lastTimer0 == 0)
  //   lastTimer0 = SMART_Get_SystemTime();
  // if ((SMART_Get_SystemTime() - lastTimer0) > 1000) {
  //   lastTimer0 = SMART_Get_SystemTime();
  //   Timer_cnt++;
  //   if (Timer_cnt == 1) {
  //     GPIO_SetBits(GPIOA, GPIO_Pin_1);
  //   } else if (Timer_cnt == 2) {
  //     GPIO_ResetBits(GPIOA, GPIO_Pin_1);
  //   }
  //   if (Timer_cnt == 3) {
  //     Timer_cnt = 0;
  //   }
  // }
  // return 0;
}

MODULE_INITCALL(Control_Led_Task_Init)
MODULE_LOOPER(Control_Led_Task)