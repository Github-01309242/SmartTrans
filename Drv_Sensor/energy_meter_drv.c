
#include <stdbool.h>
#include <stdio.h>
#include "delay.h"
#include "led_drv.h"
#include "switch_drv.h"
#include "ht7036_drv.h"
#include "energy_meter_drv.h"
#include "store_app.h"

meter_data_str PowerData;    
 
uint32_t* w_Ugain;
uint32_t* w_Igain;
uint32_t* w_Pgain;
uint32_t* w_Phasegain;
uint32_t N_den ;
uint32_t RegValue;
float value ;


void ElectricCollectionProc(void )
{
  uint8_t Sta ;
  bool g_pass = false ;
   
  Sta = GetSwitchStatus ();
  /***校准电压 电流 功率****/
  if( Sta ==  r_Calib_UIP)
  {  
    w_Ugain  = Get_UgainData(220.0);
    w_Igain  = Get_IgainData(5.0);
    w_Pgain  = Get_PgainData(1100.0);
    
    for( uint8_t i = 0 ;i < 3; i++)
    {
     SetUgainParam(*(w_Ugain+i), i);
     SetIgainParam(*(w_Igain+i), i); 
     SetPgainParam(*(w_Pgain+i), i);
    }
    
    while(1)
    {
     ToggleLed(1);   
     DelayNms (100);
    }
    
  }
  /******校准相位********/
  else if ( Sta == r_Calib_Phase )
  {
    w_Phasegain = Get_PhasegainData(550.0);
    
    for( uint8_t i = 0 ;i < 3; i++)
    {
     g_pass = SetPhasegainParam(*(w_Phasegain+i), i);
    }
    
    printf ("写入EEPROM内的相位校正值  %#x %#x %#x\r\n", *w_Phasegain ,*(w_Phasegain+1),*(w_Phasegain+2));
    
    while(g_pass)
    {
     ToggleLed(1);   
     DelayNms (200);
    }
  
  }  
  
  /***客户应用场景***/
  else if ( Sta ==  r_Customer )
  {
//    PowerData.r_URms [0]  = 0.0f;
//    PowerData.r_URms [1]  = 1100.0f;
//    PowerData.r_URms [2]  = 2200.0f;
    
    /**相电压有效值*10倍、电流有效值*100倍**/
    for( uint8_t i = 0; i<3; i++)
    {
      PowerData.r_URms [i]  = ReadSampleRegister (0X0D + i ) / 8192 *10;
      PowerData.r_IRms [i]  = ReadSampleRegister (0X10 + i ) / 8192 / UI_K *100;
    }
    
    /**分相有功功率*10倍**/
    for(uint8_t i =0 ;i < 3; i++)
    {
      RegValue = ReadSampleRegister (0X01 + i ) ;
      if( RegValue > 8388608)
      {
        RegValue = RegValue - 16777216 ;
        RegValue = ~RegValue ;       
        PowerData.r_P [i] =  -1* ( RegValue * P_K ) *10 ; 
      }
      else 
      {  
        RegValue =RegValue;  
        PowerData.r_P [i]  = RegValue * (1)* P_K *10 ;        
      }
    }
    /**合相有功功率*10倍**/
    RegValue = ReadSampleRegister (0X04 ) ;
    if( RegValue > 8388608)
    {
      RegValue = RegValue - 16777216 ;
      RegValue = ~RegValue ;       
      PowerData.r_Pt   =  -1* (RegValue * 2 * P_K ) * 10 ; 
    }
    else 
    {  
      RegValue = RegValue;  
      PowerData.r_Pt  = 1* RegValue * 2 * P_K * 10 ;        
    }
    
    
    /**分相无功功率*10倍**/
    for(uint8_t i =0 ;i < 3; i++)
    {
      RegValue = ReadSampleRegister (0X05 + i ) ;
      if( RegValue > 8388608)
      {
        RegValue = RegValue - 16777216 ;
        RegValue = ~RegValue ;       
        PowerData.r_Q [i] =  -1 * (RegValue * P_K) * 10 ; 
      }
      else 
      {  
        RegValue =RegValue;  
        PowerData.r_Q [i] = RegValue * (1) * P_K * 10;        
      }     
    }
    /**合相无功功率*10倍**/
    RegValue = ReadSampleRegister (0X08 ) ;
    if( RegValue > 8388608)
    {
      RegValue = RegValue - 16777216 ;
      RegValue = ~RegValue ;       
      PowerData.r_Qt   =  -1* (RegValue * 2 * P_K) * 10; 
    }
    else 
    {  
      RegValue = RegValue;  
      PowerData.r_Qt  = 1* RegValue * 2 * P_K * 10 ;        
    }
    
    /**分相视在功率*10倍**/
    for(uint8_t i =0 ;i < 3; i++)
    {
      RegValue = ReadSampleRegister (0X09 + i ) ;
      if( RegValue > 8388608)
      {
       RegValue = RegValue - 16777216 ;
      }
      else RegValue =RegValue;
      PowerData.r_S [i] = RegValue * P_K * 10 ;
    }
    
    /**合相视在功率*10倍**/
    RegValue = ReadSampleRegister (0X0C ) ;
    if( RegValue > 8388608)
    {
      RegValue = RegValue - 16777216 ;      
    }
    else 
    {  
      RegValue = RegValue;      
    }
    PowerData.r_St = RegValue * 2 * P_K * 10;
    
    /**合相功率因数*1000倍**/
    RegValue = ReadSampleRegister ( 0X17 ) ;
    if( RegValue > 8388608)
    {
      RegValue = RegValue - 16777216 ;
      RegValue = ~RegValue ;       
      PowerData.r_Pft  =  -1* (RegValue * 1.0f / 8388608) *1000; 
    }
    else 
    {  
      RegValue = RegValue;  
      PowerData.r_Pft  = RegValue * 1.0f * (1)/ 8388608 *1000;        
    }     

    /***线频率*100倍***/
    RegValue = ReadSampleRegister ( 0X1C ) ;
    PowerData.r_Freq = RegValue / 8192.0 *100;
          
    /*
    printf("相电压*10           %f   %f   %f \r\n",PowerData.r_URms [APhase] ,PowerData.r_URms [BPhase], PowerData.r_URms [CPhase]);  
    printf("相电流*100          %f   %f   %f \r\n",PowerData.r_IRms [APhase] ,PowerData.r_IRms [BPhase], PowerData.r_IRms [CPhase]);
    printf("分相有功功率        %f   %f   %f \r\n",PowerData.r_P [APhase] ,PowerData.r_P [BPhase] , PowerData.r_P [CPhase] );  
    printf("合相有功功率        %f           \r\n",PowerData.r_Pt);  
    printf("分相无功功率        %f   %f   %f \r\n",PowerData.r_Q [APhase] ,PowerData.r_Q [BPhase], PowerData.r_Q [CPhase]); 
    printf("合相无功功率        %f           \r\n",PowerData.r_Qt); 
    printf("视在功率            %f   %f   %f \r\n",PowerData.r_S [APhase]  ,PowerData.r_S [BPhase], PowerData.r_S [CPhase]);
    printf("分相功率因数*1000   %f   %f   %f \r\n",PowerData.r_Pf [APhase] ,PowerData.r_Pf [BPhase], PowerData.r_Pf [CPhase]);
    printf("分相功率角*100      %d   %d   %d \r\n",Phase_PowerData[APhase].Power_Angle  ,Phase_PowerData[BPhase].Power_Angle, Phase_PowerData[CPhase].Power_Angle);    

    N_den   = (ReadSampleRegister (0X10 ) / 8192 ) ;
    N = N_den/5.0f;
    printf("比例系数N =   %f\r\n",N);       
    */
  }
}


 /**
***********************************************************
* @brief 所有数据输出接口
* @param
* @return 
***********************************************************
*/
uint32_t* ExportAll(void)
{
  return (uint32_t *)(&PowerData);
}




