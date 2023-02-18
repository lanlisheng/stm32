/*
 * @creater:  lanlisheng
 * @since: 2023-02-18 21:30:58
 * @lastTime: 2023-02-18 21:49:57
 * @LastAuthor: lanlisheng
 * @文件相对于项目的路径: \stm32f405\user\main.c
 */
// #include "stm32f4xx.h"
#include "stm32f10x.h"
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount) {
  while (nCount--) {
  }
}

int main() {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_1);

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  // GPIO_Init(GPIOB, &GPIO_InitStructure);
  // GPIO_ResetBits(GPIOB, GPIO_Pin_0);

  while (1) {
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    Delay(0xeFFFFF);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    Delay(0xeFFFFF);
  }

  return 0;
}