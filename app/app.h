/**
 ******************************************************************************
 * @file           : app.h
 * @author         : lanlisheng
 * @brief          : None
 * @attention      : None
 * @date           : 2023/11/29
 ******************************************************************************
 */

#ifndef APP_H_
#define APP_H_
#include "stdint.h"
// 菜单相关声明区域
typedef enum {
  SCREEN_CMD_NULL,    // 无用指令
  SCREEN_CMD_RESET,   // 重置屏显
  SCREEN_CMD_RECOVER, // 恢复原来显示
  SCREEN_CMD_UPDATE   // 更新原来显示
} SCREEN_CMD;

// 定义菜单的位置，主要用于超时退出判断
typedef enum {
  DESKTOP_MENU_POS,
  STG_MENU_POS,
  STG_WIFI_MENU_POS,
  STG_SUB_2_MENU_POS,
  STG_SUB_3_MENU_POS,
  STG_SUB_4_MENU_POS
} MODE_POS;

// 普通菜单列表
typedef enum {
  GNL_MENU_DESKTOP, // 桌面
  GNL_MENU_SUM,
} GENERAL_MENU_LIST; // 普通菜单列表

typedef struct MODE_MENU {
  uint8_t ID;           // 菜单唯一id号
  MODE_POS menuPos;     // 当前菜单的位置信息
  uint8_t *pModeType;   // 指向当前的模式类型
  void (*action)(void); // 当前模式下的响应函数
  SCREEN_CMD refreshScreenCmd;
  uint8_t reserved;          // 预留，方便参数传递
  uint8_t keyVal;            // 按键值，0xff代表无按键触发
  struct MODE_MENU *pLase;   // 指向上一个选项
  struct MODE_MENU *pNext;   // 指向下一个选项
  struct MODE_MENU *pParent; // 指向父级选项
  struct MODE_MENU *p;       // 指向子级选项
} stu_mode_menu;

typedef struct SYSTEM_TIME {
  unsigned short year;
  unsigned char mon;
  unsigned char day;
  unsigned char week;
  unsigned char hour;
  unsigned char min;
  unsigned char sec;
} stu_system_time;

void AppInit(void);
void AppProc(void);

#endif // _APP_H_
