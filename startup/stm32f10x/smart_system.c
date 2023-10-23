#include "smart_system.h"
#include "misc.h"
#include "smart_irq.h"
/* Local Macros */
#define MEM8(addr) (*((volatile uint8_t *)(addr)))
#define MEM16(addr) (*((volatile uint16_t *)(addr)))
#define MEM32(addr) (*((volatile uint32_t *)(addr)))

void SMART_System_Init(void) {

  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  SysTick_Config(SystemCoreClock / 1000);

  /* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                             RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                             RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,
                         ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);
  // #if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || defined
  // (STM32F10X_MD) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD) ||
  // defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined
  // (STM32F10X_CL)
  NVIC_Configuration();
  // #endif
}

void SMART_SYS_Reset(void) {
  // disable all interrupts
  SMART_IRQ_Disable();

  /* XXX F10x port resets most (but not all) peripherals ... do we care? */

  /* Reset STM32 */
  NVIC_SystemReset();
}

/**
 * Configures Vector Table base location and SysTick
 */
void NVIC_Configuration(void) {
  /* Set the Vector Table base address as specified in .ld file */
  extern void *smart_isr_vector_table_base;
  NVIC_SetVectorTable((uint32_t)&smart_isr_vector_table_base, 0x0);

  /* 4 bits for Interrupt priorities so no sub priorities */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}
// #endif /* STM32F10X_xxx */

/* Returns the serial number as a string
 * param[out] str pointer to a string which can store at least 32 digits + zero
 * terminator! (24 digits returned for STM32) return < 0 if feature not
 * supported
 */
int32_t SMART_SYS_SerialNumberGet(char *str) {
  int i;

  /* Stored in the so called "electronic signature" */
  for (i = 0; i < SMART_SYS_SERIAL_NUM_ASCII_LEN; ++i) {
    uint8_t b = MEM8(0x1fff7a10 + (i / 2));
    if (!(i & 1))
      b >>= 4;
    b &= 0x0f;

    str[i] = ((b > 9) ? ('A' - 10) : '0') + b;
  }
  str[i] = '\0';

  /* No error */
  return 0;
}
