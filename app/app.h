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

#define SENSOR_CODE_DOOR_OPEN 0x0A   // 开门
#define SENSOR_CODE_DOOR_CLOSE 0x0E  // 关门
#define SENSOR_CODE_DOOR_TAMPER 0x07 // 防拆,预留
#define SENSOR_CODE_DOOR_LOWPWR 0x06 // 低压

// 遥控器 EV1527编码
#define SENSOR_CODE_REMOTE_ENARM 0x02
#define SENSOR_CODE_REMOTE_DISARM 0x01
#define SENSOR_CODE_REMOTE_HOMEARM 0x04
#define SENSOR_CODE_REMOTE_SOS 0x08

#define SENSOR_CODE_PIR 0x03
#define SENSOR_CODE_PIR_TAMPER 0x0B
#define SENSOR_CODE_PIR_LOWPWR 0x05

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

// 设置菜单列表ID
typedef enum {
  STG_MENU_MAIN_SETTING,     // 0
  STG_MENU_LEARNING_SENSOR,  // 1
  STG_MENU_DTC_LIST,         // 2
  STG_MENU_WIFI,             // 3
  STG_MENU_MACHINE_INFO,     // 4
  STG_MENU_FACTORY_SETTINGS, // 5
  STG_MENU_SUM               // 6
} STG_MENU_LIST;             // 普通菜单列表

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
  struct MODE_MENU *pChild;  // 指向子级选项
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
