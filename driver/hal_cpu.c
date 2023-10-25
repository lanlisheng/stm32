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
 *  @������   hal_getprimask
 *  @����     ��ȡCPU���ж�״̬
 *  @����     ��
 *  @����ֵ   0-���жϹر� 1-���жϴ�
 *  @ע��     ��
 ********************************************************************************************************/
static uint8_t hal_getprimask(void) {
  return (!__get_PRIMASK()); // 0���жϴ򿪣�1���жϹرգ�����Ҫȡ��
}
/********************************************************************************************************
 *  @������   hal_CPU_Critical_Control
 *  @����     CPU�ٽ紦�����
 *  @����     cmd-��������  *pSta-���ж�״̬
 *  @����ֵ   ��
 *  @ע��     ��
 ********************************************************************************************************/
static void hal_CPU_Critical_Control(CPU_EA_TYPEDEF cmd, uint8_t *pSta) {
  if (cmd == CPU_ENTER_CRITICAL) {
    *pSta = hal_getprimask(); // �����ж�״̬
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
  // ��ʱ�жϴ���
  OS_ClockInterruptHandle(); // 10ms
}
