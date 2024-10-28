
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

#define GET_I2C_SDA()             GPIO_ReadInputDataBit(GP8202AS_IIC_PORT, GP8202AS_IIC_SDA)	// ��SDA����״̬ 

#define SET_I2C_SCL()             GPIO_SetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SCK)           // ʱ����SCL����ߵ�ƽ
#define CLR_I2C_SCL()             GPIO_ResetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SCK)         // ʱ����SCL����͵�ƽ
#define SET_I2C_SDA()             GPIO_SetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SDA)           // ������SDA����ߵ�ƽ
#define CLR_I2C_SDA()             GPIO_ResetBits(GP8202AS_IIC_PORT, GP8202AS_IIC_SDA)         // ������SDA����͵�ƽ

//#define SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)8<<4;}          //SDA��������ģʽ
//#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)3<<4;}          //SDA�������ģʽ
 
static void GP8202AS_GPIO_Config(void )
{
//	GPIO_InitTypeDef GPIO_InitStructure; 	
//	/*��������ʱ��*/
//  RCC_APB2PeriphClockCmd ( GP8202AS_IIC_CLOCK_TIME, ENABLE );	
//	/*ѡ������*/															   
//  GPIO_InitStructure.GPIO_Pin = GP8202AS_IIC_SDA | GP8202AS_IIC_SCK;	
//	/*��������ģʽΪͨ�ÿ�©���*/
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
//	/*������������ */   
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//	/*���ÿ⺯������ʼ��GPIO_PORT*/
//  GPIO_Init ( GPIOB, &GPIO_InitStructure );	 
//  GPIO_SetBits(GPIOB, GP8202AS_IIC_SDA|GP8202AS_IIC_SCK);	
  
  	GPIO_InitTypeDef GPIO_InitStructure; 	
	/*��������ʱ��*/
  RCC_APB2PeriphClockCmd ( GP8202AS_IIC_CLOCK_TIME, ENABLE );	
	/*ѡ������*/															   
  GPIO_InitStructure.GPIO_Pin = GP8202AS_IIC_SDA | GP8202AS_IIC_SCK;	
	/*��������ģʽΪͨ�ÿ�©���*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   
	/*������������ */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIO_PORT*/
  GPIO_Init ( GPIOB, &GPIO_InitStructure );	 
  GPIO_SetBits(GPIOB, GP8202AS_IIC_SDA|GP8202AS_IIC_SCK);	
}

//-------------------------------------------------
// �������ƣ�IIC_Start()
// �������ܣ�IIC��
// ��ڲ�������
// ���ڲ�������
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
// �������ƣ�IIC_Stop()
// �������ܣ�IICֹͣ
// ��ڲ�������
// ���ڲ�������
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
// �������ƣ�W_Byte1()
// �������ܣ���IIC����д��һ���ֽ�
// ��ڲ�������д���һ���ֽڣ�uByteVal��
// ���ڲ�������
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
// �������ƣ�IIC_RdAck()
// �������ܣ���IICӦ��
// ��ڲ�������
// ���ڲ������Ƿ�Ӧ����ֵ
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
	return AckFlag;		// Ӧ�𷵻أ�0;��Ӧ�𷵻أ�1
}

//-----------------------------------------------------
// �������ƣ�IIC_Nack()
// �������ܣ�IIC��Ӧ��
// ��ڲ�������
// ���ڲ�������
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
  
	GP8202AS_IIC_Start();			// ���Ϳ�ʼ�ź�
  
  uByteVal = order*2 ;
	W_Byte1(0xb0|uByteVal);		  //1,0,1,1,A2,A1,A0,0
									          //оƬ����		��ַλ
	IIC_RdAck();  					  //��Ӧ���ź�
	W_Byte1(0x02);					  //�̶�02
	IIC_RdAck();					    //��Ӧ���ź�
	W_Byte1(DAC_data&0xFF);		//���� ��8λ											
	IIC_RdAck();					    //��Ӧ���ź�
	W_Byte1(DAC_data>>8&0xFF);//���� �߰�λ		
	IIC_RdAck();					    //��Ӧ���ź�
	GP8202AS_IIC_Stop();
}

static void GpioInit(void)
{
  // �򿪴���GPIO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
  
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;               
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void AdcInit( void)
{
  /* ʹ��ADC1ͨ��ʱ�� */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1	, ENABLE );	  
  /* ����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M  */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
  /* ��λADC1 */  
  ADC_DeInit(ADC1);  
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC1���� */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 //����ģʽ������ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;                       //���ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE  ;                //ADC�����ڵ���ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             //ADC�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 3;                            //3��ͨ��
  ADC_Init(ADC1, &ADC_InitStructure);

  /******************************************************************
     ����ָ��ADC�Ĺ�����ͨ��������ͨ����Ӧ��ת��˳��Ͳ���ʱ��
  *******************************************************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 2, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime_55Cycles5);

  ADC_DMACmd(ADC1, ENABLE);                                          //ʹ��ADC��DMA����
  ADC_Cmd(ADC1, ENABLE);                                             //ʹ��ADC

  ADC_ResetCalibration(ADC1);                                        //У�鸴λ
  while(ADC_GetResetCalibrationStatus(ADC1));                        //�ȴ���λ���

  ADC_StartCalibration(ADC1);                                        //��ʼADC1У׼
  while(ADC_GetCalibrationStatus(ADC1));                             //�ȴ�У�����
  /* ����ת�� */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);                            
}

