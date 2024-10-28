
#include "stm32f10x.h"
#include <stdint.h>
#include <stdlib.h>

static volatile uint64_t g_sysRunTime = 0;

static void (*g_pTaskScheduleFunc)(void);          // ����ָ�룬����������ȵĺ�����ַ

/**
***********************************************************
* @brief ע��������Ȼص�����
* @param pFunc, ����ص�������ַ
* @return 
***********************************************************
*/
void TaskScheduleCbReg(void (*pFunc)(void))
{
	g_pTaskScheduleFunc = pFunc;
}

/**
***********************************************************
* @brief systick��ʼ��
* @param
* @return 
***********************************************************
*/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemCoreClock / 1000))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while (1);
	}
}

/**
***********************************************************
* @brief ��ʱ�жϷ�������1ms����һ���ж�
* @param
* @return 
***********************************************************
*/
void SysTick_Handler(void)
{	
	g_sysRunTime++;
	if (g_pTaskScheduleFunc == NULL)
	{
		return;
	}
	g_pTaskScheduleFunc();

}

/**
***********************************************************
* @brief ��ȡϵͳ����ʱ��
* @param
* @return ��1msΪ��λ
***********************************************************
*/
uint64_t GetSysRunTime(void)
{
	return g_sysRunTime;
}

/*********************************************END OF FILE**********************/
