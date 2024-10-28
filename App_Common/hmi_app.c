
#include "stm32f10x.h"
#include "systick_drv.h"
#include "key_drv.h"
#include "delay.h"
#include "led_drv.h"
#include "max7219_drv.h"
#include "eeprom_drv.h"
#include "systick_drv.h"
#include "timing_drv.h"
#include "store_app.h"
#include "energy_meter_drv.h"
#include "analogout_app.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

  
/**
***********************************************************
* @brief 定义菜单枚举
* @param
* @return 
***********************************************************
*/
typedef enum
{
  Main=0,
  Code,
  Set,
  Dis,
  Conn,
  AO_1,
  AO_2,
  AO_3,
  Save,
  /**二级菜单**/
  AO_1_Child,
  AO_2_Child,
  AO_3_Child,
  
}UI_STATE;

UI_STATE UI_State;


 const char homepage_displayBuf[][20] ={
 {"UA  "},
 {"Ub  "},
 {"UC  "},
 {"IA  "},
 {"Ib  "},
 {"IC  "},
 {"PS  "},
 {"qS  "},
 {"SS  "},
 {"PF  "},
 {"F    "},
};


#define homepage_num  ( sizeof(homepage_displayBuf) / sizeof(homepage_displayBuf[0]) )


static void MainUI(uint8_t  index_main )
{ 
  static uint32_t* g_RmsData;
  char strBuf[10]; 
  char result[20];
  float miden;
     
  g_RmsData = ExportAll();
  
  if( index_main < 3)
  {
    miden = (*(float *)(g_RmsData + index_main ))/10;
      
    sprintf(strBuf, "%5.1f", miden );

    sprintf(result, "%s%s ", homepage_displayBuf[index_main], strBuf);

    DisplayText(result,LEFT);  
  
  } 
  
  else if( index_main > 2 && index_main < 6)
  {
    miden = (*(float *)(g_RmsData + index_main  ))/100;
      
    sprintf(strBuf, "%5.3f", miden );

    sprintf(result, "%s%s ", homepage_displayBuf[index_main], strBuf);

    DisplayText(result,LEFT);  

  }
  else if( index_main == 6)
  {
    miden = (*(float *)(g_RmsData + index_main + 3*1 ))/10;
      
    sprintf(strBuf, "%4.0f", miden );

    sprintf(result, "%s%s ", homepage_displayBuf[index_main], strBuf);

    DisplayText(result,LEFT);  

  }
  
  else if( index_main == 7)
  {
    miden = (*(float *)(g_RmsData + index_main + 3*2 ))/10;
      
    sprintf(strBuf, "%4.0f", miden );

    sprintf(result, "%s%s ", homepage_displayBuf[index_main], strBuf);

    DisplayText(result,LEFT);  

  }
  
  else if( index_main == 8)
  {
    miden = (*(float *)(g_RmsData + index_main + 3*3 ))/10;
      
    sprintf(strBuf, "%4.0f", miden );

    sprintf(result, "%s%s ", homepage_displayBuf[index_main], strBuf);

    DisplayText(result,LEFT);  

  }
  else if( index_main == 9)
  {
    miden = (*(float *)(g_RmsData + index_main +3*3 ))/1000;
      
    sprintf(strBuf, "%5.3f", miden );

    sprintf(result, "%s%s ", homepage_displayBuf[index_main], strBuf);

    DisplayText(result,LEFT);  

  } 
  else if( index_main == 10)
  {
    miden = (*(float *)(g_RmsData + index_main +3*3 ))/100;
      
    sprintf(strBuf, "%4.2f", miden );

    sprintf(result, "%s%s ", homepage_displayBuf[index_main], strBuf);

    DisplayText(result,LEFT);  

  }    
} 

 const char AO_Childpage_displayBuf[][20] ={
 {"TYPEUA  "},
 {"TYPEUb  "},
 {"TYPEUC  "},
 {"TYPEIA  "},
 {"TYPEIb  "},
 {"TYPEIC  "},
 {"TYPEPA  "},
 {"TYPEPb  "},
 {"TYPEPC  "},
 {"TYPEPS  "},
 {"TYPEqA  "},
 {"TYPEqb  "},
 {"TYPEqC  "},
 {"TYPEPS  "},
 {"TYPESA  "},
 {"TYPESb  "},
 {"TYPESC  "},
 {"TYPESS  "},
 {"TYPEPF  "},
 {"TYPEF  "},
};
 
