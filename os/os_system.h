#ifndef _OS_SYSTEM_H
#define _OS_SYSTEM_H

#include <stdint.h>

typedef enum {
  CPU_ENTER_CRITICAL,
  CPU_EXIT_CRITICAL,
} CPU_EA_TYPEDEF;

typedef void (*CPUInterrupt_CallBack_t)(CPU_EA_TYPEDEF cmd, uint8_t *pSta);

typedef enum {
  OS_TASK1,
  OS_TASK2,
  OS_TASK3,
  OS_TASK4,
  OS_TASK5,
  OS_TASK6,
  OS_TASK_SUM
} OS_TaskIDTypeDef;

typedef enum {
  OS_SLEEP,
  OS_RUN = !OS_SLEEP,
} OS_TaskStatusTypeDef;

typedef struct {
  void (*task)(void);
  OS_TaskStatusTypeDef RunFlag;
  uint32_t RunPeriod;
  uint32_t RunTimer;
} OS_TaskTypeDef;

#endif