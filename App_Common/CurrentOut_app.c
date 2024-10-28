
#include <stdbool.h>
#include <stdio.h>
#include "delay.h"
#include "switch_drv.h"
#include "gp8202as_drv.h"
#include "store_app.h"


uint8_t sta ;
uint32_t* miden;
void CurrrntOutTask(void )
{
  static bool Flag = false ;
  float  g_offset[3] ,g_slope[3];
  sta = GetSwitchStatus ();
  if( sta == Calibration)    //1-ON, 2-ON
  {  
    AutoCalibrationPro();   
  }
  else if( sta == Normal)   //1-OFF, 2-OFF
  {
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
    
    if( Flag == false )
    {
      printf("存储输出比例   %f   %f   %f \r\n",g_offset[0] ,g_offset[1], g_offset[2]); 
      printf("存储输出偏置   %f   %f   %f \r\n",g_slope[0] ,g_slope[1], g_slope[2]);  
      Flag = true ; 
    }
       
    //测试输出5.0mA
    gp8211s_OutPutCurrent( HardIDofFirst, 0x6666 , g_offset[0] ,g_slope[0] );   
    gp8211s_OutPutCurrent( HardIDofSecond, 0xCCCC , g_offset[1] ,g_slope[1] );
    gp8211s_OutPutCurrent( HardIDofThird, 0x51EB , g_offset[2] ,g_slope[2] );  
  }
}

