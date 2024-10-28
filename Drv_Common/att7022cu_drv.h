#ifndef __ATT7022CU_DRV_H
#define __ATT7022CU_DRV_H	 

#include <stdint.h>

#define APhase  0x00
#define BPhase  0x01
#define CPhase  0x02

void att7022cu_drv_init(void );
void ResetATT7022(void);
//读计量寄存器参数
uint32_t  ReadSampleRegister(uint8_t cmd);
//写校表寄存器
void WriteAdjustRegister(uint8_t addr,uint32_t  data);
//读校表寄存器
uint32_t  ReadAdjustRegister(uint8_t addr);
//===============================================================
// 电压校正函数
// PhaseType : 相电压类型
// StandValue: 标准电压值
// AdjustBuff: 保存校表数据
//===============================================================
uint32_t AdjustVoltageFrun(float StandValue,uint8_t PhaseType);


#endif

