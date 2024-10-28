
#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "systick_drv.h"
#include "delay.h"
#include "usb2Com_drv.h"
#include "led_drv.h"
#include "iwdg_drv.h"
#include "key_drv.h"
#include "switch_drv.h"
#include "eeprom_drv.h"
#include "max7219_drv.h"
#include "timing_drv.h"

#include "lcd12864_drv.h"
#include "gp8202as_drv.h"
#include "sensor_drv.h"

#include "store_app.h"
#include "sensor_app.h"
#include "modbus_app.h"
#include "analogout_app.h"
#include "hmi_app.h"


typedef struct
{
	uint8_t run;                // ���ȱ�־��1�����ȣ�0������
	uint16_t timCount;          // ʱ��Ƭ����ֵ
	uint16_t timRload;          // ʱ��Ƭ����ֵ
	void (*pTaskFuncCb)(void);  // ����ָ���������������ҵ����ģ�麯����ַ
} TaskComps_t;

static TaskComps_t g_taskComps[] = 
{
  {0, 5,    5,     Hmipass_Task  },
  {0, 10,   10,    ModbusTask  },
  {0, 1000, 1000,  Sensor_Task  },
  {0, 100,  100,   AnalogOutTask   },
  {0, 3000, 3000,  LED_Test   }
	/* ����ҵ����ģ�� */
};

#define TASK_NUM_MAX   (sizeof(g_taskComps) / sizeof(g_taskComps[0]))

static void TaskHandler(void)
{
	for (uint8_t i = 0; i < TASK_NUM_MAX; i++)
	{
		if (g_taskComps[i].run)                // �ж�ʱ��Ƭ��־
		{
			g_taskComps[i].run = 0;              // ��־����
			g_taskComps[i].pTaskFuncCb();        // ִ�е���ҵ����ģ��
		}
	}
}

/**
***********************************************************
* @brief �ڶ�ʱ���жϷ������б���ӵ��ã�����ʱ��Ƭ��ǣ�
         ��Ҫ��ʱ��1ms����1���ж�
* @param
* @return 
***********************************************************
*/
static void TaskScheduleCb(void)
{
	for (uint8_t i = 0; i < TASK_NUM_MAX; i++)
	{
		if (g_taskComps[i].timCount)
		{
			g_taskComps[i].timCount--;
			if (g_taskComps[i].timCount == 0)
			{
				g_taskComps[i].run = 1;
				g_taskComps[i].timCount = g_taskComps[i].timRload;
			}
		}
	}
}


static void DrvInit(void)
{
  DelayInit ();
  Usb2ComDrvInit();
  printf("This is UART printf test!\n");
  LedDrvInit (); 
  Max7219Init ();  
  TimingDrvInit();
  KeyDrvInit ();
  SwitchDrvInit ();
  EepromDrvInit ();
  Gp8202asDrvInit();
  IwdgDrvInit();
  SensorDrvInit ();  
  
}

static void AppInit(void)
{
	TaskScheduleCbReg(TaskScheduleCb);  
  InitSysParam ();
  ModbusAppInit();
}


int main(void)
{ 
  SysTick_Init();      
  DrvInit();

//  EepromErase();  
  
  AppInit(); 

  DelayNms (1000);  

  while(1)
  {		
    TaskHandler(); 
  }
}


/*********************************************END OF FILE**********************/
