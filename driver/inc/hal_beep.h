#ifndef HAL_BEEP_H
#define HAL_BEEP_H

void hal_BeepInit(void);
void hal_BeepProc(void);
void hal_BeepPwmCtrl(unsigned char cmd);
#endif
