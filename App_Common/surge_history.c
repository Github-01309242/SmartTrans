
#include <stdbool.h>
#include "surge_history.h"
#include "surgemdl_drv.h"
#include "sensor_drv.h"
#include "norflash_drv.h"
#include "systeminf.h"
#include "store_app.h"
#include "led_drv.h"

static SurgeInfo_t g_SurgeInfoCurrent;

/**
***********************************************************
* @brief ������ӿ������ϵͳ�ϵ��ʼ���ṹ����
* @param
* @return 
***********************************************************
*/

static bool StoreSurgeNum(uint8_t Num)
{
//	if (Num == 0 )
//	{
//		return true;
//	}	
	SysParam_t Param ;  
  ReadSysParam(&Param);
	Param.LightningNum  = Num;	
	if (!WriteSysParam(&Param))
	{
		return false;
	}
	return true;
}


/**
***********************************************************
* @brief ������ӿ������Ϣ���ⲿnorflash��
* @param
* @return 
***********************************************************
*/
static void  RecordSurgeParam(SurgeInfo_t * Param)
{
  static uint8_t  Num = 0;
  
  Num = Param->Num - 1;
  //��ҳд���ݣ�1ҳ��Ӧһ�ε�������Ϣ��¼
  EraseNorflashForWrite( (uint32_t )(Num*4096), 1);
	WriteNorflashData( (uint32_t )(Num*4096), (uint8_t *)Param, sizeof(SurgeInfo_t) );
  
//uint8_t  bufferRead[NUM_MAX] = {0};
//ReadNorflashData ((uint32_t )(Num*4096),bufferRead,NUM_MAX);
///**��������ͨ�¼���¼�ñ���***/
//for( uint8_t i=0; i < NUM_MAX; i++)
//{
//  USART_ClearFlag(USART1, USART_FLAG_TC);
//  USART_SendData( USART1 , *( bufferRead + i ) );
//  /*�ȴ��������*/
//  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) ;
//}
  
}

/**
***********************************************************
* @brief ��¼�׻���Ϣ����
* @param
* @return 
***********************************************************
*/
#define Change_factor    0x14      // 1kA ��Ӧ 0x14=20  ���ֵ��Ҫ�ر�궨
static SensorData_t sensorData;
void SurgeHistoryProc( void  )
{
  
//  static uint8_t LastNum = 0; 
  GetSensorData(&sensorData);
//  if(g_SurgemdlParam.Num == LastNum )
//  {
//   return ;
//  }
//   LastNum = g_SurgemdlParam.Num; 
  if( GetRefreshflag ())
  {
   g_SurgeInfoCurrent.Num =  sensorData.SurgemdlParam .Num  ;
   g_SurgeInfoCurrent.PeakCurrent = sensorData.SurgemdlParam .PeakCurrent   * 1.0f / Change_factor * 10;   
   //�����洢������eeprom��  
   StoreSurgeNum( g_SurgeInfoCurrent.Num );  
   //��ȡ����ʱ��    
   GetRtcTime ( &g_SurgeInfoCurrent.RtcTime );   
    //������ӿ������Ϣ���ⲿnorflash��  
   RecordSurgeParam( &g_SurgeInfoCurrent) ;    
  }
}

//uint8_t * RecvStr(void )
//{
//  return (uint8_t *)&g_SurgeInfoCurrent;
//}
/**
***********************************************************
* @brief ������ӿ��ʷ����
* @param
* @return 
***********************************************************
*/
#define   BUFFER_SIZE              4096
#define   FLASH_ADDRESS            4096
void EraseSurgeHistoryInfo(void )
{
  while (sensorData.SurgemdlParam .Num--)
  {    
    EraseNorflashForWrite( FLASH_ADDRESS* sensorData.SurgemdlParam .Num, BUFFER_SIZE);
    ToggleLed(0);   
  }
  StoreSurgeNum(0);      //��eeprom�иı���ӿ����
  PresetNum_Surge(0);    //����Ҫ��λ
}  
/**
***********************************************************
* @brief ���ؾ����Ĵ���ӿ�������Ϣ
* @param pageindex  ָ��buffer
* @return 
***********************************************************
*/
void GetSurgeHistoryInfo(uint16_t pageindex ,uint8_t * buffer)
{
  ReadNorflashData ((uint32_t )(pageindex*4096),buffer,sizeof (SurgeInfo_t));
}





