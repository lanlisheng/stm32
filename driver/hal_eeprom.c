#include "hal_eeprom.h"
#include "stm32F10x.h"

// #define I2C_SCL_PORT GPIOB
// #define I2C_SCL_PIN GPIO_Pin_8

// #define I2C_SDA_PORT GPIOB
// #define I2C_SDA_PIN GPIO_Pin_9

#define EEPROM_PAGE_SIZE 32

void hal_I2C_SDA(uint8_t bVal) {
  if (bVal) {
    GPIO_SetBits(I2C_SDA_PORT, I2C_SDA_PIN);
  } else {
    GPIO_ResetBits(I2C_SDA_PORT, I2C_SDA_PIN);
  }
}

void hal_I2C_SCL(uint8_t bVal) {
  if (bVal) {
    GPIO_SetBits(I2C_SCL_PORT, I2C_SCL_PIN);
  } else {
    GPIO_ResetBits(I2C_SCL_PORT, I2C_SCL_PIN);
  }
}

static void hal_I2CConfig(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure I2C2 pins: PB8->SCL and PB9->SDA */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(I2C_SCL_PORT, &GPIO_InitStructure);

  /* init gpio */
  hal_I2C_SDA(1);
  hal_I2C_SCL(1);
}

void hal_eepromInit(void) { hal_I2CConfig(); }

void hal_I2C_SDA_IO_Set(uint8_t IOMode) {
  if (IOMode == 0) // 输出
  {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
  } else if (IOMode == 1) // 输入
  {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);
  }
}

uint8_t hal_I2C_SDA_INPUT(void) {
  return GPIO_ReadInputDataBit(I2C_SDA_PORT, I2C_SDA_PIN);
}

static void I2C_delay(uint16_t time) {
  uint16_t i = 50, j, c;
  c = time;
  for (j = 0; j < c; j++) {
    while (i) {
      i--;
    }
  }
}

static void I2C_Start(void) {
  hal_I2C_SDA(1);
  I2C_delay(1);
  hal_I2C_SCL(1);
  I2C_delay(1);
  hal_I2C_SDA(0);
  I2C_delay(1);
}

static void I2C_Stop(void) {
  hal_I2C_SDA(0);
  I2C_delay(1);
  hal_I2C_SCL(1);
  I2C_delay(1);
  hal_I2C_SDA(1);
  I2C_delay(1);
}

static void I2C_Ack(void) {
  hal_I2C_SCL(0);
  I2C_delay(1);
  hal_I2C_SDA(0);
  I2C_delay(1);
  hal_I2C_SCL(1);
  I2C_delay(1);
  hal_I2C_SCL(0);
  I2C_delay(1);
}

static void I2C_NoAck(void) {
  hal_I2C_SCL(0);
  I2C_delay(1);
  hal_I2C_SDA(1);
  I2C_delay(1);
  hal_I2C_SCL(1);
  I2C_delay(1);
  hal_I2C_SCL(0);
  I2C_delay(1);
}

static uint8_t I2C_WaitAck(void) {
  hal_I2C_SDA(1);
  hal_I2C_SDA_IO_Set(1);
  hal_I2C_SCL(1);
  I2C_delay(1);
  if (hal_I2C_SDA_INPUT()) {
    return 0;
  }
  hal_I2C_SCL(0);
  hal_I2C_SDA_IO_Set(0);
  I2C_delay(1);
  return 1;
}

static void I2C_SendByte(uint8_t SendByte) {
  uint8_t i = 0;
  uint8_t temp;
  temp = SendByte;
  for (i = 0; i < 8; i++) {
    hal_I2C_SCL(0);
    I2C_delay(1);
    if (temp & 0x80) {
      hal_I2C_SDA(1);
    } else {
      hal_I2C_SDA(0);
    }
    I2C_delay(1);
    hal_I2C_SCL(1);
    I2C_delay(1);
    temp <<= 1;
  }
  hal_I2C_SCL(0);
  I2C_delay(1);
  hal_I2C_SDA(1);
  I2C_delay(1);
}

// 字节写
static void I2C_WriteByte(uint16_t address, uint8_t dat) {
  I2C_Start();

  I2C_SendByte(0xA0);
  I2C_WaitAck();

  I2C_SendByte((address >> 8) & 0xFF);
  I2C_WaitAck();

  I2C_SendByte(address & 0xFF);
  I2C_WaitAck();

  I2C_SendByte(dat);
  I2C_WaitAck();

  I2C_Stop();
  I2C_delay(1000); // 延时10ms
}

