#include <stdint.h>
#include "stm32f10x.h"
#include "systick_drv.h"


typedef struct
{
	uint32_t rcu;
	GPIO_TypeDef * gpio;
	uint16_t pin;
} Switch_GPIO_t;

static Switch_GPIO_t g_gpioList[] = 
{
	{RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_6},  // INA
	{RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_7},  // INB
  {RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_8},  // INC
  {RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_9},  // IND
};

#define SWITCH_NUM_MAX (sizeof(g_gpioList) / sizeof(g_gpioList[0]))

/**
***********************************************************
* @brief 拨码开关硬件初始化
* @param
* @return 
***********************************************************
*/
void SwitchDrvInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	for (uint8_t i = 0; i < SWITCH_NUM_MAX; i++)
	{
	 RCC_APB2PeriphClockCmd(g_gpioList[i].rcu , ENABLE);
    
	 GPIO_InitStructure.GPIO_Pin = g_gpioList[i].pin ;				
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	  
   GPIO_Init(g_gpioList[i].gpio ,&GPIO_InitStructure);
	}
}

/**
***********************************************************
* @brief 获取拨码开关状态   bit(0,0,0,0,INA,INB,INC,IND)
* @param
* @return 0-校准状态
          1-有效值状态
***********************************************************
*/
uint8_t GetSwitchStatus(void)
{
	uint8_t res = 0x00;

	for (uint8_t i = 0; i < SWITCH_NUM_MAX; i++)
	{
		res <<= 1;
		if ( GPIO_ReadInputDataBit(g_gpioList[i].gpio, g_gpioList[i].pin) )
    {
		 res |= 0x01;
    }
    else 
    {
     res |= 0x00;
    }
	}	
	return res;
}

