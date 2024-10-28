#ifndef _MODBUS_SLAVE_H_
#define _MODBUS_SLAVE_H_

#include <stdint.h>
#include "mb.h"

typedef struct {
    eMBErrorCode (*ReadRegs)(uint8_t startAddr, uint8_t regNum, uint8_t *buf);
    eMBErrorCode (*WriteRegs)(uint8_t startAddr, uint8_t regNum, uint8_t *buf);
  
    eMBErrorCode (*ReadDiscreteInputs)( uint8_t *buf, USHORT startAddr, USHORT DiscreteCnt);
  
} ModbusFuncCb_t;

typedef struct {
    uint8_t slaveAddr;      //�ӻ���ַ
    uint32_t baudRate;      //������
    ModbusFuncCb_t cb;      //�ص�����
} ModbusSlaveInstance_t;

void ModbusSlaveInit(ModbusSlaveInstance_t *mbInstance);

#endif