/*
 * 系统储存应用程序
 */
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "delay.h"
#include "eeprom_drv.h"
#include "mb.h"
#include "rtc_drv.h"
#include "led_drv.h"
#include "switch_drv.h"
#include "ht7036_drv.h"
#include "modbus_slave.h"
#include "store_app.h"
#include "surgemdl_drv.h"
#include "systeminf.h"


static const SysParam_t g_sysParamDefault =
{
  .magicCode = MAGIC_CODE,
	.modbusAddr = 1,
  .modbusBaud = 9600,
  .LightningNum = 0,

};

static SysParam_t g_sysParamAlive;


#define SYSPARAM_MAX_SIZE              200
#define SYSPARAM_START_ADDR            0
#define BACKUP_START_ADDR              128

static uint8_t CalcCrc8(uint8_t *buf, uint32_t len)
{
    uint8_t crc = 0xFF;

    for (uint8_t byte = 0; byte < len; byte++)
    {
      crc ^= (buf[byte]);
      for (uint8_t i = 8; i > 0; --i)
      {
        if (crc & 0x80)
        {
          crc = (crc << 1) ^ 0x31;
        }
        else 
        {	
          crc = (crc<<1);
        }
      }
    }
    return crc;
}

static bool ReadDataWithCheck(uint8_t readAddr, uint8_t *pBuffer, uint16_t numToRead)
{
	if (!ReadEepromData(readAddr, pBuffer, numToRead))
	{
		return false;
	}
	uint8_t crcVal = CalcCrc8(pBuffer, numToRead - 1);
	if (crcVal != pBuffer[numToRead - 1])
	{
		return false;
	}
	return true;
}

bool ReadSysParam(SysParam_t *sysParam)
{
	uint16_t sysParamLen = sizeof(SysParam_t);
	
	if (ReadDataWithCheck(SYSPARAM_START_ADDR, (uint8_t *)sysParam, sysParamLen))
	{
		return true;
	}
	if (ReadDataWithCheck(BACKUP_START_ADDR, (uint8_t *)sysParam, sysParamLen))
	{
		return true;
	}
	return false;
}

static bool WriteDataWithCheck(uint8_t writeAddr, uint8_t *pBuffer, uint16_t numToWrite)
{
	pBuffer[numToWrite - 1] = CalcCrc8(pBuffer, numToWrite - 1);
	if (!WriteEepromData(writeAddr, pBuffer, numToWrite))
	{
		return false;
	}
	return true;
}
	
bool WriteSysParam(SysParam_t *sysParam)
{
	uint16_t sysParamLen = sizeof(SysParam_t);
	if (sysParamLen > SYSPARAM_MAX_SIZE)
	{
		return false;
	}
	if (!WriteDataWithCheck(SYSPARAM_START_ADDR, (uint8_t *)sysParam, sysParamLen))
	{
		return false;
	}
	
	WriteDataWithCheck(BACKUP_START_ADDR, (uint8_t *)sysParam, sysParamLen);
	
	return true;
}

void InitSysParam(void)
{
	SysParam_t sysParam;
	printf ("存储结构体长度  %d\r\n", sizeof(SysParam_t) );
  
  uint8_t Sta ;
  Sta = GetSwitchStatus ();
  
	if ( ReadSysParam(&sysParam) && (sysParam.magicCode == MAGIC_CODE) )
	{	
		g_sysParamAlive = sysParam;
    
    for(uint8_t i = 0; i < 3; i++)
    {
      
      WriteAdjustRegister (0x17+i, g_sysParamAlive.w_UgainRegister[i]);     //相电压增益校准
      WriteAdjustRegister (0x1A+i, g_sysParamAlive.w_IgainRegister[i]);     //相电流增益校准
      WriteAdjustRegister (0x04+i, g_sysParamAlive.w_PgainRegister[i]);     //有功功率增益校准
      WriteAdjustRegister (0x07+i, g_sysParamAlive.w_PgainRegister[i]);     //无功功率增益校准
      
      if( Sta == r_Customer )
      {
        WriteAdjustRegister (0x0D+i, g_sysParamAlive.w_PhasegainRegister[i] );   //功率角相位校正0
        WriteAdjustRegister (0x10+i, g_sysParamAlive.w_PhasegainRegister[i]);    //功率角相位校正1
        WriteAdjustRegister (0x61+i, g_sysParamAlive.w_PhasegainRegister[i]);    //功率角相位校正2
      }
      else if ( Sta == r_Calib_Phase )
      {
        WriteAdjustRegister (0x0D+i, 0x0000 );                              //功率角相位校正0
        WriteAdjustRegister (0x10+i, 0x0000);                               //功率角相位校正1
        WriteAdjustRegister (0x61+i, 0x0000);                               //功率角相位校正2
      }
    }    
    printf ("读取HT7036相位校正寄存器值  %#x %#x %#x\r\n", 
             ReadAdjustRegister(0x0D) ,ReadAdjustRegister(0x0E),ReadAdjustRegister(0x0F));
		return;
	}
	g_sysParamAlive = g_sysParamDefault;
  

}


bool SetGainBuffParam(uint32_t Buff, uint8_t PhaseType)
{
  SysParam_t sysParam;
  
	if (Buff == g_sysParamAlive.Gain[PhaseType] )
	{
		return false;
	} 
  if (ReadSysParam(&sysParam) && sysParam.magicCode == MAGIC_CODE)
	{	
		g_sysParamAlive = sysParam;
  }	
	g_sysParamAlive.Gain[PhaseType] = Buff;
	
	if (!WriteSysParam(&g_sysParamAlive))
	{   
		return false;
	}  
	return true;
}

