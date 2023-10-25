#ifndef OS_SYSTEM_H
#define OS_SYSTEM_H

#include <stdint.h>

typedef enum {
  CPU_ENTER_CRITICAL, // CPU进入临界
  CPU_EXIT_CRITICAL,  // CPU退出临界
} CPU_EA_TYPEDEF;

// 定义一个CPU中断控制回调函数指针
typedef void (*CPUInterrupt_CallBack_t)(CPU_EA_TYPEDEF cmd, uint8_t *pSta);

typedef enum {
  OS_TASK1,
  OS_TASK_SUM,
} OS_TaskIDTypeDef;

typedef enum {
  OS_SLEEP, // 任务休眠
  OS_RUN,   // 任务运行
} OS_TaskStatusTypeDef;

// 任务结构体
typedef struct {
  void (*task)(void);           // 任务执行函数指针
  OS_TaskStatusTypeDef RunFlag; // 任务运行状态
  uint16_t RunPeriod;           // 任务调度频率
  uint16_t RunTimer;            // 任务调度计时器
} OS_TaskTypeDef;

void OS_ClockInterruptHandle(void);
void OS_CPUInterruptCBSRegister(CPUInterrupt_CallBack_t pCPUInterruptCtrlCBS);
void OS_TaskInit(void);
void OS_CreatTask(unsigned char ID, void (*proc)(void), unsigned short TimeDly,
                  OS_TaskStatusTypeDef flag);
void OS_Start(void);
void OS_TaskGetUp(OS_TaskIDTypeDef taskID);
void OS_TaskSleep(OS_TaskIDTypeDef taskID);
#endif
