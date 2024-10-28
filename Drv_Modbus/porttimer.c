/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "stm32f10x.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{

  NVIC_InitTypeDef NVIC_InitStructure;
 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	 
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* ʹ�ܶ�ʱ��ʱ�ӣ�*/
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5  , ENABLE);   
  /* ��λ��ʱ����*/
	TIM_DeInit(TIM5);
  /* �����Զ���װ��ֵ��*/ 
	TIM_TimeBaseStructure.TIM_Period = usTim1Timerout50us - 1;
  /* ����Ԥ��Ƶ��ֵ��*/ 
  // Ƶ��72MHZ / 3600 = 20khz,��Ӧ����50us
	TIM_TimeBaseStructure.TIM_Prescaler = 3600 - 1; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
  /* ʹ�ܶ�ʱ���ļ��������жϣ�*/
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);	

  /* ʹ�ܶ�ʱ���жϺ����ȼ���*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;            
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           
	NVIC_Init(&NVIC_InitStructure);              
  /* ʹ�ܶ�ʱ����*/ 
  //	TIM_Cmd(TIM5, ENABLE);	
  return TRUE ;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
  /* ���㶨ʱ�� */  
  TIM_SetCounter(TIM5 , 0);
  /* �����ʱ�������ж� */
  TIM_ClearFlag(TIM5,TIM_FLAG_Update);
  /* ʹ�ܶ�ʱ�������ж� */
  TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);	
  /* ʹ�ܶ�ʱ����*/ 
 	TIM_Cmd(TIM5, ENABLE);
  
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
  /* ���㶨ʱ�� */  
  TIM_SetCounter(TIM5 , 0);
  /* �����ʱ�������ж� */
  TIM_ClearFlag(TIM5,TIM_FLAG_Update);
  /* ���ö�ʱ�������ж� */
  TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE);	
  /* ʧ�ܶ�ʱ��*/ 
 	TIM_Cmd(TIM5, DISABLE);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

void TIM5_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)
   {
		 TIM_ClearITPendingBit(TIM5 , TIM_IT_Update); 
		 TIM_SetCounter(TIM5 , 0);
     prvvTIMERExpiredISR();
	 } 		 
}
