
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f10x.h"
#include "surgemdl_drv.h"
#include "RTT_Debug.h"
#include "sensor_drv.h"

#define MAX_BUF_SIZE           30
static uint8_t  g_recvDataBuf[MAX_BUF_SIZE];
static uint16_t g_recvDataLen;

typedef struct
{
	USART_TypeDef * uartNo;
	uint32_t rcuUart;
	uint32_t rcuGpio;
	GPIO_TypeDef* gpio;
	uint32_t txPin;
	uint32_t rxPin;
	uint8_t irq;
  uint32_t rcuDma;
  DMA_Channel_TypeDef* dmaCh;
  
} UartHwInfo_t;

static UartHwInfo_t g_uartHwInfo = {USART2, RCC_APB1Periph_USART2, RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_2, GPIO_Pin_3, 
                                    USART2_IRQn, RCC_AHBPeriph_DMA1, DMA1_Channel6};


static void SurgeGpioInit(void)
{
  RCC_APB2PeriphClockCmd(g_uartHwInfo.rcuGpio , ENABLE);
   
  GPIO_InitTypeDef GPIO_InitStructure;	
  
  /*��USART Tx����Ϊ���ù������� */
  GPIO_InitStructure.GPIO_Pin = g_uartHwInfo.txPin  ;				
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  
  GPIO_Init(g_uartHwInfo.gpio  ,&GPIO_InitStructure);
  /*Configure USART Rx as input floating*/
  GPIO_InitStructure.GPIO_Pin = g_uartHwInfo.rxPin   ;				
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	  
  GPIO_Init(g_uartHwInfo.gpio  ,&GPIO_InitStructure);
}

static void SurgeUartInit(uint32_t baudRate)
{
    /* ʹ��ʱ�� */
  RCC_APB1PeriphClockCmd( g_uartHwInfo.rcuUart  , ENABLE);
  
	USART_InitTypeDef USART_InitStructure;	
  
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
	/* ���ù���ģʽ��ֻ��  */
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	/* ��ɴ��ڵĳ�ʼ������  */
	USART_Init(g_uartHwInfo.uartNo , &USART_InitStructure);
  
  /* ���û����ָ����USART�ж�   �����ж� */
  USART_ITConfig(g_uartHwInfo.uartNo ,USART_IT_IDLE ,ENABLE );

  /*  ʹ�ܴ����ж� */
  NVIC_InitTypeDef NVIC_InitStructure; 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	 
	NVIC_InitStructure.NVIC_IRQChannel = g_uartHwInfo.irq ;			   	 //���ô���2�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	       //��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				       //�����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					         //ʹ��
  NVIC_Init(&NVIC_InitStructure);    
  /* ʹ�ܴ��� */
	USART_Cmd(g_uartHwInfo.uartNo , ENABLE);	
}


	//��ʼһ�� DMA ���䣡 
static void MX_DMA_Enable(DMA_Channel_TypeDef* DMA_CHx)
{
	DMA_Cmd(DMA_CHx, DISABLE ); //�ر� DMA1 ��ָʾ��ͨ�� 
	DMA_SetCurrDataCounter(DMA_CHx,MAX_BUF_SIZE);//���� DMA ����Ĵ�С
	DMA_Cmd(DMA_CHx, ENABLE); //ʹ�� DMA1 ��ָʾ��ͨ��
}

static void SurgeDmaInit(void )
{
  DMA_InitTypeDef  DMA_InitStructure;

  RCC_AHBPeriphClockCmd(g_uartHwInfo.rcuDma , ENABLE);	//ʹ��DMA����

  DMA_DeInit(g_uartHwInfo.dmaCh );  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;//DMA�����ַ  
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)g_recvDataBuf;//DMA �洢��0��ַ  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC ;//���赽�洢��ģʽ  
  DMA_InitStructure.DMA_BufferSize = MAX_BUF_SIZE;//���ݴ�����   
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ  
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ  
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ  
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ   
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�  
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
  DMA_Init(g_uartHwInfo.dmaCh , &DMA_InitStructure);//��ʼ��DMA Stream  

  /* ʹ��UART��������ʹ��DMA */ 
	USART_DMACmd(g_uartHwInfo.uartNo, USART_DMAReq_Rx, ENABLE);
  /* ʹ��DMAͨ����*/ 
  MX_DMA_Enable(g_uartHwInfo.dmaCh );
}  

