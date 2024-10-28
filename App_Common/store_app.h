#ifndef _STORE_APP_H_
#define _STORE_APP_H_

#include <stdint.h>
#include <stdbool.h>
#include "systeminf.h"


bool WriteSysParam(SysParam_t *sysParam);
bool ReadSysParam(SysParam_t *sysParam);


bool SetGainBuffParam(uint32_t Buff, uint8_t  PhaseType);
bool SetOffsetBuffParam(uint32_t Buff, uint8_t PhaseType);

bool SetUgainParam(uint32_t Buff, uint8_t  PhaseType);
bool SetIgainParam(uint32_t Buff, uint8_t  PhaseType);
bool SetPgainParam(uint32_t Buff, uint8_t  PhaseType);

bool SetPhasegainParam(uint32_t Buff, uint8_t  PhaseType);


bool SetModbusAddrParam(uint8_t addr);
bool SetModbusBaudParam(uint32_t buad);

bool SetAnalogTypeParam(uint8_t Buff, uint8_t Route);


void InitSysParam(void);
void StoreTask(void );

/**
***********************************************************
* @brief ��ȡ�׻�����
* @param
* @return �׻�����
***********************************************************
*/

uint16_t  GetLightNumData(void);

/**
***********************************************************
* @brief ��ȡmodbus��ַ
* @param
* @return �׻�����
***********************************************************
*/

uint8_t  GetModbusAddrData(void);

/**
***********************************************************
* @brief ��ȡmodbus������
* @param
* @return 
***********************************************************
*/

uint32_t  GetModbusBaudData(void);

/**
***********************************************************
* @brief ��ȡeeprom����洢����ӿ����
* @param
* @return 
***********************************************************
*/

uint32_t  GetsysNum(void);


/**
***********************************************************
* @brief ��ȡ��������
* @param
* @return 
***********************************************************
*/
uint32_t*  GetGainData(void);

/**
***********************************************************
* @brief ��ȡƫ������
* @param
* @return 
***********************************************************
*/
uint32_t*  GetOffsetData(void);

/**
***********************************************************
* @brief ���ģ������������
* @param
* @return 
***********************************************************
*/
uint8_t*  GetAnalogTypeData(void);


#endif


