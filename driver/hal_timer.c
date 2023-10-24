#include "hal_timer.h"
#include "hal_led.h"
#include "stm32f10x.h"

void hal_timer4Config(void) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  TIM_DeInit(TIM4);
  // TIM_TimeBaseStructure.TIM_Period = 10000; 			//10ms
  //  TIM_TimeBaseStructure.TIM_Period = 1000; // 1ms
  TIM_TimeBaseStructure.TIM_Period = 50; // 50us
  TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM4, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM4_IRQHandler(void) {
  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
  static unsigned short i = 0;
  i++;
  if (i > 10000) // 50us*10000 = 500ms
  {
    i = 0;
    hal_LedTurn();
  }
}