static void AO_ChildUI(uint8_t  index )
{ 
  DisplayText((char *)AO_Childpage_displayBuf[index],LEFT); 
}


static void CodeUI(void )
{ 
  DisplayText("Code    ",LEFT);
} 

static void SetUI(void )
{
  DisplayText("SEt    ",LEFT);
} 

static void disUI(void )
{
  DisplayText("dIS    ",LEFT); 
} 

static void ConnUI(void )
{
  DisplayText("Conn   ",LEFT); 
}

static void AO_1UI(void )
{
  DisplayText("AO-1    ",LEFT); 
}

static void AO_2UI(void )
{
  DisplayText("AO-2    ",LEFT); 
}

static void AO_3UI(void )
{
  DisplayText("AO-3    ",LEFT); 
}

static void SAVEUI(void )
{
  DisplayText("SAUE    ",LEFT); 
}


static uint8_t index = 1;
volatile uint8_t g_passflag = 0 ;
/*
 * 函数名：PasswordProc
 * 描述  ：验证密码函数
 * 输入  ：无
 * 输出  ：无
 */
void PasswordProc(void)
{
//  static uint8_t  index ;
//  uint8_t keyVal ;
//  uint8_t passUI_flag = 0;
//  
//  keyVal = GetKeyVal ();
//  
//  if( keyVal == KEY3_SHORT_PRESS)
//  {
//    PassUI (); 
//    passUI_flag = 1;
//  }
//  while( passUI_flag )
//  {  
//    keyVal = GetKeyVal ();
//    switch ( keyVal )
//    {  
//      case KEY2_SHORT_PRESS:
//            index++;
//            V_change ( index );
//            Write7219(3,LED3_vol);
//            Write7219(4,LED4_vol);  
//        break;
//      case KEY3_LONG_PRESS:    //验证密码是否正确
//         if( index == 5)
//         { 
//          index = 0;             
//          g_passflag = 1;
//         
//         }
//         else
//         {
//           /**rr 提示密码错误*/
//           Write7219(3,0x46);
//           Write7219(4,0x46);  
//           DelayNms (1000);
//           
//         } 
//         passUI_flag = 0;
//         
//        break;
//    } 
//  }  
}

volatile uint16_t baudRate;

uint8_t index_AO[3] = {0,1,2};

