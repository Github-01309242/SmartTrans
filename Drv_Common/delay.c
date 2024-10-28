#include <stdint.h>
#include "stm32f10x.h"

#define  DEM_CR      *(volatile u32 *)0xE000EDFC
 
#define  DWT_CR      *(volatile u32 *)0xE0001000
 
#define  DWT_CYCCNT  *(volatile u32 *)0xE0001004
 
#define  DEM_CR_TRCENA                   (1 << 24)
 
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/**
***********************************************************
* @brief DWT��ʼ������
* @param
* @return 
***********************************************************
*/
void DelayInit(void) 
{
    /* ��ʹ��DWT���裬���ں˵��ԼĴ���DEM_CR��λ24���ƣ�д1ʹ�� */
    DEM_CR  |=  (uint32_t )DEM_CR_TRCENA; 
    /* ʹ��CYCCNT�Ĵ���֮ǰ��Ҫ����0*/
    DWT_CYCCNT = 0;
    /* ʹ��CYCCNT�Ĵ�������DWT_CTRL��λ0���ƣ�д1ʹ�� */
    DWT_CR  |=  (uint32_t )DWT_CR_CYCCNTENA;


}


/**
***********************************************************
* @brief ΢�뼶��ʱ����
* @param nUs�������ʱʱ��( 2^32 / �ں���Ƶ ) * 10^6 us 
* @return 
***********************************************************
*/
void DelayNus(uint32_t uSec)
{

  int ticks_start, ticks_end, ticks_delay;
  
  ticks_start = DWT_CYCCNT;
   
  ticks_delay = ( uSec * ( 72000000 / (1000*1000) ) ); // ��΢��������ɵδ���          
  
  ticks_end = ticks_start + ticks_delay;
  
  if ( ticks_end > ticks_start )
  {
    while( DWT_CYCCNT < ticks_end );
  }
  else // �����������ת
  {
    while( DWT_CYCCNT >= ticks_end ); // ��ת���ֵ�����ticks_endС
    while( DWT_CYCCNT < ticks_end );
  }
}

/**
***********************************************************
* @brief ���뼶��ʱ����
* @param nMs����ʱʱ��n����
* @return 
***********************************************************
*/
void DelayNms(uint32_t nMs)
{
	for (uint32_t i = 0; i < nMs; i++)
	{
		DelayNus(1000);
	}
}