static uint8_t I2C_ReceiveByte(void) {
  uint8_t i;
  uint8_t ReceiveByte = 0;

  hal_I2C_SCL(0);
  I2C_delay(1);
  hal_I2C_SDA(1);
  hal_I2C_SDA_IO_Set(1); // SDA设置成输入
  for (i = 0; i < 8; i++) {
    ReceiveByte <<= 1;
    I2C_delay(1);
    hal_I2C_SCL(1);
    I2C_delay(1);
    if (hal_I2C_SDA_INPUT()) {
      ReceiveByte |= 0x01;
    }
    hal_I2C_SCL(0);
  }
  hal_I2C_SDA_IO_Set(0); // SDA设置成输出
  I2C_delay(1);
  return ReceiveByte;
}

// 页写函数,有自动翻页功能,24C64一页32Byte,num最大可写65523个字节
void I2C_PageWrite(uint16_t address, uint8_t *pDat, uint16_t num) {
  uint8_t *pBuffer, j;
  uint16_t len, i, page, remainder, addr, temp;
  pBuffer = pDat;
  len = num;
  addr = address;
  temp = 0;
  if (addr % EEPROM_PAGE_SIZE) // 判断要写的地址
  {
    temp =
        EEPROM_PAGE_SIZE -
        (addr % EEPROM_PAGE_SIZE); // 32-7=25 //计算出当前地址还差多少字节满1页
    if (len <= temp) {
      temp = len;
    }
  }

  // 先填满写入地址页
  if (temp) {
    I2C_Start();

    I2C_SendByte(0xA0);
    I2C_WaitAck();

    I2C_SendByte((addr >> 8) & 0xFF);
    I2C_WaitAck();

    I2C_SendByte(addr & 0xFF);
    I2C_WaitAck();

    for (j = 0; j < temp; j++) {
      I2C_SendByte(pBuffer[j]);
      I2C_WaitAck();
    }
    I2C_Stop();
    I2C_delay(20000);
  }

  len -= temp;
  addr += temp; // 地址加上已经写入的字节

  page = len / EEPROM_PAGE_SIZE;
  remainder = len % EEPROM_PAGE_SIZE;
  for (i = 0; i < page; i++) {
    I2C_Start();
    I2C_SendByte(0xA0);
    I2C_WaitAck();

    I2C_SendByte((addr >> 8) & 0xFF);
    I2C_WaitAck();

    I2C_SendByte(addr & 0xFF);
    I2C_WaitAck();

    for (j = 0; j < EEPROM_PAGE_SIZE; j++) {
      I2C_SendByte(pBuffer[temp + j]);
      I2C_WaitAck();
    }
    I2C_Stop();
    addr += EEPROM_PAGE_SIZE;
    //	temp += EEPROM_PAGE_SIZE;
    I2C_delay(20000);
  }

  if (remainder) {
    I2C_Start();
    I2C_SendByte(0xA0);
    I2C_WaitAck();

    I2C_SendByte((addr >> 8) & 0xFF);
    I2C_WaitAck();

    I2C_SendByte(addr & 0xFF);
    I2C_WaitAck();

    for (j = 0; j < remainder; j++) {
      I2C_SendByte(pBuffer[temp + j]);
      I2C_WaitAck();
    }
    I2C_Stop();
    I2C_delay(20000);
  }
}

// 读1个字节
uint8_t I2C_ReadByte(uint16_t address) {
  uint8_t dat;
  I2C_Start();
  I2C_SendByte(0xA0);
  I2C_WaitAck();

  I2C_SendByte((address >> 8) & 0xFF);
  I2C_WaitAck();

  I2C_SendByte(address & 0xFF);
  I2C_WaitAck();

  I2C_Start();
  I2C_SendByte(0xA1);
  I2C_WaitAck();

  dat = I2C_ReceiveByte();
  I2C_NoAck();
  I2C_Stop();
  return dat;
}

// 连续读多个字节
void I2C_Read(unsigned short address, unsigned char *pBuffer,
              unsigned short len) {
  unsigned short length;
  length = len;
  I2C_Start();
  I2C_SendByte(0xA0);
  I2C_WaitAck();

  I2C_SendByte((address >> 8) & 0xFF);
  I2C_WaitAck();

  I2C_SendByte(address & 0xFF);
  I2C_WaitAck();

  I2C_Start();
  I2C_SendByte(0xA1);
  I2C_WaitAck();

  // dat = I2C_ReceiveByte();
  while (length) {
    *pBuffer = I2C_ReceiveByte();
    if (length == 1)
      I2C_NoAck();
    else
      I2C_Ack();
    pBuffer++;
    length--;
  }

  I2C_Stop();
}