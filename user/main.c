#include "OS_System.h"
#include "app.h"
#include "hal_cpu.h"
#include "hal_key.h"
#include "hal_led.h"
#include "hal_rfd.h"
#include "hal_timer.h"

int main(void) {

  hal_CPUInit();
  OS_TaskInit();
  hal_TimerInit();

  hal_LedInit();
  OS_CreatTask(OS_TASK1, hal_LedProc, 1, OS_RUN);

  hal_KeyInit();
  OS_CreatTask(OS_TASK2, hal_KeyProc, 1, OS_RUN);

  hal_RFDInit();
  OS_CreatTask(OS_TASK3, hal_RFDProc, 1, OS_RUN);

  AppInit();
  OS_CreatTask(OS_TASK4, AppProc, 1, OS_RUN);

  OS_Start();
}