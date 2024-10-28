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

#include "port.h"
#include "stm32f10x.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

typedef struct
{
	USART_TypeDef* uartNo;
	GPIO_TypeDef* gpio;
	uint32_t txPin;
	uint32_t rxPin;
	uint8_t irq;
} UartHwInfo_t;

static UartHwInfo_t g_uartHwInfo = {USART1, GPIOA, GPIO_Pin_9, GPIO_Pin_10, USART1_IRQn};

static void GpioInit(void )
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  // �򿪴���GPIO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

 /**
  * @brief  RS485_USART ����,������������
  * @param  ��
  * @retval ��
  */
static void UartInit(uint32_t baudRate)
{
  USART_InitTypeDef USART_InitStructure;	
  /*  ʹ��ʱ�� */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1  , ENABLE);
  /* ��λUART */
  USART_DeInit(g_uartHwInfo.uartNo );
	/* ���ô��ڵĹ������� */
	/* ���ò�����  */
	USART_InitStructure.USART_BaudRate = baudRate;
	/* ���� �������ֳ� */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	/* ����ֹͣλ  */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* ����У��λ  */
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	/* ����Ӳ��������  */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	/* ���ù���ģʽ���շ�һ��  */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* ��ɴ��ڵĳ�ʼ������  */
	USART_Init(USART1, &USART_InitStructure);

  /*  ʹ�ܴ����ж� */
  NVIC_InitTypeDef NVIC_InitStructure; 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			   	 //���ô���1�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				 //�����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					   //ʹ��
  NVIC_Init(&NVIC_InitStructure);  
  /* ʹ�ܴ��� */
	USART_Cmd(USART1, ENABLE);	
	
}

static void SwitchInit(void)
{
   GPIO_InitTypeDef GPIO_InitStructure; 
  // �򿪴���GPIO��ʱ��
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
   /* ������� */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;				
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	  
   GPIO_Init(GPIOA ,&GPIO_InitStructure);                               
}

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	 if(TRUE == xRxEnable)
    {
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ʹ�ܽ����ж�
		GPIO_ResetBits(GPIOA, GPIO_Pin_8 ); 
    }
    else
    {
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//ʧ�ܽ����ж�
		GPIO_SetBits(GPIOA, GPIO_Pin_8 );    
    }
    
   if(TRUE == xTxEnable)
    {
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);//ʹ�ܷ����ж�
		GPIO_SetBits(GPIOA, GPIO_Pin_8 );   
    }
    else
    {
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);//ʧ�ܷ����ж�
		GPIO_ResetBits(GPIOA, GPIO_Pin_8 );   
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    (void)ucPORT;
    (void)ucDataBits;
    (void)eParity;
    SwitchInit();
    GpioInit();
    UartInit(ulBaudRate);
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    USART_SendData(g_uartHwInfo.uartNo , ucByte);
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = USART_ReceiveData(g_uartHwInfo.uartNo );
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

void USART1_IRQHandler(void)
{
    //�жϽ��ջ��Ƿ����ж�
    if (RESET != USART_GetITStatus(g_uartHwInfo.uartNo , USART_IT_RXNE))
    {
        //��������ж�
        prvvUARTRxISR();
        USART_ClearITPendingBit(g_uartHwInfo.uartNo, USART_IT_RXNE);
    }
    if(RESET != USART_GetITStatus(g_uartHwInfo.uartNo, USART_IT_TC))
    {
        //��������ж�
        prvvUARTTxReadyISR();
        USART_ClearITPendingBit(g_uartHwInfo.uartNo, USART_IT_TC);
    }
}
