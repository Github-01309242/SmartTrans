
#ifndef _SWITCH_DRV_H_
#define _SWITCH_DRV_H_

#include <stdint.h>
//bit(0,0,0,0,INA,INB,INC,IND)
/**4-20mA**/
#define a_Calibration             0x00           //0b00000000  

#define a_CustomizedTesting       0x0C           //0b00001100   定值测试

/**电量采集**/
//电压电流功率校正
#define r_Calib_UIP               0x03           //0b00000011

//相位校正
#define r_Calib_Phase             0x07           //0b00000111

/**客户使用运行状态**/
#define r_Customer                0x0F           //0b00001111

/**
***********************************************************
* @brief 拨码开关硬件初始化
* @param
* @return 
***********************************************************
*/
void SwitchDrvInit(void);

/**
***********************************************************
* @brief 获取拨码开关状态
* @param
* @return 0-校准状态
          1-有效值状态
***********************************************************
*/
uint8_t GetSwitchStatus(void);

#endif

