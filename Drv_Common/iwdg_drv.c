#include <stdint.h>
#include "stm32f10x.h"


/**
***********************************************************
* @brief �������Ź���ʼ������ʱʱ��2S
* @param
* @return 
***********************************************************
*/
void IwdgDrvInit(void)
{
  /*ʹ��IWDG_PR��IWDG_RLR�����Ĵ���*/
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  /*����Ԥ��Ƶ��*/
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  /*������װ�ؼĴ���*/
  IWDG_SetReload(299);    //1s �� (1/40000)*256*(299+1)    ���Ź�ι��ʱ�䶨ʱ1.92s����
  /* �������Ź� */
  IWDG_Enable();
}

/**
***********************************************************
* @brief ι��
* @param
* @return 
***********************************************************
*/

void FeedDog(void)
{
	IWDG_ReloadCounter();
}

