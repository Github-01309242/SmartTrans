#ifndef __HT7036_DRV_H
#define __HT7036_DRV_H	 

#include <stdint.h>

#define APhase  0x00
#define BPhase  0x01
#define CPhase  0x02

#define P_K    0.008858f   //����ϵ�� K=2.592*10^10/(HFconst*EC*2^23)  
#define UI_K   22.4f       //����ϵ��

void ht7036cu_drv_init(void );
void Resetht7036(void);
//�������Ĵ�������
uint32_t  ReadSampleRegister(uint8_t cmd);
//дУ��Ĵ���
void WriteAdjustRegister(uint8_t addr,uint32_t  data);
//��У��Ĵ���
uint32_t  ReadAdjustRegister(uint8_t addr);

uint32_t*  Get_UgainData(float StandValue);
uint32_t*  Get_IgainData(float StandValue);
uint32_t*  Get_PgainData(float StandValue );
uint32_t*  Get_PhasegainData(float StandValue );

long BmToYm(long value);


#endif

