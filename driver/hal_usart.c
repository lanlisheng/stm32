#include "hal_usart.h"
#include "OS_System.h"
#include "stm32f10x.h"

uint8_t DebugTxDMAMapBuff[DEBUG_TXBUFF_SIZE_MAX];
volatile uint8_t DebugIsBusy;
volatile Queue256 DebugTxMsg;

static void hal_usart_Config(void);
static void hal_DebugSendByte(uint8_t Dat);
static void hal_DMA_Config(void);
static void hal_DMAC4_Enable(uint32_t size);
static void hal_debugProc(void);

void hal_UsartInit(void) { hal_usart_Config(); }

void hal_UsartProc(void) {}

static void hal_debugProc(void) {
  if (DebugIsBusy) {
    return;
  }
  uint8_t len = QueueDataLen(DebugTxMsg);
  for (int i = 0; i < len; i++) {
    QueueDataOut(DebugTxMsg, &DebugTxDMAMapBuff[i]);
  }
  if (len) {
    hal_DMAC4_Enable(len);
    DebugIsBusy = 1;
  }
}

static void hal_usart_Config(void) {
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  USART_InitTypeDef USART_InitStructure = {0};
  NVIC_InitTypeDef NVIC_InitStructure = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl =
      USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  USART_Cmd(USART1, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl =
      USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  // USART_DMACmd(WIFI_USART_PORT,USART_DMAReq_Tx,ENABLE);
  USART_Cmd(USART2, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void hal_DMA_Config(void) {
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 启动DMA时钟

  DMA_DeInit(DMA1_Channel4);

  // USART1发送DMA
  DMA_InitStructure.DMA_PeripheralBaseAddr =
      (unsigned long)(&USART1->DR); // 数据传输目标地址
  DMA_InitStructure.DMA_MemoryBaseAddr =
      (unsigned long)DebugTxDMAMapBuff;              // 数据缓存地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // 外设作为数据传输的目的地
  DMA_InitStructure.DMA_BufferSize = DEBUG_TXBUFF_SIZE_MAX; // 发送Buff数据大小
  DMA_InitStructure.DMA_PeripheralInc =
      DMA_PeripheralInc_Disable; // 设置外设地址是否递增
  DMA_InitStructure.DMA_MemoryInc =
      DMA_MemoryInc_Enable; // 设置内存地址是否递增
  DMA_InitStructure.DMA_PeripheralDataSize =
      DMA_PeripheralDataSize_Byte; // 外设数据宽度为8位
  DMA_InitStructure.DMA_MemoryDataSize =
      DMA_MemoryDataSize_Byte;                  // 内存数据宽度为8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // 普通缓存模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 高优先级
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 禁止DMA2个内存相互访问
  DMA_Init(DMA1_Channel4, &DMA_InitStructure); // 初始化DMA, USART1在DMA1的通道4
  DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); // 添加USART_TX中断配置

  // USART1发送DMA中断
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

// 启动debug usart1的DMA发送函数,size-需要发送的数据大小
static void hal_DMAC4_Enable(uint32_t size) {
  DMA1_Channel4->CCR &= ~(1 << 0);
  DMA1_Channel4->CNDTR = size;
  DMA1_Channel4->CCR |= 1 << 0;
}

// debug usart1通过DMA发送完毕中断回调函数
void DMA1_Channel4_IRQHandler(void) {
  if (DMA_GetITStatus(DMA1_IT_TC4) != RESET) {
    DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DebugIsBusy = 0;
  }
}

static void hal_DebugSendByte(uint8_t Dat) {
  USART_SendData(USART1, Dat);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

static void hal_SendByte(uint8_t data) {
  // USART_SendData(WIFI_USART_PORT,data);
  USART_SendData(USART1, (uint8_t)data);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    ;
}

void USART1_IRQHandler(void) {

  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
    uint8_t dat = USART_ReceiveData(USART1);
    // hal_DebugSendByte(dat);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
  if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
    USART_ClearITPendingBit(USART1, USART_IT_TXE);
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  }
}

void USART2_IRQHandler(void) {
  unsigned char dat;
  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
    dat = USART_ReceiveData(USART2);
    hal_DebugSendByte(dat);

    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }

  if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
    USART_ClearITPendingBit(USART2, USART_IT_TXE);
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  }
}
