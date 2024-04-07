#ifndef PARA_H
#define PARA_H

#include <stdint.h>

#define PARA_DTC_SUM 20

typedef enum {
  DTC_DOOR,
  DTC_PIR_MOTION,
  DTC_REMOTE,
  DTC_TYP_SUM,
} DTC_TYPE_TYPEDEF;

typedef enum {
  ZONE_TYP_24HOURS,
  ZONE_TYP_1ST,
  ZONE_TYP_2ND,
  ZONE_TYP_DEV_AT_SUM,
} ZONE_TYPED_TYPEDEF;

typedef struct {
  uint8_t ID;
  uint8_t Mark;
  uint8_t NameNum;
  uint8_t Name[16];
  DTC_TYPE_TYPEDEF DTCType;
  ZONE_TYPED_TYPEDEF ZoneType;
  uint8_t Code[3];
} Stu_DTC;

#define STU_DTC_SIZE sizeof(Stu_DTC)
#define STU_SYSTEMPARA_SIZE sizeof(SystemPara_InitTypeDef)
#define STU_DTC_SIZE sizeof(Stu_DTC)

#define STU_DEVICEPARA_OFFSET 0

#endif