#ifndef SMART_SYSTEM_H
#define SMART_SYSTEM_H

#define SMART_SYS_SERIAL_NUM_BINARY_LEN 12
#define SMART_SYS_SERIAL_NUM_ASCII_LEN (SMART_SYS_SERIAL_NUM_BINARY_LEN * 2)

extern void SMART_System_Init(void);
extern void SMART_SYS_Reset(void);
extern int
SMART_SYS_SerialNumberGet(char str[SMART_SYS_SERIAL_NUM_ASCII_LEN + 1]);

#endif
