#include "para.h"
#include "hal_eeprom.h"

Stu_DTC dERP[PARA_DTC_SUM];

static uint8_t ParaCheck(void);
static void CreatDtc(uint8_t n);

void ParaInit(void) {
  I2C_Read(STU_DEVICEPARA_OFFSET, (uint8_t *)(&dERP), sizeof(dERP));
  if (ParaCheck()) {
    FactoryReset();
  }
}

void FactoryReset(void) {
  for (int i = 0; i < PARA_DTC_SUM; i++) {
    dERP[i].ID = 0;
    dERP[i].Mark = 0;
    dERP[i].NameNum = 0;
    for (int j = 0; j < 16; j++) {
      dERP[i].Name[i] = 0;
    }
    dERP[i].DTCType = DTC_DOOR;
    dERP[i].ZoneType = ZONE_TYP_1ST;
    dERP[i].Code[0] = 0;
    dERP[i].Code[1] = 0;
    dERP[i].Code[2] = 0;
  }
  CreatDtc(20);
  I2C_PageWrite(STU_DEVICEPARA_OFFSET, (uint8_t *)(&dERP), sizeof(dERP));

  I2C_Read(STU_DEVICEPARA_OFFSET, (uint8_t *)(&dERP), sizeof(dERP));
}

static uint8_t ParaCheck(void) {
  uint8_t error = 0;

  if (dERP[0].ID != 1) {
    error = 1;
  }
  for (int i = 0; i < PARA_DTC_SUM; i++) {
    if (dERP[i].ID >= PARA_DTC_SUM) {
      error = 1;
    }
    if (dERP[i].Mark > 1) {
      error = 1;
    }
    if (dERP[i].NameNum > PARA_DTC_SUM) {
      error = 1;
    }

    if (dERP[i].DTCType >= DTC_TYP_SUM) {
      error = 1;
    }

    if (dERP[i].ZoneType >= ZONE_TYP_DEV_AT_SUM) {
      error = 1;
    }
  }
  return error;
}

void DelSpecifyDtc(Stu_DTC *pdPara) {
  uint8_t id;
  Stu_DTC *pDevPara;
  pDevPara = pdPara;
  id = pDevPara->ID - 1;
  pDevPara->ID = 0;
  pDevPara->Mark = 0;
  pDevPara->NameNum = 0;

  for (int i = 0; i < 16; i++) {
    pDevPara->Name[i] = 0;
  }
  pDevPara->DTCType = DTC_DOOR;
  pDevPara->ZoneType = ZONE_TYP_1ST;

  pDevPara->Code[0] = 0;
  pDevPara->Code[1] = 0;
  pDevPara->Code[2] = 0;
  I2C_PageWrite(STU_DEVICEPARA_OFFSET + id * STU_DTC_SIZE,
                (uint8_t *)(pDevPara), STU_DTC_SIZE); // 修改设备参数
  I2C_Read(STU_DEVICEPARA_OFFSET + id * STU_DTC_SIZE, (uint8_t *)&dERP[id],
           STU_DTC_SIZE);
}

uint8_t GetDtcNum(void) {
  uint8_t count = 0;
  uint8_t num = PARA_DTC_SUM;
  for (int i = 0; i < num; i++) {
    if (dERP[i].Mark) {
      count++;
    }
  }
  return count;
}

uint8_t AddDtc(Stu_DTC *pDevPara) {
  Stu_DTC DevPara;
  uint8_t NameStrIndex = 0;
  uint8_t Temp = 0;

  for (int i = 0; i < PARA_DTC_SUM; i++) {
    if (dERP[i].Mark && (dERP[i].Code[1] == pDevPara->Code[1]) &&
        (dERP[i].Code[2] == pDevPara->Code[2])) {
      return (i);
    }
  }
  for (int i = 0; i < PARA_DTC_SUM; i++) {
    if (!dERP[i].Mark) {
      DevPara.Name[0] = 'Z';
      DevPara.Name[1] = 'o';
      DevPara.Name[2] = 'n';
      DevPara.Name[3] = 'e';
      DevPara.Name[4] = '-';
      NameStrIndex = 5;
      Temp = i + 1;

      DevPara.Name[NameStrIndex++] = '0' + (Temp / 100);
      DevPara.Name[NameStrIndex++] = '0' + ((Temp % 100) / 10);
      DevPara.Name[NameStrIndex++] = '0' + ((Temp % 100) % 10);

      for (int j = NameStrIndex; j < 16; j++) {
        DevPara.Name[j] = 0;
      }
      DevPara.ID = i + 1;
      DevPara.Mark = 1;
      DevPara.NameNum = Temp;

      DevPara.DTCType = pDevPara->DTCType;
      DevPara.ZoneType = pDevPara->ZoneType;

      DevPara.Code[0] = pDevPara->Code[0];
      DevPara.Code[1] = pDevPara->Code[1];
      DevPara.Code[2] = pDevPara->Code[2];

      I2C_PageWrite(STU_DEVICEPARA_OFFSET + i * STU_DTC_SIZE,
                    (uint8_t *)(&DevPara), sizeof(DevPara));
      I2C_Read(STU_DEVICEPARA_OFFSET + i * STU_DTC_SIZE, (uint8_t *)&dERP[i],
               STU_DTC_SIZE);

      return (i);
    }
  }
  return 0xFF;
}

