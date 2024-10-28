
#include <stdint.h>
#include "stm32f10x.h"
#include "systick_drv.h"
#include "led_drv.h"
#include "delay.h"



typedef struct
{
	uint32_t rcu;
	GPIO_TypeDef * gpio;
	uint16_t pin;
} Discreteinput_GPIO_t;

static Discreteinput_GPIO_t g_gpioList[] = 
{
  {RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_9},    // ��������1
	{RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_8},    // ��������2
	
};

#define NUM_MAX (sizeof(g_gpioList) / sizeof(g_gpioList[0]))

/**
***********************************************************
* @brief Ӳ����ʼ��
* @param
* @return 
***********************************************************
*/
void Discreteinput_DrvInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	for (uint8_t i = 0; i < NUM_MAX; i++)
	{
	 RCC_APB2PeriphClockCmd(g_gpioList[i].rcu , ENABLE);
    
	 GPIO_InitStructure.GPIO_Pin = g_gpioList[i].pin ;				
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	  
   GPIO_Init(g_gpioList[i].gpio ,&GPIO_InitStructure);
	}
}

#define Normally_open   1    

/**
***********************************************************
* @brief ��ȡ�ⲿ����������״̬
* @param
* @return   �����׵�ַ
***********************************************************
*/
static uint8_t res[NUM_MAX] ;
uint8_t* GetDiscreteinputStatus(void)
{	
	for (uint8_t i = 0; i < NUM_MAX; i++)
	{		
    #if Normally_open
		if ( GPIO_ReadInputDataBit(g_gpioList[i].gpio, g_gpioList[i].pin) )
    {  
   		res [i] = 0x00;
	  }
    else 
    {      
      res [i] = 0x01;
    }
    #else
    if (  !GPIO_ReadInputDataBit(g_gpioList[i].gpio, g_gpioList[i].pin) )
    {  
   		res [i] = 0x00;
	  }
    else 
    {      
      res [i] = 0x01;
    }
    #endif

	}    
	return (uint8_t *)res;
}






