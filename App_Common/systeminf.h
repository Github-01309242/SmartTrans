#ifndef _SYSTEMINF_H_
#define _SYSTEMINF_H_

#include <stdint.h>

typedef struct {
    uint16_t  magicCode;
	
	/* 添加配置参数开始 */
    uint8_t modbusAddr;
    uint32_t modbusBaud;
    uint8_t LightningNum;
  
    uint32_t Gain[3];
    uint32_t Offset[3];
    	
    uint32_t w_UgainRegister[3];
    uint32_t w_IgainRegister[3];
    uint32_t w_PgainRegister[3];
    uint32_t w_QgainRegister[3];
    uint32_t w_sgainRegister[3];    
    uint32_t w_PhasegainRegister[3];
  
    uint8_t  AnalogType[3];
   
	/* 添加配置参数结束 */
	
	uint8_t crcVal;
} SysParam_t;

#define MAGIC_CODE     0x5A5A

#endif

