
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
  /***У׼��ѹ ���� ����****/
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
  /******У׼��λ********/
  else if ( Sta == r_Calib_Phase )
  {
    w_Phasegain = Get_PhasegainData(550.0);
    
    for( uint8_t i = 0 ;i < 3; i++)
    {
     g_pass = SetPhasegainParam(*(w_Phasegain+i), i);
    }
    
    printf ("д��EEPROM�ڵ���λУ��ֵ  %#x %#x %#x\r\n", *w_Phasegain ,*(w_Phasegain+1),*(w_Phasegain+2));
    
    while(g_pass)
    {
     ToggleLed(1);   
     DelayNms (200);
    }
  
  }  
  
  /***�ͻ�Ӧ�ó���***/
  else if ( Sta ==  r_Customer )
  {
//    PowerData.r_URms [0]  = 0.0f;
//    PowerData.r_URms [1]  = 1100.0f;
//    PowerData.r_URms [2]  = 2200.0f;
    
    /**���ѹ��Чֵ*10����������Чֵ*100��**/
    for( uint8_t i = 0; i<3; i++)
    {
      PowerData.r_URms [i]  = ReadSampleRegister (0X0D + i ) / 8192 *10;
      PowerData.r_IRms [i]  = ReadSampleRegister (0X10 + i ) / 8192 / UI_K *100;
    }
    
    /**�����й�����*10��**/
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
    /**�����й�����*10��**/
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
    
    
    /**�����޹�����*10��**/
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
    /**�����޹�����*10��**/
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
    
    /**�������ڹ���*10��**/
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
    
    /**�������ڹ���*10��**/
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
    
    /**���๦������*1000��**/
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

    /***��Ƶ��*100��***/
    RegValue = ReadSampleRegister ( 0X1C ) ;
    PowerData.r_Freq = RegValue / 8192.0 *100;
          
    /*
    printf("���ѹ*10           %f   %f   %f \r\n",PowerData.r_URms [APhase] ,PowerData.r_URms [BPhase], PowerData.r_URms [CPhase]);  
    printf("�����*100          %f   %f   %f \r\n",PowerData.r_IRms [APhase] ,PowerData.r_IRms [BPhase], PowerData.r_IRms [CPhase]);
    printf("�����й�����        %f   %f   %f \r\n",PowerData.r_P [APhase] ,PowerData.r_P [BPhase] , PowerData.r_P [CPhase] );  
    printf("�����й�����        %f           \r\n",PowerData.r_Pt);  
    printf("�����޹�����        %f   %f   %f \r\n",PowerData.r_Q [APhase] ,PowerData.r_Q [BPhase], PowerData.r_Q [CPhase]); 
    printf("�����޹�����        %f           \r\n",PowerData.r_Qt); 
    printf("���ڹ���            %f   %f   %f \r\n",PowerData.r_S [APhase]  ,PowerData.r_S [BPhase], PowerData.r_S [CPhase]);
    printf("���๦������*1000   %f   %f   %f \r\n",PowerData.r_Pf [APhase] ,PowerData.r_Pf [BPhase], PowerData.r_Pf [CPhase]);
    printf("���๦�ʽ�*100      %d   %d   %d \r\n",Phase_PowerData[APhase].Power_Angle  ,Phase_PowerData[BPhase].Power_Angle, Phase_PowerData[CPhase].Power_Angle);    

    N_den   = (ReadSampleRegister (0X10 ) / 8192 ) ;
    N = N_den/5.0f;
    printf("����ϵ��N =   %f\r\n",N);       
    */
  }
}


 /**
***********************************************************
* @brief ������������ӿ�
* @param
* @return 
***********************************************************
*/
uint32_t* ExportAll(void)
{
  return (uint32_t *)(&PowerData);
}




