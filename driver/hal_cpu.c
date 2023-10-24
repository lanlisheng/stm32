#include "hal_cpu.h"
#include "hal_led.h"
#include "stm32f10x.h"

static void hal_CoreClockInit(void);

void hal_CpuInit(void) { hal_CoreClockInit(); }

static void hal_CoreClockInit(void) {
  SysTick_Config(SystemCoreClock / 100); // 10ms
  // SysTick_Config(720000);			 //10ms
}

void SysTick_Handler(void) {
  static unsigned char i = 0;
  i++;
  if (i > 10) {
    i = 0;
    hal_LedTurn();
  }
  // 定时中断处理
}
