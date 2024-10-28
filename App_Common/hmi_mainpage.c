
#include <stdio.h>
#include <string.h>

#include "ntc_drv.h"
#include "lcd12864_drv.h"
#include "sensor_drv.h"
#include "mov_invalid.h"
#include "hmi_common.h"
#include "picture.h"

extern PictureDesc_t g_tempIconPicDesc;
extern PictureDesc_t g_centigradeIconPicDesc;
extern PictureDesc_t g_surgeIconPicDesc;
extern PictureDesc_t g_movPercentPicsDesc[];


#define TEMP_ICON_POS_X         1
#define TEMP_ICON_POS_Y         3
#define TEMP_VAL_POS_X          TEMP_ICON_POS_X + 5
#define TEMP_VAL_POS_Y          3
#define CENTIGRADE_ICON_POS_X   TEMP_VAL_POS_X + 4
#define CENTIGRADE_ICON_POS_Y   3

#define TempAdjust  4.0f
static void DisplayTempInfo(void)
{
  char strBuf[10];
 	float tempData;
//  LcdDrawPicture (TEMP_ICON_POS_X , TEMP_ICON_POS_Y ,&g_tempIconPicDesc);
  LcdDrawString(TEMP_ICON_POS_X,TEMP_ICON_POS_Y,(unsigned char *)"Temp:");  
  LcdDrawPicture (CENTIGRADE_ICON_POS_X , CENTIGRADE_ICON_POS_Y ,&g_centigradeIconPicDesc);
	if (GetTempData(&tempData))
	{
    memset(strBuf, 0, sizeof(strBuf));
    if(  tempData*10 < 0x0640)
    {
      tempData = tempData - TempAdjust ;
      LcdDrawString(TEMP_VAL_POS_X,TEMP_VAL_POS_Y,(unsigned char *)"+") ; 
    }
    else if ( tempData*10 > 0xFF9C  &&  tempData*10 < 0xFFFF)
    {
      tempData =  0xffff - (uint16_t )(tempData*10 - 1);
      tempData = tempData / 10.0f + TempAdjust;
      LcdDrawString(TEMP_VAL_POS_X,TEMP_VAL_POS_Y,(unsigned char *)"-") ;    
    } 
    else 
     return ;
    
    sprintf(strBuf, "%03d", (uint16_t)tempData); 
    LcdDrawString(TEMP_VAL_POS_X+1,TEMP_VAL_POS_Y,(unsigned char *)strBuf) ;
	}
	else
	{
		LcdDrawString(TEMP_VAL_POS_X,TEMP_VAL_POS_Y,(unsigned char *)"--") ;     			
	}	
}

#define SURGE_ICON_POS_X         1
#define SURGE_ICON_POS_Y         5
#define SURGE_NUM_POS_X          SURGE_ICON_POS_X + 6
#define SURGE_NUM_POS_Y          5

static void DisplaySurgeNumInfo(void)
{
  char strBuf[10];
	SensorData_t sensorData;
  
  LcdDrawString(SURGE_ICON_POS_X,SURGE_ICON_POS_Y,(unsigned char *)"Timer:"); 
//  LcdDrawChinese (SURGE_NUM_POS_X+3, SURGE_NUM_POS_Y, (unsigned  char *)"��");
	if (GetSensorData(&sensorData))
	{
		memset(strBuf, 0, sizeof(strBuf));
		sprintf(strBuf, "%03d", sensorData.SurgemdlParam.Num );
		LcdDrawString(SURGE_NUM_POS_X,SURGE_NUM_POS_Y,(unsigned char *)strBuf) ;     
	}
	else
	{
		LcdDrawString(SURGE_NUM_POS_X,SURGE_NUM_POS_Y,(unsigned char *)"--") ;     			
	}	
}

#define Invalid_ICON_POS_X         13
#define Invalid_ICON_POS_Y         1

static void DisplayInvalidInfo(void)
{
  MovLevel_t MovLevel;
  MovLevel = GetMovLevel();
  LcdDrawPicture (Invalid_ICON_POS_X , Invalid_ICON_POS_Y ,&g_movPercentPicsDesc[MovLevel]);
  
}

#define TIME_POS_X   1
#define TIME_POS_y   1

static void DisplayTime(void)
{
  RtcTime_t rtcTime;
  char strBuf[20];
  
  GetRtcTime (&rtcTime);
  
  static uint8_t LastSecond = 0;
  if( rtcTime.second ==  LastSecond)
  {
   return ;
  }  
  else 
  {  
    LastSecond= rtcTime.second;
    
    sprintf(strBuf, "%02d:%02d:%02d", rtcTime.hour , rtcTime.minute , rtcTime.second );
    LcdDrawString(TIME_POS_X,TIME_POS_y,(unsigned char *)strBuf);    
  }
  
}

void DisplayMainpage(void)
{
	DisplayMenuKey();
  DisplayTime();
	DisplayTempInfo();
	DisplaySurgeNumInfo();
  DisplayInvalidInfo();
}
