
#include "stm32f10x.h"
#include <time.h>
#include "rtc_drv.h"
#include "delay.h"
#include <stdint.h>
#include <string.h>


#define MAGIC_CODE    0x5A5A

/**
***********************************************************
* @brief RTCӲ����ʼ��
* @param
* @return 
***********************************************************
*/
void RtcDrvInit(void)
{
	u8 temp=0;
  
  /*��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ���ͬ*/
	if ( BKP_ReadBackupRegister(BKP_DR1) != MAGIC_CODE )		
  {	    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
    PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ������� 
    BKP_DeInit();	//��λ�������� 	
    RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
    {
    temp++;
    DelayNms(10);
    }
    if(temp>=250) return;                     //��ʼ��ʱ��ʧ��,����������	    
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
    RCC_RTCCLKCmd(ENABLE);	                  //ʹ��RTCʱ��  
    RTC_WaitForLastTask();	                  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    RTC_WaitForSynchro();		                  //�ȴ�RTC�Ĵ���ͬ��  
    RTC_WaitForLastTask();	                  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    RTC_EnterConfigMode();                    // ��������	
    RTC_SetPrescaler(32767);                  //����RTCԤ��Ƶ��ֵ
    RTC_WaitForLastTask();	                  //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    /* 2024��5��24�� 15:30:53 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
    PWR_BackupAccessCmd(ENABLE);	            //ʹ��RTC�ͺ󱸼Ĵ������� 
    RTC_SetCounter(1716535853);	              //����RTC��������ֵ
    RTC_WaitForLastTask();	                 //�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
    /* �˳�����ģʽ */
    RTC_ExitConfigMode();  
    /* ��ָ���ĺ󱸼Ĵ�����д���û���������  */    
    BKP_WriteBackupRegister(BKP_DR1, MAGIC_CODE);	     
    return;    
  }
  /* ʹ�ܶ�RTC�ķ��ʣ�ʹ��PMU��BKPʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	
  /* ʹ�ܶԺ󱸼Ĵ�����RTC��дȨ�� */
  PWR_BackupAccessCmd(ENABLE);	
  /* �ȴ�APB1�ӿ�ʱ�Ӻ�RTCʱ��ͬ�� */
  RTC_WaitForSynchro();	
  /* �ȴ��ϴζ� RTC �Ĵ���д������� */
  RTC_WaitForLastTask();   
}		


/**
***********************************************************
* @brief ����ʱ��
* @param time,���룬����ʱ��
* @return 
***********************************************************
*/
void SetRtcTime(RtcTime_t *time)
{
  time_t timeStamp;
  struct tm tmInfo;
  memset(&tmInfo, 0, sizeof(tmInfo));

  tmInfo.tm_year = time->year - 1900;
  tmInfo.tm_mon = time->month - 1;
  tmInfo.tm_mday = time->day;
  tmInfo.tm_hour = time->hour;
  tmInfo.tm_min = time->minute;
//  tmInfo.tm_sec = time->second;

  timeStamp = mktime(&tmInfo) - 8 * 60 * 60;

  RTC_WaitForLastTask();	 //�ȴ����һ�ζ�RTC�Ĵ�����д�������
  RTC_SetCounter(timeStamp);
}

/**
***********************************************************
* @brief ��ȡʱ��
* @param time,���������ʱ��
* @return 
***********************************************************
*/
void GetRtcTime(RtcTime_t *time)
{
	time_t timeStamp;
	struct tm *tmInfo;
	
	timeStamp = RTC_GetCounter() + 8 * 60 * 60;
	tmInfo = localtime(&timeStamp);
	time->year = tmInfo->tm_year + 1900 ;
	time->month = tmInfo->tm_mon + 1;
	time->day = tmInfo->tm_mday;
	time->hour = tmInfo->tm_hour;
	time->minute = tmInfo->tm_min;
	time->second = tmInfo->tm_sec;
}
		  


/************************END OF FILE***************************************/
