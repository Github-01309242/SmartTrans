
#include "stm32f10x.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "delay.h"
#include "led_drv.h"
#include "gp8202as_drv.h"
#include "store_app.h"

#define GP8202AS_IIC_CLOCK_TIME	  RCC_APB2Periph_GPIOB
#define GP8202AS_IIC_PORT				  GPIOB
#define GP8202AS_IIC_SDA					GPIO_Pin_9
#define GP8202AS_IIC_SCK					GPIO_Pin_8

#define GET_I2C_SDA()             GPIO_ReadInputDataBit(GP8202AS_IIC_PORT, GP8202AS_IIC_SDA)	// 读SDA口线状态 

#define SET_I2C_SCL()             GPIO_SetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SCK)           // 时钟线SCL输出高电平
#define CLR_I2C_SCL()             GPIO_ResetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SCK)         // 时钟线SCL输出低电平
#define SET_I2C_SDA()             GPIO_SetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SDA)           // 数据线SDA输出高电平
#define CLR_I2C_SDA()             GPIO_ResetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SDA)         // 数据线SDA输出低电平

//#define SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)8<<4;}          //SDA配置输入模式
//#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)3<<4;}          //SDA配置输出模式
 
static void GP8202AS_GPIO_Config(void )
{
//	GPIO_InitTypeDef GPIO_InitStructure; 	
//	/*开启外设时钟*/
//  RCC_APB2PeriphClockCmd ( GP8202AS_IIC_CLOCK_TIME, ENABLE );	
//	/*选择引脚*/															   
//  GPIO_InitStructure.GPIO_Pin = GP8202AS_IIC_SDA | GP8202AS_IIC_SCK;	
//	/*设置引脚模式为通用开漏输出*/
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
//	/*设置引脚速率 */   
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//	/*调用库函数，初始化GPIO_PORT*/
//  GPIO_Init ( GPIOB, &GPIO_InitStructure );	 
//  GPIO_SetBits(GPIOB, GP8202AS_IIC_SDA|GP8202AS_IIC_SCK);	
  
  	GPIO_InitTypeDef GPIO_InitStructure; 	
	/*开启外设时钟*/
  RCC_APB2PeriphClockCmd ( GP8202AS_IIC_CLOCK_TIME, ENABLE );	
	/*选择引脚*/															   
  GPIO_InitStructure.GPIO_Pin = GP8202AS_IIC_SDA | GP8202AS_IIC_SCK;	
	/*设置引脚模式为通用开漏输出*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   
	/*设置引脚速率 */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*调用库函数，初始化GPIO_PORT*/
  GPIO_Init ( GPIOB, &GPIO_InitStructure );	 
  GPIO_SetBits(GPIOB, GP8202AS_IIC_SDA|GP8202AS_IIC_SCK);	
}

//-------------------------------------------------
// 函数名称：IIC_Start()
// 函数功能：IIC起动
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
static void GP8202AS_IIC_Start(void)
{
//	SDA_OUT();
	SET_I2C_SDA();
	DelayNus (5);
	SET_I2C_SCL();
	DelayNus (5);
	CLR_I2C_SDA();
	DelayNus (5);
}


//-------------------------------------------------
// 函数名称：IIC_Stop()
// 函数功能：IIC停止
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
static void GP8202AS_IIC_Stop(void)                     
{
//	SDA_OUT();
	CLR_I2C_SDA();
	DelayNus (5);
	SET_I2C_SCL();
	DelayNus (5);
	SET_I2C_SDA();
}


//-------------------------------------------------------
// 函数名称：W_Byte1()
// 函数功能：向IIC器件写入一个字节
// 入口参数：待写入的一个字节（uByteVal）
// 出口参数：无
//-------------------------------------------------------
static void W_Byte1(uint8_t  uByteVal)
{
//	SDA_OUT();
  for(uint8_t iCount = 0;iCount < 8;iCount++)
  {  
    CLR_I2C_SCL();
    DelayNus (5);			
    
    if( uByteVal & 0x80 )
      SET_I2C_SDA();
    else
      CLR_I2C_SDA();
    
    DelayNus (5);        
    SET_I2C_SCL();
    DelayNus (5);
    uByteVal <<= 1;
  } 
	CLR_I2C_SCL();  	  
}

