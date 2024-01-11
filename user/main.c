#include "app.h"
#include "hal_cpu.h"
#include "hal_key.h"
#include "hal_led.h"
#include "hal_oled.h"
#include "hal_timer.h"
#include "os_system.h"

int main(void) {
  hal_CPUInit();
  OS_TaskInit();
  hal_TimerInit();

  hal_LedInit();
  OS_CreatTask(OS_TASK1, hal_LedProc, 1, OS_RUN);

  hal_KeyInit();
  OS_CreatTask(OS_TASK2, hal_KeyProc, 1, OS_RUN);

  AppInit();
  OS_CreatTask(OS_TASK3, AppProc, 1, OS_RUN);

  OS_Start();
}
