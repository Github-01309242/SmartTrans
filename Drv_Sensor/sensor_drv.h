#ifndef _SENSOR_DRV_H_
#define _SENSOR_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#include "energy_meter_drv.h"

typedef struct
{
  meter_data_str meter_data;
  int reserve[4];
} SensorData_str;

/**
***********************************************************
* @brief ������������ʼ��
* @param
* @return 
***********************************************************
*/
void SensorDrvInit(void);

/**
***********************************************************
* @brief ��ȡ����������
* @param sensorData,��������������ݻ�д��ַ
* @return bool����
***********************************************************
*/
SensorData_str* GetSensorData(void );

/**
***********************************************************
* @brief ��������ת������������
* @param
* @return 
***********************************************************
*/
void SensorDrvProc(void);

#endif
