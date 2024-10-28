
/*
 * 传感器监测应用程序
 */
#include "stm32f10x.h"
#include "sensor_drv.h"


void Sensor_Task(void )
{
  SensorDrvProc();

}
