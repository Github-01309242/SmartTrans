#include "stm32f10x.h"

void InitIrqAfterBoot(void)
{
  NVIC_SetVectorTable(FLASH_BASE, 0x3000);
	__enable_irq();
}

void ResetToBoot(void)
{
	__disable_irq();    //�ر������ж�
	NVIC_SystemReset(); //��λ��������ҪһЩִ�е�ʱ��
}
