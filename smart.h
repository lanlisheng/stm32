#ifndef _SMART_H_
#define _SMART_H_

/* C Lib Includes */
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(STM32F40_41xxx)
#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#endif

#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) ||                       \
    defined(STM32F10X_MD) || defined(STM32F10X_MD_VL) ||                       \
    defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||                       \
    defined(STM32F10X_XL) || defined(STM32F10X_CL)
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#endif

#include "smart_initcall.h"
#include "smart_system.h"

extern uint32_t system_time_ms;
extern uint32_t SMART_Get_SystemTime();
extern uint32_t SMART_Get_Ms_Since(uint32_t time);

#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })

#define FALSE 0
#define TRUE 1
#define false 0
#define true 1

#define SMART_Assert(x)                                                        \
  do {                                                                         \
  } while (!(x))
#define SMART_DEBUG_Assert(x) SMART_Assert(x)

#define NELEMENTS(x) (sizeof(x) / sizeof(*(x)))

#define SMART_DS18B20_PARALLEL
#define SMART_INCLUDE_IRQ
#define SMART_INCLUDE_DELAY
#define SMART_INCLUDE_USART
// #define SMART_INCLUDE_COM
#define SAMRT_INCLUDE_ACTUATOR // 电缸连接
// #define SMART_INCLUDE_I2C
// #define SMART_INCLUDE_I2C2
// #define SMART_INCLUDE_I2C3
// #define SMART_INCLUDE_EXTI2
// #define SMART_INCLUDE_SPI
// #define SMART_INCLUDE_ADC
#define SMART_INCLUDE_CAN
#define SMART_INCLUDE_TIMER
#define SMART_INCLUDE_PWM

// #define SMART_INCLUDE_TEST_INTERVAL_TIMER
#define SMART_INCLUDE_FLASH_INTERNAL
#define SMART_INCLUDE_SETTINGS
// #define SMART_INCLUDE_FLASH_JEDEC
#define SMART_INCLUDE_CAN_MOTOR_STATUS

#if defined(SMART_INCLUDE_FLASH_INTERNAL) || defined(SMART_INCLUDE_FLASH_JEDEC)
#define SMART_INCLUDE_FLASH
#endif

// #define SMART_INCLUDE_HMC5883
// #define SMART_INCLUDE_MS5611
// #define SMART_INCLUDE_PX4FLOW
// #define SMART_INCLUDE_OLED
// #define SMART_INCLUDE_JY61
#define SMART_INCLUDE_R6093U
// #define SMART_INCLUDE_BMS_POWER
// #define SMART_INCLUDE_JIABAIDA_POWER
// #define SMART_INCLUDE_JIABAIDAV4_POWER
#define SMART_INCLUDE_KANGLIV82_POWER
// #define SMART_INCLUDE_YOUBEITE_BMS_POWER
#define SMART_INCLUDE_UART_LCD
#define SMART_INCLUDE_WS28XX

#if defined(SMART_INCLUDE_JIABAIDA_POWER) &&                                   \
    defined(SMART_INCLUDE_JIABAIDAV4_POWER)
#error "不能同时存在两种嘉佰达通讯协议"
#endif

// #define SMART_INCLUDE_WT931
// #define SMART_INCLUDE_OLED_DEBUG
// #define SMART_INCLUDE_ENCODER
// #define SMART_INCLUDE_PZEM_POWER
// #define SMART_INCLUDE_TTL2CAN
// #define SMART_INCLUDE_ZLAGV
// #define SMART_INCLUDE_IR_SWITCH
// #define SMART_INCLUDE_ENCODER_HARD_SPI
// #define SMART_INCLUDE_ASDB_MOTOR
// #define SMART_INCLUDE_ASDB_MOTOR_GROUP
// #define SMART_ASDB_GROUP_COUNT		1
// #define SMART_INCLUDE_HCSR04
// #define SMART_HCSR04_COUNT	3
#define SMART_INCLUDE_SERVO // pwm servo
#define SMART_INCLUDE_FAN
// #define SMART_INCLUDE_LOBOT_SERVO  // not pwm servo
// #define SMART_INCLUDE_STEPMOTOR_CONTROLLER2
// #define SMART_INCLUDE_STEPMOTOR_CONTROLLER
#define SMART_INCLUDE_TELEMETRY
// #define SMART_TELEMETRY_USB_COM1
#define TELEMETRY_UPDATE_DATA_INTERVAL
// #define SMART_INCLUDE_KS109
// #define SMART_INCLUDE_MPU6050
// #define SMART_MPU6050_ACCEL
// #define SMART_INCLUDE_IAP

