#ifndef _MOV_INVALID_H_
#define _MOV_INVALID_H_

#include <stdint.h>

/*��ʾ��λ*/
typedef enum
{
	MOV_LEVEL_GRID_0,
	MOV_LEVEL_GRID_1,
	MOV_LEVEL_GRID_2,
	MOV_LEVEL_GRID_3,
	MOV_LEVEL_GRID_4,
	OTHERLEVEL
} MovLevel_t;

typedef enum 
{
	EXCELLENT,
	MILD,          //��΢
	MODERATE,      //�ж�
	SEVERE,        //���ص�
	BROKEN,      //��ȫ��
	OTHERINDEX
} MovQualityIndex;

MovLevel_t GetMovLevel(void);
void MovInvalidProc(void );

#endif

