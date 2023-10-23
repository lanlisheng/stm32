#include "smart_system.h"
#include "smart.h"
#include "smart_delay.h"
#include "smart_irq.h"

/* Local Macros */
#define MEM8(addr) (*((volatile uint8_t *)(addr)))
#define MEM16(addr) (*((volatile uint16_t *)(addr)))
#define MEM32(addr) (*((volatile uint32_t *)(addr)))

void SMART_System_Init(void) {
  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  SysTick_Config(SystemCoreClock / 1000);

  SMART_DELAY_Init();

  /* Set the Vector Table base address as specified in .ld file */
  extern void *smart_isr_vector_table_base;
  NVIC_SetVectorTable((uint32_t)&smart_isr_vector_table_base, 0x0);

  /*
   * Turn on all the peripheral clocks.
   * Micromanaging clocks makes no sense given the power situation in the
   * system, so light up everything we might reasonably use here and just leave
   * it on.
   */
  RCC_AHB1PeriphClockCmd(
      RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |
          RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
          RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI |
          RCC_AHB1Periph_CRC | RCC_AHB1Periph_FLITF | RCC_AHB1Periph_SRAM1 |
          RCC_AHB1Periph_SRAM2 | RCC_AHB1Periph_BKPSRAM | RCC_AHB1Periph_DMA1 |
          RCC_AHB1Periph_DMA2 |
          // RCC_AHB1Periph_ETH_MAC |			No ethernet
          // RCC_AHB1Periph_ETH_MAC_Tx |
          // RCC_AHB1Periph_ETH_MAC_Rx |
          // RCC_AHB1Periph_ETH_MAC_PTP |
          // RCC_AHB1Periph_OTG_HS |			No high-speed USB
          // (requires
          // external PHY) RCC_AHB1Periph_OTG_HS_ULPI |		No ULPI PHY (see
          // above)
          0,
      ENABLE);
  RCC_AHB2PeriphClockCmd(
      // RCC_AHB2Periph_DCMI |				No camera   @todo might
      // make sense later for basic vision support? RCC_AHB2Periph_CRYP |
      // No crypto RCC_AHB2Periph_HASH |				No hash
      // generator
      // RCC_AHB2Periph_RNG |				No random numbers @todo
      // might be good to have later if entropy is desired RCC_AHB2Periph_OTG_FS
      // |
      0, ENABLE);
  RCC_AHB3PeriphClockCmd(
      // RCC_AHB3Periph_FSMC |				No external static
      // memory
      0, ENABLE);
  RCC_APB1PeriphClockCmd(
      RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 |
          RCC_APB1Periph_TIM5 | RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM7 |
          RCC_APB1Periph_TIM12 | RCC_APB1Periph_TIM13 | RCC_APB1Periph_TIM14 |
          RCC_APB1Periph_WWDG | RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3 |
          RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3 | RCC_APB1Periph_UART4 |
          RCC_APB1Periph_UART5 | RCC_APB1Periph_I2C1 | RCC_APB1Periph_I2C2 |
          RCC_APB1Periph_I2C3 | RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2 |
          RCC_APB1Periph_PWR | RCC_APB1Periph_DAC | 0,
      ENABLE);

  RCC_APB2PeriphClockCmd(
      RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM8 | RCC_APB2Periph_USART1 |
          RCC_APB2Periph_USART6 | RCC_APB2Periph_ADC | RCC_APB2Periph_ADC1 |
          RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC3 | RCC_APB2Periph_SDIO |
          RCC_APB2Periph_SPI1 | RCC_APB2Periph_SYSCFG | RCC_APB2Periph_TIM9 |
          RCC_APB2Periph_TIM10 | RCC_APB2Periph_TIM11 | 0,
      ENABLE);

  /*
   * Configure all pins as input / pullup to avoid issues with
   * uncommitted pins, excepting special-function pins that we need to
   * remain as-is.
   */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // default is un-pulled
                                                 // input

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
#if (SMART_USB_ENABLED)
  GPIO_InitStructure.GPIO_Pin &= ~(GPIO_Pin_11 | GPIO_Pin_12);
// leave USB D+/D- alone
#endif
  GPIO_InitStructure.GPIO_Pin &=
      ~(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15); // leave JTAG pins alone
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  GPIO_Init(GPIOI, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_All);
  GPIO_SetBits(GPIOB, GPIO_Pin_All);
  GPIO_SetBits(GPIOC, GPIO_Pin_All);
  GPIO_SetBits(GPIOD, GPIO_Pin_All);
}

void SMART_SYS_Reset(void) {
  // disable all interrupts
  SMART_IRQ_Disable();

  /* XXX F10x port resets most (but not all) peripherals ... do we care? */

  /* Reset STM32 */
  NVIC_SystemReset();
}

/**
 * Returns the serial number as a string
 * param[out] str pointer to a string which can store at least 32 digits + zero
 * terminator! (24 digits returned for STM32) return < 0 if feature not
 * supported
 */
int SMART_SYS_SerialNumberGet(char *str) {
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
