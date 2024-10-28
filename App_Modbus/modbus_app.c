#include <stdint.h>
#include <stdlib.h>
#include "modbus_slave.h"
#include "mb.h"
#include "mbcrc.h"
#include "sensor_drv.h"
#include "led_drv.h"
//#include "signal_drv.h"
//#include "norflash_drv.h"
#include "store_app.h"
#include "stm32f10x.h"



static void ModbusGetDispersed(uint16_t *value , USHORT  , USHORT);

static void ModbusGetPhaseURms_A(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_URms [0] );
}

static void ModbusGetPhaseURms_B(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_URms [1] );
}
static void ModbusGetPhaseURms_C(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_URms [2] );
}
static void ModbusGetPhaseIRms_A(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_IRms [0] );
}
static void ModbusGetPhaseIRms_B(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_IRms [1] );
}
static void ModbusGetPhaseIRms_C(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_IRms [2] );
}

static void ModbusGetPhaseP_A(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_P [0] );
}
static void ModbusGetPhaseP_B(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_P [1] );
}
static void ModbusGetPhaseP_C(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_P [2] );
}
static void ModbusGetPhasePt(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_Pt  );
}
static void ModbusGetPhaseQ_A(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_Q [0] );
}
static void ModbusGetPhaseQ_B(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_Q [1] );
}
static void ModbusGetPhaseQ_C(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_Q [2] );
}
static void ModbusGetPhaseQt(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_Qt  );
}
static void ModbusGetPhaseS_A(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_S [0] );
}
static void ModbusGetPhaseS_B(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_S [1] );
}
static void ModbusGetPhaseS_C(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_S [2] );
}
static void ModbusGetPhaseSt(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_St  );
}
static void ModbusGetPhasePft(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_Pft  );
}
static void ModbusGetPhaseFreq(uint16_t *value)
{
	SensorData_str* sensorData;
	sensorData = GetSensorData();
	*value = (uint16_t)(sensorData->meter_data .r_Freq  );
}

static void ModbusSetLed1(uint16_t value)
{
	value == 0 ? TurnOffLed(LED1) : TurnOnLed(LED1);
}

static void ModbusSetLed2(uint16_t value)
{
	value == 0 ? TurnOffLed(LED2) : TurnOnLed(LED2);
}
/**
***********************************************************
* @brief 保持寄存器相关句柄
* @param
* @return 
***********************************************************
*/
#define R   (1 << 0)
#define W   (1 << 1)

typedef struct {
    uint8_t  property;				// 读写的属性
    const uint16_t address;		// 地址
    uint16_t minValue;				// 最小值
    uint16_t maxValue;				// 最大值
    void (*ReadCb)(uint16_t *value);
    void (*WriteCb)(uint16_t value); 
} MbRegisterInstance_t;

static MbRegisterInstance_t g_regInstanceTab[] = {
    {
        .property = R,
        .address = 0x0000,          // A相电压
		.ReadCb = ModbusGetPhaseURms_A,
    },
    {
        .property = R,
        .address = 0x0001,          // B相电压
		.ReadCb = ModbusGetPhaseURms_B,
    },
    {
        .property = R,
        .address = 0x0002,          // C相电压
		.ReadCb = ModbusGetPhaseURms_C,
    },
    {
        .property = R,
        .address = 0x0003,          // A相电流
		.ReadCb = ModbusGetPhaseIRms_A,
    },
    {
        .property = R,
        .address = 0x0004,          // B相电流
		.ReadCb = ModbusGetPhaseIRms_B,
    },
    {
        .property = R,
        .address = 0x0005,          // C相电流
		.ReadCb = ModbusGetPhaseIRms_C,
    },
    {
        .property = R,
        .address = 0x0006,          // A相有功功率
		.ReadCb = ModbusGetPhaseP_A,
    },
    {
        .property = R,
        .address = 0x0007,           // B相有功功率
		.ReadCb = ModbusGetPhaseP_B,
    },
    {
        .property = R,
        .address = 0x0008,           // C相有功功率
		.ReadCb = ModbusGetPhaseP_C,
    },                            
    {
        .property = R,
        .address = 0x0009,           // 合相有功功率
		.ReadCb = ModbusGetPhasePt,
    },
    {
        .property = R,
        .address = 0x000A,          // A相无功功率
		.ReadCb = ModbusGetPhaseQ_A,
    },
    {
        .property = R,
        .address = 0x000B,           // B相无功功率
		.ReadCb = ModbusGetPhaseQ_B,
    },
    {
        .property = R,
        .address = 0x000C,           // C相无功功率
		.ReadCb = ModbusGetPhaseQ_C,
    },                            
    {
        .property = R,
        .address = 0x000D,           // 合相无功功率
		.ReadCb = ModbusGetPhaseQt,
    },
    {
        .property = R,
        .address = 0x000E,           // A相视在功率
		.ReadCb = ModbusGetPhaseS_A,
    },
    {
        .property = R,
        .address = 0x000F,           // B相视在功率
		.ReadCb = ModbusGetPhaseS_B,
    },
    {
        .property = R,
        .address = 0x0010,           // C相视在功率
		.ReadCb = ModbusGetPhaseS_C,
    },   
    {
        .property = R,
        .address = 0x0011,           // 合相视在功率
		.ReadCb = ModbusGetPhaseSt,
    },
    {
        .property = R,
        .address = 0x0012,           // 合相功率因数
		.ReadCb = ModbusGetPhasePft,
    },   
    {
        .property = R,
        .address = 0x0013,           // 线频率
		.ReadCb = ModbusGetPhaseFreq,
    },    

    
    {
        .property = R | W,
        .address = 0x000A,          // LED1开关 01 06 00 02 00 01  ,LED1 LED2 01 10 00 02 00 02 04 00 01 00 01
        .minValue = 0,
        .maxValue = 1,
		.WriteCb = ModbusSetLed1,
    },
    {
        .property = R | W,
        .address = 0x000B,          // LED2开关 01 06 00 03 00 01
        .minValue = 0,
        .maxValue = 1,
		.WriteCb = ModbusSetLed2,
    },
    
   
};
#define REG_TABLE_SIZE         (sizeof(g_regInstanceTab) / sizeof(g_regInstanceTab[0]))

