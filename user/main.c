// #include "stm32f4xx.h"
#include "hal_cpu.h"
#include "hal_led.h"
#include "hal_timer.h"
#include "smart.h"
#include "stm32f10x.h"

int main(void) {
  hal_ledConfig();
  hal_CpuInit();
  hal_timer4Config();
  while (1) {
  }
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