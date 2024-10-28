
#ifndef __ENERGY_METER_DRV_H_
#define __ENERGY_METER_DRV_H_

#include <stdint.h>

typedef struct
{
  float   r_URms[3];
  
	float   r_IRms[3];
 
	float   r_P[3];               //分相有功 
  float   r_Pt;                 //合相有功
  
	float   r_Q[3];               //分相无功 
  float   r_Qt;                 //合相无功
  
	float   r_S[3];               //分相视在
  float   r_St;                 //合相视在
  
  float  	r_Pft;                //合相功率因数
  
  float   r_Freq;               //线频率

}meter_data_str;   


void ElectricCollectionProc(void );

uint32_t* ExportAll(void);

#endif
