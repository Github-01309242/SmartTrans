#ifndef __INVALID_APP_H
#define	__INVALID_APP_H

#include <stdint.h>

#define Invalid_START_Sector        257   //  ��ʼ�洢��ɢ����������仯ʱ�̵�ʱ����Ϣ��������ʼ��ַ
/**
***********************************************************
* @brief ����ʧЧʱ��
* @param pageindex  ָ��buffer
* @return 
***********************************************************
*/
void GetInvalidTime(uint16_t pageindex ,uint8_t * buffer);

void Invalid_Task(void );


#endif
