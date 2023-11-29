#ifndef HAL_TIME_H
#define HAL_TIME_H

#include <stdint.h>

typedef enum {
  T_LED,
  T_SUM,
} TIMER_ID_TYPEDEF;

typedef enum {
  T_SUCCESS,
  T_FAIL,
} TIMER_RESULT_TYPEDEF;

typedef enum {
  T_STA_INVAILD,
  T_STA_STOP,
  T_STA_START,
} TIMER_STATE_TYPEDEF;

typedef struct {
  TIMER_STATE_TYPEDEF state;
  uint16_t CurrentCount;
  uint16_t Period;
  void (*func)(void);
} Stu_TimerTypeDef;

void hal_TimerInit(void);
void hal_CreatTimer(TIMER_ID_TYPEDEF id, void (*proc)(void), uint16_t Period,
                    TIMER_STATE_TYPEDEF state);
TIMER_RESULT_TYPEDEF hal_ResetTimer(TIMER_ID_TYPEDEF id,
                                    TIMER_STATE_TYPEDEF state);
#endif