static eMBErrorCode ReadRegsCb(uint8_t startAddr, uint8_t regNum, uint8_t *buf)
{
    if (buf == NULL)
    {
          return MB_EINVAL;
    }
    
    for (uint32_t i = 0; i < regNum; i++)
    {
      MbRegisterInstance_t *instance = NULL;
      for (uint32_t j = 0; j < REG_TABLE_SIZE; j++)
      {
        if (g_regInstanceTab[j].address != startAddr + i)
        {
          continue;
        }
        instance = &g_regInstanceTab[j];
        if((instance->property & R) == 0)  //读写检验
        {
          return MB_EINVAL;
        }

        if (instance->ReadCb != NULL)
        {
          instance->ReadCb((uint16_t *)&buf[2 * i]);
        }
      }
      if (instance == NULL)
      {
        return MB_ENOREG;
      }		
    }
   return MB_ENOERR;
}
  
static eMBErrorCode WriteRegsCb(uint8_t startAddr, uint8_t regNum, uint8_t *buf)
{
  if (buf == NULL)
	{
        return MB_EINVAL;
	}
	
  for (uint32_t i = 0; i < regNum; i++)
	{
		
		MbRegisterInstance_t *instance = NULL;
		for (uint32_t j = 0; j < REG_TABLE_SIZE; j++)
		{
			if (g_regInstanceTab[j].address != startAddr + i)
			{
				continue;
			}
			instance = &g_regInstanceTab[j];
			
		    if((instance->property & W) == 0)  //读写检验
			{
				return MB_EINVAL;
			}
			
			uint16_t setValue = ((buf[2 * i] << 8) & 0xFF00) | (buf[2 * i + 1] & 0xFF);
			if ((setValue < instance->minValue) || (setValue > instance->maxValue))
			{
				return MB_EINVAL;
			}
			
			if (instance->WriteCb != NULL)
			{
				instance->WriteCb(setValue);
			}
		}
		
		if (instance == NULL)
		{
			return MB_ENOREG;
		}
	}
  return MB_ENOERR;
}





/**
***********************************************************
* @brief 离散输入相关句柄
* @param
* @return 
***********************************************************
*/
typedef struct {    
    const uint16_t address;		// 地址    
    void (*ReadDICb)(uint16_t *value , USHORT  , USHORT );  

} MbRegDiscreteInstance_t;

static MbRegDiscreteInstance_t g_regdiscreteInstanceTab[] = {
    {        
      .address = 0x0100,        
      .ReadDICb = ModbusGetDispersed
    },  
};
#define REGDISCRETE_TABLE_SIZE         (sizeof(g_regdiscreteInstanceTab) / sizeof(g_regdiscreteInstanceTab[0]))

#define REG_DISCRETE_START    0x0100    //开关寄存器其实地址

#define REG_DISCRETE_SIZE     2         //开关寄存器数量

static eMBErrorCode ReadDiscreteInputsCb( uint8_t *buf ,USHORT usAddress, USHORT usNDiscrete)
{
    if (buf == NULL)
    {
          return MB_EINVAL;
    }
    
    MbRegDiscreteInstance_t *instance = NULL;
    instance = &g_regdiscreteInstanceTab[REGDISCRETE_TABLE_SIZE-1];
    
    int  iRegIndex;      
    iRegIndex = (int)(usAddress + usNDiscrete - REG_DISCRETE_START);
    
    if((usAddress >= REG_DISCRETE_START)&&\
    ((usAddress+usNDiscrete) <= (REG_DISCRETE_START + REG_DISCRETE_SIZE)))
    {
      if (instance->ReadDICb   != NULL)
			{       
       instance->ReadDICb ((uint16_t *)buf , usNDiscrete , iRegIndex);
      }
      if (instance == NULL)
      {
        return MB_ENOREG;
      }
    }
    else
    {
        return MB_ENOREG;
    }
   return MB_ENOERR;

}

//模拟离散寄存器内容
//uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE] = {0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x01}; 

static void ModbusGetDispersed(uint16_t *value ,USHORT usNDiscrete ,USHORT  Index)
{
//	USHORT coilValue=0x0000;
//  USHORT readNumber = usNDiscrete;
//  
//  uint8_t* STA = GetDiscreteinputStatus();   
// 
//  for(uint8_t i = 0; i < usNDiscrete; i++)
//  {
//      readNumber--;
//      Index--;
//      coilValue|= STA[Index]<<readNumber;
//  }

//  * value = coilValue;
}


void ModbusAppInit(void)
{
	ModbusSlaveInstance_t mbInstace = {
        .slaveAddr = GetModbusAddrData(),
        .baudRate = GetModbusBaudData(),
        .cb.ReadRegs = ReadRegsCb,
        .cb.WriteRegs = WriteRegsCb,
        .cb.ReadDiscreteInputs = ReadDiscreteInputsCb,      
    };
	ModbusSlaveInit(&mbInstace);
}

void ModbusTask(void)
{
	(void)eMBPoll();
}


