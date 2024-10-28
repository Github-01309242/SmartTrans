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
  
  // 打开串口GPIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = g_uartHwInfo.txPin ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(g_uartHwInfo.gpio , &GPIO_InitStructure);
  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = g_uartHwInfo.rxPin ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_uartHwInfo.gpio , &GPIO_InitStructure);
}

 /**
  * @brief  USART 配置,工作参数配置
  * @param  无
  * @retval 无
  */
static void UartInit(uint32_t baudRate)
{
  USART_InitTypeDef USART_InitStructure;	
  /*  使能时钟 */
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2  , ENABLE);
  /* 复位UART */
  USART_DeInit(g_uartHwInfo.uartNo );
	/* 配置串口的工作参数 */
	/* 配置波特率  */
	USART_InitStructure.USART_BaudRate = baudRate;
	/* 配置 针数据字长 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	/* 配置停止位  */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* 配置校验位  */
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	/* 配置硬件流控制  */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	/* 配置工作模式，收发一起  */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* 完成串口的初始化配置  */
	USART_Init(g_uartHwInfo.uartNo , &USART_InitStructure);
  /* 使能串口 */
  USART_Cmd(g_uartHwInfo.uartNo , ENABLE);	
  USART_ClearFlag(g_uartHwInfo.uartNo , USART_FLAG_TC);
}

/**
***********************************************************
* @brief USB转串口硬件初始化
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
* @brief printf函数默认打印输出到显示器，如果要输出到串口，
		 必须重新实现fputc函数，将输出指向串口，称为重定向
* @param
* @return 
***********************************************************
*/
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(g_uartHwInfo.uartNo , (uint8_t) ch);		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(g_uartHwInfo.uartNo , USART_FLAG_TXE) == RESET);		
		return (ch);
}