/**
***********************************************************
* @brief ��ɳ�׻�������ģ��ת����Ӳ����ʼ��
* @param
* @return 
***********************************************************
*/
void SurgeMdlDrvInit(void)
{
  SurgeGpioInit();
  SurgeUartInit(115200);
  SurgeDmaInit();
}

/**
***********************************************************
* @brief ����2�жϷ�����
* @param
* @return 
***********************************************************
*/
void USART2_IRQHandler(void)
{
    if (RESET != USART_GetITStatus(g_uartHwInfo.uartNo , USART_IT_IDLE))
    {
      USART_ClearITPendingBit(g_uartHwInfo.uartNo, USART_IT_IDLE);  //��һ������ȡstat0�Ĵ��������IDLE��־λ
      USART_ReceiveData(g_uartHwInfo.uartNo );                      //�ڶ�������ȡ���ݼĴ��������IDLE��־λ
      g_recvDataLen = MAX_BUF_SIZE - DMA_GetCurrDataCounter( g_uartHwInfo.dmaCh);
      memset(&g_recvDataBuf[g_recvDataLen], 0, MAX_BUF_SIZE - g_recvDataLen);
       /* ʹ��DMAͨ����*/ 
      MX_DMA_Enable(g_uartHwInfo.dmaCh );
    }
}

/**
***********************************************************************
����ʽ��֡ͷ0  ������H ������L ������H  ������L  ����������ֵ  ����������ֵ
        0xAA   0x01    0x26    0x01     0x26     0xFFFFFFFF    0xFFFFFFFF
***********************************************************************
*/
#define FRAME_HEAD_0     0xAA  

static SurgemdlParam_t g_SurgeParam;
static bool g_refresh = false ;
void  SurgemdlDataPro(void)
{
  static uint32_t Positive_Peak ,Negative_Peak;
  
	if( *g_recvDataBuf != FRAME_HEAD_0)
  {
   g_refresh = false ; 
   return ;
  }
  else if (*g_recvDataBuf == FRAME_HEAD_0)
  {
    g_refresh = true ;
    g_SurgeParam.Num ++;
    if( g_SurgeParam.Num > Save_MAX)
    {
      g_SurgeParam.Num = 1 ;
    }    
    Positive_Peak = g_recvDataBuf[1]*256+g_recvDataBuf[2];
    Negative_Peak = g_recvDataBuf[3]*256+g_recvDataBuf[4];    
    g_SurgeParam.PeakCurrent =( Positive_Peak > Negative_Peak )? Positive_Peak:Negative_Peak;      
    
    *g_recvDataBuf = 0;
  }
}

bool GetRefreshflag(void )
{
 return g_refresh;
}  

/***********************************************************
* @brief Ԥ����ӿ����
* @param
* @return 
***********************************************************
*/
void PresetNum_Surge(uint16_t data)
{
   g_SurgeParam.Num  =  data ; 
}

bool GetSurgemdlData(SurgemdlParam_t*  SurgemdlPara)
{
  SurgemdlPara->Num = g_SurgeParam.Num;
  SurgemdlPara->PeakCurrent =  g_SurgeParam.PeakCurrent ;
  if(  SurgemdlPara->Num > Save_MAX)
  { 
   return false;
  }
  else 
  {
   return true;  
  }
}

void SurgemdlFuncTest(void )
{
  SurgemdlParam_t data;
  GetSurgemdlData (&data);
  DBG_log("surgenum is %d\n", data.Num );
  DBG_log("surge original peakcurrent is %d\n", data.PeakCurrent  );
}



