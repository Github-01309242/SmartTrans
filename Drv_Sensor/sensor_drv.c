#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "sensor_drv.h"
#include "ht7036_drv.h"
#include "energy_meter_drv.h"


/**
***********************************************************
* @brief ������������ʼ��
* @param
* @return 
***********************************************************
*/
void SensorDrvInit(void)
{
  /***��������ʼ��****/
  ht7036cu_drv_init();
}

/**
***********************************************************
* @brief ��������ת������������
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
* @brief ��ȡ����������
* @param sensorData,��������������ݻ�д��ַ
* @return 
***********************************************************
*/
SensorData_str* GetSensorData(void )
{
   
  return ((SensorData_str*)ExportAll()) ;
}
