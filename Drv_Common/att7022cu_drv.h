#ifndef __ATT7022CU_DRV_H
#define __ATT7022CU_DRV_H	 

#include <stdint.h>

#define APhase  0x00
#define BPhase  0x01
#define CPhase  0x02

void att7022cu_drv_init(void );
void ResetATT7022(void);
//�������Ĵ�������
uint32_t  ReadSampleRegister(uint8_t cmd);
//дУ��Ĵ���
void WriteAdjustRegister(uint8_t addr,uint32_t  data);
//��У��Ĵ���
uint32_t  ReadAdjustRegister(uint8_t addr);
//===============================================================
// ��ѹУ������
// PhaseType : ���ѹ����
// StandValue: ��׼��ѹֵ
// AdjustBuff: ����У������
//===============================================================
uint32_t AdjustVoltageFrun(float StandValue,uint8_t PhaseType);


#endif