#if defined(SMART_INCLUDE_JY61) || defined(SMART_INCLUDE_R6093U)
#define SMART_INCLUDE_ATTITUDE
#endif

#if defined(STM32F40_41xxx)
#define VREF_ADC 3.3f
#define SMART_ADC_SUB_DRIVER_MAX_INSTANCES 8
#define SMART_ADC_MAX_OVERSAMPLING 2
#endif

// #define SMART_SYSTEM_ID		2		// must > 0   defined in
// Makefile aready, make flash_$(ID), make flash SYSTEM_ID=$(ID)

#if defined(SMART_INCLUDE_COM)
#include "smart_com.h"
#include "smart_com_priv.h"
#endif

#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) ||                       \
    defined(STM32F10X_MD) || defined(STM32F10X_MD_VL) ||                       \
    defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||                       \
    defined(STM32F10X_XL) || defined(STM32F10X_CL)
#define SMART_SYSCLK 72000000
#define SMART_PERIPHERAL_APB1_CLOCK (SMART_SYSCLK / 1)
#define SMART_PERIPHERAL_APB2_CLOCK (SMART_SYSCLK / 1)
#endif

#if defined(STM32F40_41xxx)
#define SMART_SYSCLK 168000000
//	Peripherals that belongs to APB1 are:
//	DAC			|PWR				|CAN1,2
//	I2C1,2,3	|UART4,5			|USART3,2
//	I2S3Ext		|SPI3/I2S3			|SPI2/I2S2
//	I2S2Ext		|IWDG				|WWDG
//	RTC/BKP reg
// TIM2,3,4,5,6,7,12,13,14

// Calculated as SYSCLK / APBPresc * (APBPre == 1 ? 1 : 2)
// Default APB1 Prescaler = 4
#define SMART_PERIPHERAL_APB1_CLOCK (SMART_SYSCLK / 2)

//	Peripherals belonging to APB2
//	SDIO			|EXTI				|SYSCFG
//|SPI1 	ADC1,2,3 	USART1,6 	TIM1,8,9,10,11
//
// Default APB2 Prescaler = 2
//
#define SMART_PERIPHERAL_APB2_CLOCK SMART_SYSCLK
#endif

#define SMART_TIM_MAX_DEVS 8
#define SMART_TIMER_MAX_COUNTS 15

typedef union {
  float value;
  uint8_t datas[4];
} float_union;

#if !defined(UNUSED)
#define UNUSED(x) (void)(x)
#endif

#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) ||                       \
    defined(STM32F10X_MD) || defined(STM32F10X_MD_VL) ||                       \
    defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||                       \
    defined(STM32F10X_XL) || defined(STM32F10X_CL)
#define VREF_ADC 3.3f
#define SMART_ADC_USE_TEMP_SENSOR 1
#define SMART_ADC_MAX_NUM_CHANNELS 8
#define SMART_ADC_SUB_DRIVER_MAX_INSTANCES 5
#define SMART_ADC_ADCCLK RCC_PCLK2_Div8
#define SMART_ADC_MAX_OVERSAMPLING 36
#define SMART_INTERNAL_ADC_UPDATE_RATE 25.0f
#endif

#endif
