
#include <stdbool.h>
#include <stdio.h>
#include "delay.h"
#include "switch_drv.h"
#include "gp8202as_drv.h"
#include "store_app.h"
#include "energy_meter_drv.h"
#include "analogout_app.h"
#include "hmi_app.h"

#define P_Active_Power 3300
#define N_Active_Power -3300

#define OUT_PUT_20mA  0XCCCC
#define OUT_PUT_4mA   0X28F5

#define Power_k     6.355f
#define Power_b     31456.5f 

#define I_k         8388.6f
#define I_b         10485.0f

#define U_k         190.65f
#define U_b         10485.0f


static void Gp8211s_AnalogOutput(uint8_t Order, float Median, analog_class type )
{	
  uint16_t  RegVal = 0;
  
	switch (type )
  {   
    case URms:
      RegVal = Median /10.0 *U_k + U_b ;
    
      Send_gp8202as_data(Order ,RegVal);
      break ;
    
    case IRms:
      RegVal = Median /100.0 *I_k + I_b ;
    
      Send_gp8202as_data(Order ,RegVal);
      break ;
    
    case Power :
      RegVal = Median /10.0 *Power_k + Power_b ;
    
      Send_gp8202as_data(Order ,RegVal);
      break ;      
    
    case Pft:
      RegVal = Median /100.0 *I_k + I_b ;

      Send_gp8202as_data(Order ,RegVal);
      break ;
    
    case Freq:
      RegVal = Median /100.0 *I_k + I_b ;

      Send_gp8202as_data(Order ,RegVal);
      break ;
            
    default:
			break;      
            
    
  
  }
}

//#define NeedCalib  

#ifdef NeedCalib
void CurrrntOutTask(void )
{
  uint8_t sta ;
  uint32_t* miden;
  static bool Flag = false ;
  float  g_offset[3] ,g_slope[3];
  
  miden = GetOffsetData();
  /**偏置数据**/
  g_offset[0] = *(miden)/1000.0f;
  g_offset[1] = *(miden+1)/1000.0f;
  g_offset[2] = *(miden+2)/1000.0f;
  /**增益数据**/
  miden = GetGainData();
  g_slope[0] = *(miden)/1000.0f;
  g_slope[1] = *(miden+1)/1000.0f;
  g_slope[2] = *(miden+2)/1000.0f;
  
  sta = GetSwitchStatus ();
  //bit(0,0,0,0,INA,INB,INC,IND)
  if( sta == a_Calibration)    //1-ON, 2-ON
  {  
    AutoCalibrationPro();   
  }
  else if( sta == a_CustomizedTesting)   //1-OFF, 2-OFF
  {
    if( Flag == false )
    {
      printf("存储输出比例   %f   %f   %f \r\n",g_offset[0] ,g_offset[1], g_offset[2]); 
      printf("存储输出偏置   %f   %f   %f \r\n",g_slope[0] ,g_slope[1], g_slope[2]);  
      Flag = true ; 
    }
       
    //测试输出10.0mA
    gp8211s_OutPutCurrent( HardIDofFirst, 0x6666 , g_offset[0] ,g_slope[0] );   
    gp8211s_OutPutCurrent( HardIDofSecond, 0x6666 , g_offset[1] ,g_slope[1] );
    gp8211s_OutPutCurrent( HardIDofThird, 0x6666 , g_offset[2] ,g_slope[2] );  
  }
  else if( sta == customer)      //提供给客户的模拟量输出
  {
    meter_data_str g_meter_data_str;
    uint16_t g_median;
    Export(&g_meter_data_str);        
    g_median = (uint16_t )(6.355* g_meter_data_str.Active_Power + 31456.5);
    gp8211s_OutPutCurrent( HardIDofFirst, g_median , g_offset[0] ,g_slope[0] );      //I0 对应合相有功功率
    g_median = (uint16_t )(6.355* g_meter_data_str.Reactive_Power + 31456.5);
    gp8211s_OutPutCurrent( HardIDofSecond, g_median , g_offset[1] ,g_slope[1] );     //I1 对应合相无功功率
    g_median = (uint16_t )(83.886* g_meter_data_str.I_Curr  + 10485);
    gp8211s_OutPutCurrent( HardIDofThird, g_median , g_offset[2] ,g_slope[2] );      //I2 对应A相电流有效值
  }
}

#else 

/******************************************************************************
函数说明：解析模拟量类型
入口数据：data
返回值：  analog_class
******************************************************************************/
analog_class Assay(uint8_t data )
{
  if( data < 3)
  {
   return URms ;
  } 
  else if   ( data > 2  && data < 6 )
  {
   return IRms ;
  }
  else if   ( data > 5  && data < 18 )
  {
   return Power ;
  }
  else if   ( data == 18 )
  {
   return Pft ;
  } 
  else if   ( data == 19 )
  {
   return Freq ;
  }  
  return Other;
}


/******************************************************************************
函数说明：模拟量输出函数
入口数据：
返回值：  
******************************************************************************/
static uint32_t* g_RmsData = NULL ;
static uint8_t * g_Analog_Type_Index = NULL  ;


void AnalogOutTask(void )
{    
  uint8_t  Analog_Type[3];
  
  analog_class g_Analog_Type;
  
  g_Analog_Type_Index = GetAnalogTypeData();
  
  Analog_Type[0] = *g_Analog_Type_Index;
  Analog_Type[1] = *(g_Analog_Type_Index+1);
  Analog_Type[2] = *(g_Analog_Type_Index+2);
    
  printf ("选择的3路输出量   %d,  %d,  %d ",*g_Analog_Type_Index, *(g_Analog_Type_Index+1), *(g_Analog_Type_Index+2));
  
  g_RmsData = ExportAll();
  
  for (uint8_t i =0; i <3; i++)
  { 
    g_Analog_Type = Assay(Analog_Type[i]); 
    
    Gp8211s_AnalogOutput(i, (*(float *)(g_RmsData + Analog_Type[i])),  g_Analog_Type);
    
    DelayNms (100);
  
  }
}

#endif










