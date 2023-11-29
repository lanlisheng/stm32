#include "os_system.h"

volatile OS_TaskTypeDef OS_Task[OS_TASK_SUM];

CPUInterrupt_CallBack_t CPUInterruptCtrlCBS;

/********************************************************************************************************
 *  @函数名   OS_CPUInterruptCBSRegister
 *  @描述     注册CPU中断控制函数
 *  @参数     pCPUInterruptCtrlCBS-CPU中断控制回调函数地址
 *  @返回值   void
 *  @注意     None
 ********************************************************************************************************/
void OS_CPUInterruptCBSRegister(CPUInterrupt_CallBack_t pCPUInterruptCtrlCBS) {
  if (CPUInterruptCtrlCBS == 0) {
    CPUInterruptCtrlCBS = pCPUInterruptCtrlCBS;
  }
}

/********************************************************************************************************
 *  @函数名   OS_TaskInit
 *  @描述     系统任务初始化
 *  @参数     None
 *  @返回值   None
 *  @注意     None
 ********************************************************************************************************/
void OS_TaskInit(void) {
  for (int i = 0; i < OS_TASK_SUM; ++i) {
    OS_Task[i].task = 0;
    OS_Task[i].RunFlag = OS_SLEEP;
    OS_Task[i].RunPeriod = 0;
    OS_Task[i].RunTimer = 0;
  }
}

/********************************************************************************************************
 *  @函数名   OS_CreatTask
 *  @描述     创建任务
 *  @参数     任务id 用户函数入口地址(*proc)() Period 任务执行频率 flag 任务状态
 *  @返回值    None
 *  @注意     None
 ********************************************************************************************************/
void OS_CreatTask(uint8_t ID, void (*proc)(void), uint16_t Period,
                  OS_TaskStatusTypeDef flag) {
  if (!OS_Task[ID].task) {
    OS_Task[ID].task = proc;
    OS_Task[ID].RunFlag = OS_SLEEP;
    OS_Task[ID].RunPeriod = Period;
    OS_Task[ID].RunTimer = 0;
  }
}
/********************************************************************************************************
 *  @函数名   OS_ClockInterruptHandle
 *  @描述     系统任务调度函数
 *  @参数     None
 *  @返回值   None
 *  @注意     10ms进入一次该函数
 ********************************************************************************************************/
void OS_ClockInterruptHandle(void) {
  for (int i = 0; i < OS_TASK_SUM; ++i) {
    if (OS_Task[i].task) {
      OS_Task[i].RunTimer++;
      if (OS_Task[i].RunTimer >= OS_Task[i].RunPeriod) {
        OS_Task[i].RunTimer = 0;
        OS_Task[i].RunFlag = OS_RUN;
      }
    }
  }
}
/********************************************************************************************************
 *  @函数名   OS_Start
 *  @描述     任务开始
 *  @参数     None
 *  @返回值   None
 *  @注意     None
 ********************************************************************************************************/
void OS_Start(void) {
  while (1) {
    for (int i = 0; i < OS_TASK_SUM; ++i) {
      if (OS_Task[i].RunFlag == OS_RUN) {
        OS_Task[i].RunFlag = OS_SLEEP;
        (*(OS_Task[i].task))(); // 任务开始运行，死循环
      }
    }
  }
}
/********************************************************************************************************
 *  @函数名   OS_TaskGetUp
 *  @描述     唤醒一个任务
 *  @参数     唤醒任务ID
 *  @返回值   None
 *  @注意     None
 ********************************************************************************************************/
void OS_TaskGetUp(OS_TaskIDTypeDef TaskID) {
  uint8_t IptStatus;
  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
  }
  OS_Task[TaskID].RunFlag = OS_RUN;
  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
  }
}
/********************************************************************************************************
 *  @函数名   OS_TaskSleep
 *  @描述     挂起一个任务
 *  @参数     TaskID 需要挂起的任务ID
 *  @返回值   None
 *  @注意     None
 ********************************************************************************************************/
void OS_TaskSleep(OS_TaskIDTypeDef TaskID) {
  uint8_t IptStatus;
  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
  }
  OS_Task[TaskID].RunFlag = OS_SLEEP;
  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
  }
}

void S_QueueEmpty(unsigned char **Head, unsigned char **Tail,
                  unsigned char *HBuff) {
  *Head = HBuff;
  *Tail = HBuff;
}

void S_QueueDataIn(unsigned char **Head, unsigned char **Tail,
                   unsigned char *HBuff, unsigned short Len,
                   unsigned char *HData, unsigned short DataLen) {
  unsigned short num;
  unsigned char IptStatus;

  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
  }
  for (num = 0; num < DataLen; num++, HData++) {
    **Tail = *HData;
    (*Tail)++;
    if (*Tail == HBuff + Len)
      *Tail = HBuff;
    if (*Tail == *Head) {
      if (++(*Head) == HBuff + Len)
        *Head = HBuff;
    }
  }
  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
  }
}

unsigned char S_QueueDataOut(unsigned char **Head, unsigned char **Tail,
                             unsigned char *HBuff, unsigned short Len,
                             unsigned char *Data) {
  unsigned char back = 0;
  unsigned char IptStatus;
  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
  }
  *Data = 0;
  if (*Tail != *Head) {
    *Data = **Head;
    back = 1;
    if (++(*Head) == HBuff + Len)
      *Head = HBuff;
  }
  if (CPUInterruptCtrlCBS != 0) {
    CPUInterruptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
  }
  return back;
}

unsigned short S_QueueDataLen(unsigned char **Head, unsigned char **Tail,
                              unsigned short Len) {
  if (*Tail > *Head)
    return *Tail - *Head;
  if (*Tail < *Head)
    return *Tail + Len - *Head;
  return 0;
}
