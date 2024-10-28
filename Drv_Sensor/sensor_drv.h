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
* @brief 传感器驱动初始化
* @param
* @return 
***********************************************************
*/
void SensorDrvInit(void);

/**
***********************************************************
* @brief 获取传感器数据
* @param sensorData,输出，传感器数据回写地址
* @return bool类型
***********************************************************
*/
SensorData_str* GetSensorData(void );

/**
***********************************************************
* @brief 触发驱动转换传感器数据
* @param
* @return 
***********************************************************
*/
void SensorDrvProc(void);

#endif
