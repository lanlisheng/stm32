#ifndef _HAL_TIMER_H
#define _HAL_TIMER_H

#include <stdint.h>

typedef enum {
  T_LED,
  T_BEEP,
  T_SUM,
} TIMER_ID_TYPEDEF;

typedef enum {
  T_SUCCESS,
  T_FAIL,
} TIMER_RESULT_TYPEDEF;

typedef enum {
  T_STA_INVALID,
  T_STA_STOP,
  T_STA_START,
} TIMER_STATE_TYPEDEF;

typedef struct {
  TIMER_STATE_TYPEDEF state;
  uint32_t CurrentCount;
  uint32_t Period;
  void (*func)(void);
} Stu_TimerTypeDef;

void hal_timerInit(void);
void hal_CreatTimer(TIMER_ID_TYPEDEF id, void (*proc)(void), uint32_t Period,
                    TIMER_STATE_TYPEDEF state);
TIMER_RESULT_TYPEDEF hal_CtrlTimerAction(TIMER_ID_TYPEDEF id,
                                         TIMER_STATE_TYPEDEF sta);
TIMER_RESULT_TYPEDEF hal_ResetTimer(TIMER_ID_TYPEDEF id,
                                    TIMER_STATE_TYPEDEF sta);
TIMER_STATE_TYPEDEF hal_GetTimerState(TIMER_ID_TYPEDEF id);

#endif