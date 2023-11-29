#include "app.h"
#include "hal_cpu.h"
#include "hal_key.h"
#include "hal_led.h"
#include "hal_timer.h"
#include "os/os_system.h"
#include "stm32f10x.h"

int main(void) {
  hal_CpuInit();
  OS_TaskInit();
  hal_TimerInit();

  hal_LedInit();
  OS_CreatTask(OS_TASK1, hal_LedProc, 1, OS_RUN);

  hal_KeyInit();
  OS_CreatTask(OS_TASK2, hal_KeyProc, 1, OS_RUN);

  AppInit();
  OS_CreatTask(OS_TASK3,AppProc,1,OS_RUN);
  OS_Start();
  return 0;
}

// int main() {
//   GPIO_InitTypeDef GPIO_InitStructure;
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//   // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//   GPIO_Init(GPIOA, &GPIO_InitStructure);
//   // GPIO_ResetBits(GPIOA, GPIO_Pin_1);

//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//   GPIO_Init(GPIOB, &GPIO_InitStructure);
//   // GPIO_ResetBits(GPIOB, GPIO_Pin_0);

//   while (1) {
//     GPIO_SetBits(GPIOA, GPIO_Pin_1);
//     GPIO_SetBits(GPIOB, GPIO_Pin_0);
//     Delay(0x0FFFFF);
//     GPIO_ResetBits(GPIOA, GPIO_Pin_1);
//     GPIO_ResetBits(GPIOB, GPIO_Pin_0);
//     Delay(0x0FFFFF);
//   }

//   return 0;
// }