bool SetOffsetBuffParam(uint32_t Buff, uint8_t PhaseType)
{
  SysParam_t sysParam;
  
	if (Buff == g_sysParamAlive.Offset[PhaseType] )
	{
		return false;
	} 
  if (ReadSysParam(&sysParam) && sysParam.magicCode == MAGIC_CODE)
	{	
		g_sysParamAlive = sysParam;
  }	
	g_sysParamAlive.Offset[PhaseType] = Buff;
	
	if (!WriteSysParam(&g_sysParamAlive))
	{   
		return false;
	}  
	return true;
}

/**存储电压增益**/
bool SetUgainParam(uint32_t Buff, uint8_t  PhaseType)
{

  SysParam_t sysParam;
	if (Buff == g_sysParamAlive.w_UgainRegister[PhaseType] )
	{
		return false;
	} 
  if (ReadSysParam(&sysParam) && sysParam.magicCode == MAGIC_CODE)
	{	
		g_sysParamAlive = sysParam;
  }	
	g_sysParamAlive.w_UgainRegister [PhaseType] = Buff;
	
	if (!WriteSysParam(&g_sysParamAlive))
	{
    
		return false;
	}
  
	return true;
}

/**存储电流增益**/
bool SetIgainParam(uint32_t Buff, uint8_t  PhaseType)
{
  SysParam_t sysParam;
	if (Buff == g_sysParamAlive.w_IgainRegister[PhaseType] )
	{
		return false;
	} 
  if (ReadSysParam(&sysParam) && sysParam.magicCode == MAGIC_CODE)
	{	
		g_sysParamAlive = sysParam;
  }	
	g_sysParamAlive.w_IgainRegister [PhaseType] = Buff;
	
	if (!WriteSysParam(&g_sysParamAlive))
	{   
		return false;
	}  
	return true;
}

/**存储有功功率增益**/
bool SetPgainParam(uint32_t Buff, uint8_t  PhaseType)
{
  SysParam_t sysParam;
	if (Buff == g_sysParamAlive.w_PgainRegister[PhaseType] )
	{
		return false;
	} 
  if (ReadSysParam(&sysParam) && sysParam.magicCode == MAGIC_CODE)
	{	
		g_sysParamAlive = sysParam;
  }	
	g_sysParamAlive.w_PgainRegister [PhaseType] = Buff;
	
	if (!WriteSysParam(&g_sysParamAlive))
	{   
		return false;
	}  
	return true;
}

/**存储相位校正增益**/
bool SetPhasegainParam(uint32_t Buff, uint8_t  PhaseType)
{
  SysParam_t sysParam;
	if (Buff == g_sysParamAlive.w_PhasegainRegister[PhaseType] )
	{
		return false;
	} 
  if (ReadSysParam(&sysParam) && sysParam.magicCode == MAGIC_CODE)
	{	
		g_sysParamAlive = sysParam;
  }	
	g_sysParamAlive.w_PhasegainRegister[PhaseType] = Buff;
	
	if (!WriteSysParam(&g_sysParamAlive))
	{   
		return false;
	}  
	return true;
}


bool SetModbusAddrParam(uint8_t addr)
{
	if (addr == g_sysParamAlive.modbusAddr  )
	{
		return true;
	}	 
	SysParam_t sysParam = g_sysParamAlive;
	sysParam.modbusAddr = addr;	
	if (eMBSetSlaveAddr(addr) != MB_ENOERR)
	{
		return false;
	}
	if (!WriteSysParam(&sysParam))
	{
		eMBSetSlaveAddr(g_sysParamAlive.modbusAddr);
		return false;
	}
	g_sysParamAlive = sysParam;

	return true;
}


bool SetModbusBaudParam(uint32_t buad)
{
	if (buad == g_sysParamAlive.modbusBaud   )
	{
		return true;
	}
	 
	SysParam_t sysParam = g_sysParamAlive;
	sysParam.modbusBaud = buad;

	if (!WriteSysParam(&sysParam))
	{
		
		return false;
	}
	g_sysParamAlive = sysParam;

	return true;
}



bool SetAnalogTypeParam(uint8_t  Buff, uint8_t Route)
{
  SysParam_t sysParam;
	if (Buff == g_sysParamAlive.AnalogType [Route] )
	{
		return false;
	} 
  if (ReadSysParam(&sysParam) && sysParam.magicCode == MAGIC_CODE)
	{	
		g_sysParamAlive = sysParam;
  }	
	g_sysParamAlive.AnalogType [Route] = Buff;
	
	if (!WriteSysParam(&g_sysParamAlive))
	{   
		return false;
	}  
	return true;
}




/**
***********************************************************
* @brief 获取modbus地址
* @param
* @return 雷击次数
***********************************************************
*/

uint8_t  GetModbusAddrData(void)
{ 
	return g_sysParamAlive.modbusAddr  ;
}

/**
***********************************************************
* @brief 获取modbus波特率
* @param
* @return 
***********************************************************
*/

uint32_t  GetModbusBaudData(void)
{ 
	return g_sysParamAlive.modbusBaud ;
}


/**
***********************************************************
* @brief 获取增益数据
* @param
* @return 
***********************************************************
*/

uint32_t*  GetGainData(void)
{ 
	return g_sysParamAlive.Gain ;
}

/**
***********************************************************
* @brief 获取偏置数据
* @param
* @return 
***********************************************************
*/

uint32_t*  GetOffsetData(void)
{ 
	return g_sysParamAlive.Offset ;
}


uint8_t*  GetAnalogTypeData(void)
{ 
	return g_sysParamAlive.AnalogType  ;
}

 


