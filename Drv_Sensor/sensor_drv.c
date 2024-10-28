#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "sensor_drv.h"
#include "ht7036_drv.h"
#include "energy_meter_drv.h"


/**
***********************************************************
* @brief 传感器驱动初始化
* @param
* @return 
***********************************************************
*/
void SensorDrvInit(void)
{
  /***传感器初始化****/
  ht7036cu_drv_init();
}

/**
***********************************************************
* @brief 触发驱动转换传感器数据
* @param
* @return 
***********************************************************
*/
void SensorDrvProc(void)
{
   ElectricCollectionProc();
}

/**
***********************************************************
* @brief 获取传感器数据
* @param sensorData,输出，传感器数据回写地址
* @return 
***********************************************************
*/
SensorData_str* GetSensorData(void )
{
   
  return ((SensorData_str*)ExportAll()) ;
}