uint8_t DtcMatching(uint8_t *pCode) {
  for (int i = 0; i < PARA_DTC_SUM; i++) {
    if (dERP[i].Mark && (dERP[i].Code[1] == pCode[1]) &&
        (dERP[i].Code[2] == pCode[2])) // 判断探测器是否存在
    {
      return (dERP[i].ID);
    }
  }
  return 0xFF;
}

uint8_t CheckPresenceofDtc(uint8_t i) {
  uint8_t result;
  result = 0;
  if (i < PARA_DTC_SUM) // 防溢出检测
  {
    if (dERP[i].Mark) {
      result = 1;
    }
  }
  return result;
}

uint8_t GetDtcID(uint8_t idx) {
  uint8_t result;
  result = 0;
  if (idx < PARA_DTC_SUM) // 防溢出检测
  {
    result = dERP[idx].ID;
  }
  return result;
}

void GetDtcStu(Stu_DTC *pdDevPara, uint8_t idx) {
  uint8_t i;

  if (idx >= PARA_DTC_SUM) {
    return; // id异常
  }

  pdDevPara->ID = dERP[idx].ID;

  pdDevPara->Mark = dERP[idx].Mark;
  pdDevPara->NameNum = dERP[idx].NameNum;

  for (i = 0; i < 16; i++) {
    pdDevPara->Name[i] = dERP[idx].Name[i];
  }
  pdDevPara->DTCType = dERP[idx].DTCType;
  pdDevPara->ZoneType = dERP[idx].ZoneType;

  pdDevPara->Code[0] = dERP[idx].Code[0];
  pdDevPara->Code[1] = dERP[idx].Code[1];
  pdDevPara->Code[2] = dERP[idx].Code[2];
}

void SetDtcAbt(uint8_t id, Stu_DTC *psDevPara) {
  uint8_t i;
  if (id >= PARA_DTC_SUM) {
    return; // id异常
  }
  dERP[id].ID = psDevPara->ID;
  dERP[id].Mark = psDevPara->Mark;
  dERP[id].NameNum = psDevPara->NameNum;

  for (i = 0; i < 16; i++) {
    dERP[id].Name[i] = psDevPara->Name[i];
  }
  dERP[id].DTCType = psDevPara->DTCType;
  dERP[id].ZoneType = psDevPara->ZoneType;

  dERP[id].Code[0] = psDevPara->Code[0];
  dERP[id].Code[1] = psDevPara->Code[1];
  dERP[id].Code[2] = psDevPara->Code[2];

  I2C_PageWrite(STU_DEVICEPARA_OFFSET + id * STU_DTC_SIZE, (uint8_t *)psDevPara,
                STU_DTC_SIZE); // 新设备信息写入EEPROM
  I2C_Read(STU_DEVICEPARA_OFFSET + id * STU_DTC_SIZE, (uint8_t *)&dERP[id],
           STU_DTC_SIZE);
}

static void CreatDtc(uint8_t n) {
  uint8_t NameStrIndex, Temp, j;
  for (int i = 0; i < n; i++) {
    dERP[i].ID = i + 1;
    dERP[i].Mark = 1;

    dERP[i].Name[0] = 'Z';
    dERP[i].Name[1] = 'o';
    dERP[i].Name[2] = 'n';
    dERP[i].Name[3] = 'e';
    dERP[i].Name[4] = '-';
    NameStrIndex = 5;
    Temp = i + 1;
    dERP[i].NameNum = Temp;

    dERP[i].Name[NameStrIndex++] = '0' + (Temp / 100);
    dERP[i].Name[NameStrIndex++] = '0' + ((Temp % 100) / 10);
    dERP[i].Name[NameStrIndex++] = '0' + ((Temp % 100) % 10);

    for (j = NameStrIndex; j < 16; j++) {
      dERP[i].Name[j] = 0; // 把没用到的名字字节清零
    }

    dERP[i].DTCType = DTC_DOOR;
    dERP[i].ZoneType = ZONE_TYP_1ST;

    dERP[i].Code[0] = 0x0C;
    dERP[i].Code[1] = 0xBB;
    dERP[i].Code[2] = 0xAA;
  }
}