//-------------------------------------------------
// 函数名称：IIC_RdAck()
// 函数功能：读IIC应答
// 入口参数：无
// 出口参数：是否应答真值
//--------------------------------------------------
static uint8_t  IIC_RdAck(void)                      
{ 
	uint8_t  AckFlag;
	uint8_t  uiVal = 0;
	CLR_I2C_SCL(); 
	DelayNus (5);    
	SET_I2C_SDA();
	SET_I2C_SCL();
	DelayNus (5);

//	SDA_IN();
	while((1 == GET_I2C_SDA() ) && (uiVal < 255))
	{
		uiVal ++;
		AckFlag = GET_I2C_SDA() ;
	}

	CLR_I2C_SCL();  
	return AckFlag;		// 应答返回：0;不应答返回：1
}

//-----------------------------------------------------
// 函数名称：IIC_Nack()
// 函数功能：IIC不应答
// 入口参数：无
// 出口参数：无
//-----------------------------------------------------
static void IIC_Nack(void)                    
{   
//	SDA_OUT();
	SET_I2C_SDA();
	CLR_I2C_SCL(); 
	DelayNus (5);       
	SET_I2C_SCL();
	DelayNus (5);       
	CLR_I2C_SCL();
}

void Send_gp8202as_data(uint8_t order , uint16_t DAC_data)
{
  uint8_t  uByteVal;
  
	GP8202AS_IIC_Start();			// 发送开始信号
  
  uByteVal = order*2 ;
	W_Byte1(0xb0|uByteVal);		  //1,0,1,1,A2,A1,A0,0
									          //芯片代码		地址位
	IIC_RdAck();  					  //读应答信号
	W_Byte1(0x02);					  //固定02
	IIC_RdAck();					    //读应答信号
	W_Byte1(DAC_data&0xFF);		//数据 低8位											
	IIC_RdAck();					    //读应答信号
	W_Byte1(DAC_data>>8&0xFF);//数据 高八位		
	IIC_RdAck();					    //读应答信号
	GP8202AS_IIC_Stop();
}

static void GpioInit(void)
{
  // 打开串口GPIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
  
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;               
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void AdcInit( void)
{
  /* 使能ADC1通道时钟 */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1	, ENABLE );	  
  /* 设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M  */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
  /* 复位ADC1 */  
  ADC_DeInit(ADC1);  
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC1配置 */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 //工作模式：独立模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;                       //浏览模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE  ;                //ADC工作在单次转化模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             //ADC数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 3;                            //3个通道
  ADC_Init(ADC1, &ADC_InitStructure);

  /******************************************************************
     设置指定ADC的规则组通道，设置通道对应的转化顺序和采样时间
  *******************************************************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 2, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime_55Cycles5);

  ADC_DMACmd(ADC1, ENABLE);                                          //使能ADC的DMA功能
  ADC_Cmd(ADC1, ENABLE);                                             //使能ADC

  ADC_ResetCalibration(ADC1);                                        //校验复位
  while(ADC_GetResetCalibrationStatus(ADC1));                        //等待复位完成

  ADC_StartCalibration(ADC1);                                        //开始ADC1校准
  while(ADC_GetCalibrationStatus(ADC1));                             //等待校验完成
  /* 启动转换 */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);                            
}

#define ADC_BUF_SIZE 3
/* 变量*/
static uint16_t g_adcVal[ADC_BUF_SIZE];

