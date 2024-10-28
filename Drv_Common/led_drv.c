#include <stdint.h>
#include "stm32f10x.h"
#include "delay.h"
#include "iwdg_drv.h"

typedef struct
{
  uint32_t rcu;
	GPIO_TypeDef* gpio;
	uint32_t pin;
} Led_GPIO_t;

static Led_GPIO_t g_gpioList[] =
{
	{RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_12},
  {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_3},
  {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_4},
  {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_5},
};

#define LED_NUM_MAX (sizeof(g_gpioList) / sizeof(g_gpioList[0]))

/**
***********************************************************
* @brief LED硬件初始化
* @param
* @return 
***********************************************************
*/

#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled*/

void LedDrvInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	  
	
  /**1、打开复用时钟：**/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  /**2、对PB3，PB4，PA15三个引脚进行重映射：**/
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*使能SWD 禁用JTAG*/
  
  for (uint8_t i = 0; i < LED_NUM_MAX; i++)
	{
   RCC_APB2PeriphClockCmd(g_gpioList[i].rcu , ENABLE);
    
   GPIO_InitStructure.GPIO_Pin = g_gpioList[i].pin ;				
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	  
   GPIO_Init(g_gpioList[i].gpio ,&GPIO_InitStructure);
   GPIO_ResetBits(g_gpioList[i].gpio ,g_gpioList[i].pin );
	}
}

/**
***********************************************************
* @brief 点亮LED
* @param ledNo，LED标号，0~2
* @return 
***********************************************************
*/
void TurnOnLed(uint8_t ledNo)
{
	if (ledNo >= LED_NUM_MAX)
	{
		return;
	}
	GPIO_ResetBits(g_gpioList[ledNo].gpio, g_gpioList[ledNo].pin);
}

/**
***********************************************************
* @brief 熄灭LED
* @param ledNo，LED标号，0~2
* @return 
***********************************************************
*/
void TurnOffLed(uint8_t ledNo)
{
	if (ledNo >= LED_NUM_MAX)
	{
		return;
	}
	GPIO_SetBits(g_gpioList[ledNo].gpio, g_gpioList[ledNo].pin);
}

/**
***********************************************************
* @brief LED状态取反
* @param ledNo，LED标号，0~2
* @return 
***********************************************************
*/
void ToggleLed(uint8_t ledNo)
{
	if (ledNo >= LED_NUM_MAX)
	{
		return;
	}
	uint8_t  bit_state;
	bit_state = GPIO_ReadInputDataBit(g_gpioList[ledNo].gpio, g_gpioList[ledNo].pin);
	bit_state = (1 - bit_state);
	GPIO_WriteBit(g_gpioList[ledNo].gpio, g_gpioList[ledNo].pin, (BitAction )bit_state);
}


void LED_Test(void )
{
  ToggleLed (1);
  FeedDog();
}



 
  
