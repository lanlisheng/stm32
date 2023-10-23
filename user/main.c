// #include "stm32f4xx.h"
#include "board_init.h"
#include "smart.h"
#include "smart_initcall.h"
#include "smart_system.h"
#include "stm32f10x.h"
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount) {
  while (nCount--) {
  }
}
int main(void) {
  static uint32_t lastTimer0 = 0;
  static uint32_t Timer_cnt = 0;
  // ϵͳʱ�ӡ���ʱ���ж����������س�ʼ��
  SMART_System_Init();
  // ����Ӳ����ʼ�����磺ADC��I2C��SPI��FLASH��OLED����������
  SMART_Board_Init();
  // ����ȫ�����ݶ����ʼ�����磺�������ݡ�ҡ�����ݡ���������
  // DATA_INITIALISE_ALL();
  // ����ģ���ʼ��
  // MODULE_INITIALISE_ALL();
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // GPIO_ResetBits(GPIOA, GPIO_Pin_1);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // GPIO_ResetBits(GPIOB, GPIO_Pin_0);
  while (1) {
    if (lastTimer0 == 0)
      lastTimer0 = SMART_Get_SystemTime();
    if ((SMART_Get_SystemTime() - lastTimer0) > 1000) {
      lastTimer0 = SMART_Get_SystemTime();
      Timer_cnt++;
      if (Timer_cnt == 1) {
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
      } else if (Timer_cnt == 2) {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
      }
      if (Timer_cnt == 3) {
        Timer_cnt = 0;
      }
    }
    // ��������ģ���task
    // MODULE_LOOPER_ALL();
  }
  return 0;
}

// int main() {
//   GPIO_InitTypeDef GPIO_InitStructure;
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//   // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//   GPIO_Init(GPIOA, &GPIO_InitStructure);
//   // GPIO_ResetBits(GPIOA, GPIO_Pin_1);

//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//   GPIO_Init(GPIOB, &GPIO_InitStructure);
//   // GPIO_ResetBits(GPIOB, GPIO_Pin_0);

//   while (1) {
//     GPIO_SetBits(GPIOA, GPIO_Pin_1);
//     GPIO_SetBits(GPIOB, GPIO_Pin_0);
//     Delay(0x0FFFFF);
//     GPIO_ResetBits(GPIOA, GPIO_Pin_1);
//     GPIO_ResetBits(GPIOB, GPIO_Pin_0);
//     Delay(0x0FFFFF);
//   }

//   return 0;
// }