static  void DmaInit(void)
{
    /* 使能DMA1时钟 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  DMA_InitTypeDef DMA_InitStructure;

  DMA_DeInit(DMA1_Channel1);                                         //外设地址
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_adcVal;         //内存地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //传输方向:外设 -> 内存
  DMA_InitStructure.DMA_BufferSize = ADC_BUF_SIZE;                   //传输长度
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设递增:关闭
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //内存递增:打开
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据宽度:16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                    //循环模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //优先级:高
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //内存-内存:否
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1, ENABLE);                                    //使能通道
}




#define MAX_BUF_SIZE         20
uint16_t g_curr4MplBuf[ADC_BUF_SIZE][MAX_BUF_SIZE];
uint16_t g_curr10MplBuf[ADC_BUF_SIZE][MAX_BUF_SIZE];
//bool  g_isSensorOk = true ;


/**
***********************************************************
* @brief 算术平均滤波
* @param arr，数组首地址
* @param len，元素个数
* @return 平均运算结果
***********************************************************
*/
static uint16_t ArithAvgFltr(uint16_t *arr, uint32_t len)
{
	uint32_t sum = 0;
	for (uint32_t i = 0; i < len; i++)
	{
		sum += arr[i];
	}
	return (uint16_t)(sum / len);
}

/**
***********************************************************
* @brief qsort函数调用的回调函数，比较规则，降序排列
* @param *_a，对应数组元素
* @param *_b，对应数组元素
* @return 比较结果
***********************************************************
*/
static int32_t CmpCb(const void *_a, const void *_b)
{
	uint16_t *a = (uint16_t *)_a;
	uint16_t *b = (uint16_t *)_b;
	int32_t val = 0;
	 if (*a > *b)
	 {
		val = -1;
	 }
	 else if (*a < *b)
	 {
		val =  1;
	 }
	 else
	 {
		val = 0;
	 }
	 return val;
}

/**
***********************************************************
* @brief 中位值平均滤波
* @param arr，数组首地址
* @param len，元素个数，需要大于等于3个
* @return 平均运算结果
***********************************************************
*/
static uint16_t MedianAvgFltr(uint16_t *arr, uint32_t len)
{
	qsort(arr, len, sizeof(uint16_t), CmpCb);
	return ArithAvgFltr(&arr[5], len - 5);
}

 /**
  * @brief  GP8202AS 硬件初始化函数
  * @param  无
  * @retval 无
  */
void Gp8202asDrvInit ( void )
{
	GP8202AS_GPIO_Config ();	
  
  GP8202AS_IIC_Start();	
  W_Byte1(0xb0|0x00);
  IIC_RdAck();
  W_Byte1(0x02);
  IIC_RdAck();
  IIC_Nack();
  GP8202AS_IIC_Stop();	
  
  GP8202AS_IIC_Start();	
  W_Byte1(0xb0|0x02);
  IIC_RdAck();
  W_Byte1(0x02);
  IIC_RdAck();
  IIC_Nack();
  GP8202AS_IIC_Stop();	
  
  GP8202AS_IIC_Start();	
  W_Byte1(0xb0|0x04);
  IIC_RdAck();
  W_Byte1(0x02);
  IIC_RdAck();
  IIC_Nack();
  GP8202AS_IIC_Stop();	
  
//  GpioInit();
//	AdcInit();
//  DmaInit(); 
}



 /**
  * @brief  gp8211s_OutPutCurrent
  * @param  4-20mA  对应 0x28F5  - 0xCCCC
            输出电流大小为： IOUT=5V/RS * (DATA/0xFFFF)    RS=200
  * @retval 无
  * @description 
  */

void gp8211s_OutPutCurrent(uint8_t Order ,uint16_t Median  , float Offset ,float Slope )
{	
	uint16_t  RegVal = 0;
	
	if( Median > 0xCCCC )  Median = 0xCCCC;
  
  RegVal = (uint16_t )((Median*20.0 /52428.0 - Offset) * Slope + 10485) ;
	Send_gp8202as_data(Order ,RegVal);
}




typedef struct 
{
  float slopem_den[3];
  float slopem[3];
  uint16_t getLowerAdcValue[3];
  uint16_t getUpperAdcValue[3]; 
  float I_lowerlimit[3];

}STRPARA;

STRPARA  ParaDlg ;


#define RS       200                              //采样电阻阻值
#define Vref     2.499f                           //ADC基准电压
#define Scale    Vref /4096 *1.0f /RS
#define Div      2                                //电阻分压比例

/**
  * @brief  AutoCalibrationPro 自动校准过程函数
  * @param  无
  * @retval 无
  */
void  AutoCalibrationPro( void )
{
  static bool mark = false ;
  static Calibration_Type State ;
  static uint16_t s_convertNum1 = 0 , s_convertNum2 = 0;
  
  if( State == Standard_value_4 )
  {  
    if( mark == false  )
    {
      mark  = true ;
      Send_gp8202as_data(  HardIDofFirst   ,LowerReferCurrent );   //发送4.0mA对应写入GP8202AS中的十六进制数0x28F5 
      DelayNms (100);
      Send_gp8202as_data(  HardIDofSecond  ,LowerReferCurrent );   //发送4.0mA对应写入GP8202AS中的十六进制数0x28F5 
      DelayNms (100);
      Send_gp8202as_data(  HardIDofThird   ,0x47AD );   //发送4.0mA对应写入GP8202AS中的十六进制数0x28F5 
      DelayNms (100);
    }
//    if (s_convertNum1 < MAX_BUF_SIZE)
//    {
//      g_curr4MplBuf[HardIDofFirst][s_convertNum1] = g_adcVal[HardIDofFirst]  ; 
//      g_curr4MplBuf[HardIDofSecond][s_convertNum1] = g_adcVal[HardIDofSecond]  ; 
//      g_curr4MplBuf[HardIDofThird][s_convertNum1] = g_adcVal[HardIDofThird]  ; 
//      
//      s_convertNum1++;
//      if (s_convertNum1 == MAX_BUF_SIZE)
//      {   
//        
//        ParaDlg.getLowerAdcValue [HardIDofFirst] = MedianAvgFltr(g_curr4MplBuf[HardIDofFirst], s_convertNum1)  ;
//        ParaDlg.getLowerAdcValue [HardIDofSecond] = MedianAvgFltr(g_curr4MplBuf[HardIDofSecond], s_convertNum1)  ;
//        ParaDlg.getLowerAdcValue [HardIDofThird] = MedianAvgFltr(g_curr4MplBuf[HardIDofThird], s_convertNum1)  ;
//        
//        State = Standard_value_10 ;
//        mark = false ;
//      }      
//    }
  }
//  else if ( State == Standard_value_10 )
//  {    
//    if( mark == false  )
//    {
//      mark  = true ;
//      Send_gp8202as_data( HardIDofFirst, UpperReferCurrent );  //发送10.0mA对应写入GP8202AS中的十六进制数0x6666 
//      DelayNms (100);
//      Send_gp8202as_data( HardIDofSecond, UpperReferCurrent );  //发送10.0mA对应写入GP8202AS中的十六进制数0x6666 
//      DelayNms (100);
//      Send_gp8202as_data( HardIDofThird, UpperReferCurrent );   //发送10.0mA对应写入GP8202AS中的十六进制数0x6666 
//      DelayNms (100);
//    }
//    if (s_convertNum2 < MAX_BUF_SIZE)
//    { 
//      g_curr10MplBuf[HardIDofFirst][s_convertNum2] = g_adcVal[HardIDofFirst] ; 
//      g_curr10MplBuf[HardIDofSecond][s_convertNum2] = g_adcVal[HardIDofSecond] ; 
//      g_curr10MplBuf[HardIDofThird][s_convertNum2] = g_adcVal[HardIDofThird] ; 
//      s_convertNum2++;
//      if (s_convertNum2 == MAX_BUF_SIZE)
//      {   
//        ParaDlg.getUpperAdcValue [HardIDofFirst] = MedianAvgFltr(g_curr10MplBuf[HardIDofFirst], s_convertNum2)  ;
//        ParaDlg.getUpperAdcValue [HardIDofSecond] = MedianAvgFltr(g_curr10MplBuf[HardIDofSecond], s_convertNum2)  ;
//        ParaDlg.getUpperAdcValue [HardIDofThird] = MedianAvgFltr(g_curr10MplBuf[HardIDofThird], s_convertNum2)  ;
//        State = Parameter_infer ;
//      }      
//    }
//  }
//  else if ( State == Parameter_infer )
//  { 
//    for(uint8_t i =0 ;i < 3; i++ )
//    {
//      ParaDlg.slopem_den [i] = (ParaDlg.getUpperAdcValue [i] - ParaDlg.getLowerAdcValue [i]  ) *Div * Scale *1000 ;
//      ParaDlg.slopem [i] =  (UpperReferCurrent - LowerReferCurrent) * 1.0f /ParaDlg.slopem_den [i] ;
//      
//      ParaDlg.I_lowerlimit [i] =  Scale * ParaDlg.getLowerAdcValue [i] *Div *1000 ;       
//    }
//    
////    printf("比例   %f   %f   %f \r\n",ParaDlg.slopem [0] ,ParaDlg.slopem [1], ParaDlg.slopem [2]); 
////    printf("偏置   %f   %f   %f \r\n",ParaDlg.I_lowerlimit [0] ,ParaDlg.I_lowerlimit [1], ParaDlg.I_lowerlimit [2]);    
//        
//    SetGainBuffParam( ParaDlg.slopem [2]*1000, 2);
//    SetGainBuffParam( ParaDlg.slopem [1]*1000, 1);
//    SetGainBuffParam( ParaDlg.slopem [0]*1000, 0);
//    
//    SetOffsetBuffParam(ParaDlg.I_lowerlimit[2]*1000, 2);
//    SetOffsetBuffParam(ParaDlg.I_lowerlimit[1]*1000, 1);
//    SetOffsetBuffParam(ParaDlg.I_lowerlimit[0]*1000, 0);

//    State = OTHER ;
//  }
//  else if(State == OTHER  )
//  {
//    ToggleLed(0);
//    return ;
//  }
}
  
/**
  * @brief  Get_offset 获取偏移量
  * @param  无
  * @retval 无
  */
float* Get_Offset(void )
{
  return ParaDlg.I_lowerlimit  ;
}


/**
  * @brief  Get_Slope 获取斜率
  * @param  无
  * @retval 无
  */
float* Get_Slope(void )
{
 return ParaDlg.slopem;
}

 /**
  * @brief  
  * @param  无
  * @retval 无
  */
void Gp8202as_Test(void )
{

}

