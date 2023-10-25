#include "hal_cpu.h"
#include "hal_led.h"
#include "os/os_system.h"
#include "stm32f10x.h"

static void hal_CoreClockInit(void);
static void hal_CPU_Critical_Control(CPU_EA_TYPEDEF cmd, uint8_t *pSta);

void hal_CpuInit(void) {
  hal_CoreClockInit();
  OS_CPUInterruptCBSRegister(hal_CPU_Critical_Control);
}

static void hal_CoreClockInit(void) {
  SysTick_Config(SystemCoreClock / 100); // 10ms
}

/********************************************************************************************************
 *  @函数名   hal_getprimask
 *  @描述     获取CPU总中断状态
 *  @参数     无
 *  @返回值   0-总中断关闭 1-总中断打开
 *  @注意     无
 ********************************************************************************************************/
static uint8_t hal_getprimask(void) {
  return (!__get_PRIMASK()); // 0是中断打开，1是中断关闭，所以要取反
}
/********************************************************************************************************
 *  @函数名   hal_CPU_Critical_Control
 *  @描述     CPU临界处理控制
 *  @参数     cmd-控制命令  *pSta-总中断状态
 *  @返回值   无
 *  @注意     无
 ********************************************************************************************************/
static void hal_CPU_Critical_Control(CPU_EA_TYPEDEF cmd, uint8_t *pSta) {
  if (cmd == CPU_ENTER_CRITICAL) {
    *pSta = hal_getprimask(); // 保存中断状态
    __disable_irq();
  } else if (cmd == CPU_EXIT_CRITICAL) {
    if (*pSta) {
      __enable_irq();
    } else {
      __disable_irq();
    }
  }
}

void SysTick_Handler(void) {
  // 定时中断处理
  OS_ClockInterruptHandle(); // 10ms
}
