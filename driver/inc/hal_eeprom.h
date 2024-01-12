#ifndef HAL_EEPROM_H
#define HAL_EEPROM_H

#include <stdint.h>

#define I2C_SCL_PORT GPIOB
#define I2C_SCL_PIN GPIO_Pin_8

#define I2C_SDA_PORT GPIOB
#define I2C_SDA_PIN GPIO_Pin_9

void hal_eepromInit(void);
void I2C_Read(uint16_t address, uint8_t *pBuffer, uint16_t len);
void I2C_PageWrite(uint16_t address, uint8_t *pDat, uint16_t num);

#endif