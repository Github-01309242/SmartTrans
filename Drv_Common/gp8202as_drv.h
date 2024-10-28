#ifndef __GP8202AS_DRV_H
#define	__GP8202AS_DRV_H

#include <stdint.h>

#define HardIDofFirst  0
#define HardIDofSecond 1
#define HardIDofThird  2

#define LowerReferCurrent  0x28F5                  //4mA
#define MiddleReferCurrent 0x6666                  //10mA
#define UpperReferCurrent  0XCCCC                  //20mA


typedef enum
{
 Standard_value_4,   //标准4.0mA - 0x28F5
 Standard_value_10,  //标准10.0mA - 0x6666
 Parameter_infer,    //参数推导
 OTHER ,             //其他
}Calibration_Type; 

void Gp8202asDrvInit ( void );
void Gp8202as_Test(void );
void Send_gp8202as_data(uint8_t order , uint16_t DAC_data);
void AutoCalibrationPro( void );
void gp8211s_OutPutCurrent(uint8_t Order ,uint16_t Median  , float Offset ,float Slope );
float* Get_Offset(void );
float* Get_Slope(void );

#endif
