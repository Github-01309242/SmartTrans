/*
 * ʧЧ����Ӧ�ó���
 */
#include "switch_drv.h"
#include <stdbool.h>
#include "led_drv.h"
#include "rtc_drv.h"
#include "norflash_drv.h"
#include "signal_drv.h"
#include "mov_invalid.h"
#include "invalid_app.h"

#define DiscreteInputsNum   2     //  һ��2����ɢ�ź�(ң��)���� 

static RtcTime_t RtcTime[2]={0};

#define   BUFFER_SIZE              4096
#define   FLASH_ADDRESS            4096

static void SwitchinputProc(void )
{
  static uint8_t  flag[2] = {0x00 ,0x00} ;
  uint8_t* STA = GetDiscreteinputStatus();
  for (uint8_t  i = 0; i < DiscreteInputsNum; i++) 
  {
      uint8_t  mask = 0x01;  
      bool bit = (bool )(STA[i] & mask);  // ʹ�� & ������������λ��0����1

      if( bit  && !flag[i])
      {
        flag[i] = 0x01 ;
        TurnOnLed (i+1);
        GetRtcTime ( &RtcTime[i] );   //��ȡ����ʱ��
        
        EraseNorflashForWrite(FLASH_ADDRESS*(Invalid_START_Sector+i), sizeof(RtcTime[0]));
        WriteNorflashData(FLASH_ADDRESS*(Invalid_START_Sector+i), (uint8_t *)&RtcTime[i], sizeof(RtcTime[0]));
      }   
      else if( bit == false )
      {   
        flag[i] = 0x00 ;          
        TurnOffLed (i+1);
      }  
      
  }
}

/**
***********************************************************
* @brief ����ʧЧʱ��
* @param pageindex  ָ��buffer
* @return 
***********************************************************
*/
void GetInvalidTime(uint16_t pageindex ,uint8_t * buffer)
{
  ReadNorflashData ((uint32_t )(pageindex*4096),buffer,sizeof( RtcTime_t));
}

void Invalid_Task(void )
{
  SwitchinputProc();
  MovInvalidProc();
}