#define ADC_BUF_SIZE 3
/* ����*/
static uint16_t g_adcVal[ADC_BUF_SIZE];

static  void DmaInit(void)
{
    /* ʹ��DMA1ʱ�� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  DMA_InitTypeDef DMA_InitStructure;

  DMA_DeInit(DMA1_Channel1);                                         //�����ַ
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_adcVal;         //�ڴ��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //���䷽��:���� -> �ڴ�
  DMA_InitStructure.DMA_BufferSize = ADC_BUF_SIZE;                   //���䳤��
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:�ر�
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:��
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���ݿ��:16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                    //ѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:��
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1, ENABLE);                                    //ʹ��ͨ��
}




#define MAX_BUF_SIZE         20
uint16_t g_curr4MplBuf[ADC_BUF_SIZE][MAX_BUF_SIZE];
uint16_t g_curr10MplBuf[ADC_BUF_SIZE][MAX_BUF_SIZE];
//bool  g_isSensorOk = true ;


/**
***********************************************************
* @brief ����ƽ���˲�
* @param arr�������׵�ַ
* @param len��Ԫ�ظ���
* @return ƽ��������
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
* @brief qsort�������õĻص��������ȽϹ��򣬽�������
* @param *_a����Ӧ����Ԫ��
* @param *_b����Ӧ����Ԫ��
* @return �ȽϽ��
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
* @brief ��λֵƽ���˲�
* @param arr�������׵�ַ
* @param len��Ԫ�ظ�������Ҫ���ڵ���3��
* @return ƽ��������
***********************************************************
*/
static uint16_t MedianAvgFltr(uint16_t *arr, uint32_t len)
{
	qsort(arr, len, sizeof(uint16_t), CmpCb);
	return ArithAvgFltr(&arr[5], len - 5);
}

 /**
  * @brief  GP8202AS Ӳ����ʼ������
  * @param  ��
  * @retval ��
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
  * @param  4-20mA  ��Ӧ 0x28F5  - 0xCCCC
            ���������СΪ�� IOUT=5V/RS * (DATA/0xFFFF)    RS=200
  * @retval ��
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


#define RS       200                              //����������ֵ
#define Vref     2.499f                           //ADC��׼��ѹ
#define Scale    Vref /4096 *1.0f /RS
#define Div      2                                //�����ѹ����

/**
  * @brief  AutoCalibrationPro �Զ�У׼���̺���
  * @param  ��
  * @retval ��
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
      Send_gp8202as_data(  HardIDofFirst   ,LowerReferCurrent );   //����4.0mA��Ӧд��GP8202AS�е�ʮ��������0x28F5 
      DelayNms (100);
      Send_gp8202as_data(  HardIDofSecond  ,LowerReferCurrent );   //����4.0mA��Ӧд��GP8202AS�е�ʮ��������0x28F5 
      DelayNms (100);
      Send_gp8202as_data(  HardIDofThird   ,0x47AD );   //����4.0mA��Ӧд��GP8202AS�е�ʮ��������0x28F5 
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
//      Send_gp8202as_data( HardIDofFirst, UpperReferCurrent );  //����10.0mA��Ӧд��GP8202AS�е�ʮ��������0x6666 
//      DelayNms (100);
//      Send_gp8202as_data( HardIDofSecond, UpperReferCurrent );  //����10.0mA��Ӧд��GP8202AS�е�ʮ��������0x6666 
//      DelayNms (100);
//      Send_gp8202as_data( HardIDofThird, UpperReferCurrent );   //����10.0mA��Ӧд��GP8202AS�е�ʮ��������0x6666 
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
////    printf("����   %f   %f   %f \r\n",ParaDlg.slopem [0] ,ParaDlg.slopem [1], ParaDlg.slopem [2]); 
////    printf("ƫ��   %f   %f   %f \r\n",ParaDlg.I_lowerlimit [0] ,ParaDlg.I_lowerlimit [1], ParaDlg.I_lowerlimit [2]);    
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
  * @brief  Get_offset ��ȡƫ����
  * @param  ��
  * @retval ��
  */
float* Get_Offset(void )
{
  return ParaDlg.I_lowerlimit  ;
}


/**
  * @brief  Get_Slope ��ȡб��
  * @param  ��
  * @retval ��
  */
float* Get_Slope(void )
{
 return ParaDlg.slopem;
}

 /**
  * @brief  
  * @param  ��
  * @retval ��
  */
void Gp8202as_Test(void )
{

}

