
#ifndef __ENERGY_METER_DRV_H_
#define __ENERGY_METER_DRV_H_

#include <stdint.h>

typedef struct
{
  float   r_URms[3];
  
	float   r_IRms[3];
 
	float   r_P[3];               //�����й� 
  float   r_Pt;                 //�����й�
  
	float   r_Q[3];               //�����޹� 
  float   r_Qt;                 //�����޹�
  
	float   r_S[3];               //��������
  float   r_St;                 //��������
  
  float  	r_Pft;                //���๦������
  
  float   r_Freq;               //��Ƶ��

}meter_data_str;   


void ElectricCollectionProc(void );

uint32_t* ExportAll(void);

#endif
