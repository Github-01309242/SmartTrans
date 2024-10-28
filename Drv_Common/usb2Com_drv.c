#include "stm32f10x.h"
#include <stdio.h>

typedef struct
{
	USART_TypeDef* uartNo;
	GPIO_TypeDef* gpio;
	uint32_t txPin;
	uint32_t rxPin;
	uint8_t irq;
} UartHwInfo_t;

static UartHwInfo_t g_uartHwInfo = {USART2, GPIOA, GPIO_Pin_2, GPIO_Pin_3, USART2_IRQn};

static void GpioInit(void )
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  // �򿪴���GPIO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = g_uartHwInfo.txPin ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(g_uartHwInfo.gpio , &GPIO_InitStructure);
  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = g_uartHwInfo.rxPin ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_uartHwInfo.gpio , &GPIO_InitStructure);
}

 /**
  * @brief  USART ����,������������
  * @param  ��
  * @retval ��
  */
static void UartInit(uint32_t baudRate)
{
  USART_InitTypeDef USART_InitStructure;	
  /*  ʹ��ʱ�� */
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2  , ENABLE);
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
	USART_Init(g_uartHwInfo.uartNo , &USART_InitStructure);
  /* ʹ�ܴ��� */
  USART_Cmd(g_uartHwInfo.uartNo , ENABLE);	
  USART_ClearFlag(g_uartHwInfo.uartNo , USART_FLAG_TC);
}

/**
***********************************************************
* @brief USBת����Ӳ����ʼ��
* @param
* @return 
***********************************************************
*/
void Usb2ComDrvInit(void)
{
	GpioInit();
	UartInit(115200);
}

/**
***********************************************************
* @brief printf����Ĭ�ϴ�ӡ�������ʾ�������Ҫ��������ڣ�
		 ��������ʵ��fputc�����������ָ�򴮿ڣ���Ϊ�ض���
* @param
* @return 
***********************************************************
*/
///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(g_uartHwInfo.uartNo , (uint8_t) ch);		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(g_uartHwInfo.uartNo , USART_FLAG_TXE) == RESET);		
		return (ch);
}


