#include "hal_timer.h"
#include "hal_led.h"
#include "stm32f10x.h"

static void hal_timer4Config(void);
static void hal_TimerHandle(void);

volatile Stu_TimerTypeDef Stu_Timer[T_SUM];

void hal_TimerInit(void) {
  hal_timer4Config();
  for (int i = 0; i < T_SUM; ++i) {
    Stu_Timer[i].state = T_STA_STOP;
    Stu_Timer[i].CurrentCount = 0;
    Stu_Timer[i].Period = 0;
    Stu_Timer[i].func = 0;
  }
}

static void hal_timer4Config(void) {
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

void hal_CreatTimer(TIMER_ID_TYPEDEF id, void (*proc)(void), uint16_t Period,
                    TIMER_STATE_TYPEDEF state) {
  Stu_Timer[id].state = state;
  Stu_Timer[id].CurrentCount = 0;
  Stu_Timer[id].Period = Period;
  Stu_Timer[id].func = proc;
}

TIMER_RESULT_TYPEDEF hal_CtrlTimerAction(TIMER_ID_TYPEDEF id,
                                         TIMER_STATE_TYPEDEF sta) {
  if (Stu_Timer[id].func) // 判断定时器是否存在
  {
    Stu_Timer[id].state = sta;
    return T_SUCCESS;
  } else {
    return T_FAIL;
  }
}

TIMER_STATE_TYPEDEF hal_GetTimerState(TIMER_ID_TYPEDEF id) {
  if (Stu_Timer[id].func) // 判断定时器是否存在
  {
    return Stu_Timer[id].state;

  } else {
    return T_STA_INVAILD;
  }
}

TIMER_RESULT_TYPEDEF hal_DeleteTimer(TIMER_ID_TYPEDEF id) {
  if (Stu_Timer[id].func) {
    Stu_Timer[id].state = T_STA_STOP;

    Stu_Timer[id].CurrentCount = 0;
    // Stu_Timer[id].Period = 0;
    Stu_Timer[id].func = 0;
    return T_SUCCESS;
  } else {
    return T_FAIL;
  }
}

TIMER_RESULT_TYPEDEF hal_ResetTimer(TIMER_ID_TYPEDEF id,
                                    TIMER_STATE_TYPEDEF state) {
  if (Stu_Timer[id].func) {
    Stu_Timer[id].state = state;
    Stu_Timer[id].CurrentCount = 0;
    return T_SUCCESS;
  } else {
    return T_FAIL;
  }
}

static void hal_TimerHandle(void) {
  for (int i = 0; i < T_SUM; ++i) {
    if ((Stu_Timer[i].func) && (Stu_Timer[i].state == T_STA_START)) {
      Stu_Timer[i].CurrentCount++;
      if (Stu_Timer[i].CurrentCount >= Stu_Timer[i].Period) {
        Stu_Timer[i].CurrentCount = Stu_Timer[i].CurrentCount;
        Stu_Timer[i].state = T_STA_STOP;
        Stu_Timer[i].func();
      }
    }
  }
}

void TIM4_IRQHandler(void) {
  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
  hal_TimerHandle();
}