void Hmi_Task(void )
{ 
  static uint8_t index_main = 0;
  static uint8_t* g_Mark = NULL ;
  static uint8_t keyVal;
  static uint16_t testtime = 10;
  
  g_Mark = GetAnalogTypeData();
  
  if (UI_State == Main)
  {
    MainUI (index_main);

//    index_AO[0] = *g_Mark;
//    index_AO[1] = *(g_Mark+1);
//    index_AO[2] = *(g_Mark+2);
  }
  
  keyVal = GetKeyVal ();
 
  switch ( keyVal )
  { 
    case KEY1_SHORT_PRESS:  
       switch ( UI_State )
       {        
         case AO_1:           
          UI_State = AO_1_Child ;             
          AO_ChildUI(* g_Mark ) ;      
         break;
         
         case AO_2:           
          UI_State = AO_2_Child ;
          AO_ChildUI(* (g_Mark + 1) ) ;        
         break;
         
         case AO_3:           
          UI_State = AO_3_Child ;
          AO_ChildUI(* (g_Mark + 2) ) ;          
         break;
        
         case AO_1_Child:
           UI_State = AO_1 ;
           AO_1UI ();
         break;
         
         case AO_2_Child:
           UI_State = AO_2 ;
           AO_2UI ();
         break;
         
         case AO_3_Child:
           UI_State = AO_3 ;
           AO_3UI ();
         break;
                           
         default :
           break ;       
       }
       
    break;
       
    case KEY1_LONG_PRESS:
    break;
    
    case KEY2_SHORT_PRESS:
       switch ( UI_State )
       { 
         case Main:
           index_main++;
           if( index_main == homepage_num) index_main=0;
         break;
           
         case AO_1_Child : 
           index_AO[0]++;
           if( index_AO[0] == 20) index_AO[0]=0;
             AO_ChildUI(index_AO[0]);
         break;
         
         case AO_2_Child :     
           index_AO[1]++;
           if( index_AO[1] == 20) index_AO[1]=0;
             AO_ChildUI(index_AO[1]);
         break;
         
         case AO_3_Child :     
           index_AO[2]++;
           if( index_AO[2] == 20) index_AO[2]=0;
             AO_ChildUI(index_AO[2]);
         break;
                  
         
         default :           
         break ;
       
       }
      break;
    case KEY2_LONG_PRESS:
         index++;
         if( index > 4)
         {
         index = 1 ;
         }  
    break;

    case KEY3_SHORT_PRESS:      
       switch ( UI_State )
       {
         case Main:
           UI_State = Code ;
           CodeUI ();         
         break;
         case Code:
           UI_State = Set ;
           SetUI ();
         break;
         case Set:
           UI_State = Dis ;
           disUI ();
         break;
         case Dis:
           UI_State = Conn ;
           ConnUI();
         break;
         case Conn:
           UI_State = AO_1  ;
           AO_1UI ();
         break;
         case AO_1:
           UI_State = AO_2 ;
           AO_2UI ();
         break;
         case AO_2:
           UI_State = AO_3 ;
           AO_3UI ();
         break;
         case AO_3:
           UI_State = Save ;
           SAVEUI();
         break;
         case Save:
           UI_State = Main ;
         break;         


         default :
           break ;       
       }
      break;
     
    case KEY3_LONG_PRESS:
      
        if( UI_State == Save)
         {
            SetAnalogTypeParam(index_AO[0], 0);
            SetAnalogTypeParam(index_AO[1], 1);
            SetAnalogTypeParam(index_AO[2], 2);
            
            while( testtime--)
            {
             ToggleLed (3);
             DelayNms (100);  
            
            }
         }    

     break;
          
    default :      
      break ;
  } 
}


///*
// * 函数名：ScheduleCb
// * 描述  ：移位数码管闪烁函数，间隔500ms
// * 输入  ：无
// * 输出  ：无
// */
//static void ScheduleCb(void)
//{
////   if( Getgtimer () == 5 )
////   {  
////     if( UI_State == VoltageChild  || UI_State == TimerChild )
////     {
////       Write7219(index+4,0x00);            
////     }
////   } 
////   if(Getgtimer () == 10)   
////   {
////     if( UI_State == VoltageChild )
////     {
////       VoltageUI_Child (index_Vol); 
////     }
////     if( UI_State == TimerChild )
////     {
////       TimerUI_Child (index_Timer);   
////     }
////     
////   }
//}

/*
 * 函数名：Hmipass_Task
 * 描述  ：密码验证通过后，运行Hmi_Task函数
           如果没有验证通过的话，只运行主界面函数和待按键1按下运行密码验证函数
 * 输入  ：无
 * 输出  ：无
 */
void Hmipass_Task(void )
{  
//  if( g_passflag == 0)
//  {
//    PasswordProc ();
//    MainUI ();  
//  }  
//  else 
//  {
    Hmi_Task ();
//    ScheduleCb ();
//  }
}





