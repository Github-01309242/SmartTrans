#ifndef _SURGE_HISTORY_H_
#define _SURGE_HISTORY_H_

#include <stdint.h>
#include "rtc_drv.h"

/*������ӿ�����Ϣ�Ĵ���*/
typedef struct {
    uint16_t    Num;
	
	/* ������ò�����ʼ */
    uint16_t    PeakCurrent;
    RtcTime_t   RtcTime;       //����ṹ�����
	
	/* ������ò������� */
} SurgeInfo_t;

void SurgeHistoryProc(void );
//uint8_t * RecvStr(void );
void GetSurgeHistoryInfo(uint16_t pageindex ,uint8_t * buffer);
void EraseSurgeHistoryInfo(void );

#endif
