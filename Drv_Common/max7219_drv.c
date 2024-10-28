
/*********************************************************************************************************
* LED Segments:         a
*                     ----
*                   f|    |b
*                    |  g |
*                     ----
*                   e|    |c
*                    |    |
*                     ----  o dp
*                       d
*   Register bits:
*      bit:  7  6  5  4  3  2  1  0
*           dp  a  b  c  d  e  f  g
*********************************************************************************************************
* Example : The letter 'I' is represented by illuminating LED's 'b' and 'c' (refer above diagram)
*           Therfore the binary representation of 'I' is as follows
*
*           abcdefg
*           0110000
* 
*           The table below contains all the binary values for the desired font. New font characters
*           can be added or altered as required. 
*           
*           The DP bit is used to switch on the decimal place LED. DP is not included in the below table
*           but is added in the register within the libary depending on the content being displayed.  
*********************************************************************************************************/

#include "stm32f10x.h"
#include "stdbool.h"
#include "string.h"
#include "max7219_drv.h"


#define rcu      RCC_APB2Periph_GPIOB

#define gpio     GPIOB

#define cs_pin   GPIO_Pin_12
#define sck_pin  GPIO_Pin_13
#define din_pin  GPIO_Pin_14

#define SET_7219_CS()              GPIO_SetBits(GPIOB, GPIO_Pin_12)         
#define CLR_7219_CS()              GPIO_ResetBits(GPIOB, GPIO_Pin_12)        
#define SET_7219_CLK()             GPIO_SetBits(GPIOB, GPIO_Pin_13)         
#define CLR_7219_CLK()             GPIO_ResetBits(GPIOB, GPIO_Pin_13)     
#define SET_7219_DIN()             GPIO_SetBits(GPIOB, GPIO_Pin_14)       
#define CLR_7219_DIN()             GPIO_ResetBits(GPIOB, GPIO_Pin_14)       


static void Max7219_GPIO_Config(void )
{
	GPIO_InitTypeDef GPIO_InitStructure; 	
	/*开启外设时钟*/
  RCC_APB2PeriphClockCmd ( rcu, ENABLE );	
	/*选择引脚*/															   
  GPIO_InitStructure.GPIO_Pin = cs_pin | sck_pin | din_pin;	
	/*设置引脚模式为通用推挽输出*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率 */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	/*调用库函数，初始化DHT11_Dout_GPIO_PORT*/
  GPIO_Init ( gpio, &GPIO_InitStructure );	
}  

  
void Write7219(uint8_t Addr, uint8_t Data)
{
  CLR_7219_CS ();
  for(uint8_t i=0; i < 8; i++)
  {
   CLR_7219_CLK ();
   if( ( Addr & 0x80 )== 0x80)
   {
    SET_7219_DIN ();
   }
   else
   {
    CLR_7219_DIN ();
   }
   Addr <<= 1;
   SET_7219_CLK ();
  }
  
  for(uint8_t i=0; i < 8; i++)
  {
    CLR_7219_CLK ();
    if( ( Data & 0x80 )== 0x80)
    {
    SET_7219_DIN ();
    }
    else
    {
    CLR_7219_DIN ();
    }
    Data <<= 1;
    SET_7219_CLK ();  
  }
  
  SET_7219_CS ();
 
}  

/**
***********************************************************
* @brief MAX7219抗干扰保护程序
* @param
* @return
***********************************************************
*/
void Anti_interference_protection(void )
{
  Write7219(0x0C,0x01);         //关闭寄存器      00-关闭模式   01-正常模式
  Write7219(0x0F,0x00);         //显示检测寄存器  00-正常显示   01-测试显示 
  Write7219(0x09,0x00);         //译码模式寄存器  00-非BCD译码  FF-BCD译码
  Write7219(0x0B,0x07);         //扫描界限寄存器  07-显示位数0、1、2、3、4、5、6、7
  Write7219(0x0A,0x03);         //亮度寄存器      03-7/32占空比
}  

void Max7219Init(void )
{
  Max7219_GPIO_Config();

  SET_7219_CS ();
  SET_7219_CLK ();
  SET_7219_DIN ();

  Anti_interference_protection();
  
  Write7219 (1,table[0]);

  Write7219 (2,table[1]);

  Write7219 (3,table[2]);    

  Write7219 (4,table[3]);
  
  Write7219 (5,table[4]);

  Write7219 (6,table[5]);

  Write7219 (7,table[6]);    

  Write7219 (8,table[7]);

}

typedef struct {
	char     ascii;
	uint8_t     segs;
}MAX7219_Font_t;

static const MAX7219_Font_t MAX7219_Font[]= 
{
  {'A',0X77},{'B',0X7F},{'C',0X4E},{'D',0X7E},{'E',0X4F},{'F',0X47},       
  {'G',0X5E},{'H',0X37},{'I',0X30},{'J',0X3C},{'L',0X0E},{'N',0X76},       
  {'O',0X7E},{'P',0X67},{'R',0X05},{'S',0X5B},{'T',0X0F},{'U',0X3E},       
  {'Y',0X27},{'[',0X4E},{']',0X78},{'_',0X08},{'a',0X77},{'b',0X1F},       
  {'c',0X0D},{'d',0X3D},{'e',0X4F},{'f',0X47},{'g',0X5E},{'h',0X17},       
  {'i',0X10},{'j',0X3C},{'l',0X0E},{'n',0X15},{'o',0X7E},{'p',0X67},{'q',0X73},  
  {'r',0X05},{'s',0X5B},{'t',0X0F},{'u',0X1C},{'y',0X27},{'-',0X01},
  {' ',0X00},{'0',0X7E},{'1',0X30},{'2',0X6D},{'3',0X79},{'4',0X33},
  {'5',0X5B},{'6',0X5F},{'7',0X70},{'8',0X7F},{'9',0X7B},{'\0',0X00},
};

#define FontNum (sizeof(MAX7219_Font)/sizeof(MAX7219_Font[0]))

unsigned char MAX7219_LookupCode (char character, unsigned int dp)
{
  int i;
  unsigned int d=0;
  if (dp) d=1;  
  if (character>=35 && character<=44) 
  {
    character+=13;
    d=1;
  }
  for (i = 0; i < FontNum; i++) 
  {
    if (character == MAX7219_Font[i].ascii)
    {
      if (d)
      {
        d=MAX7219_Font[i].segs;
        d |= (1<<7);
        return (d);                                    
      }
      else
      {
       return MAX7219_Font[i].segs;                    
      }
    }
  }    
  return 0;                                             
}

void DisplayChar(int digit, char value, bool dp) 
{
  Write7219(digit+1,MAX7219_LookupCode(value, dp));
}


void DisplayText(char *text, int justify)
{
  int decimal[16];
  char trimStr[16] = "";
  int x,y=0;
  int s;
  
  s=strlen(text);
  
  if (s>16) 
  {  
   s=16;
  }
  
  for (x=0;x < s;x++)
  {
    if (text[x]=='.')
    {
      decimal[y-1]=1;
    }
    else
    {
      trimStr[y]=text[x];
      decimal[y]=0;
      y++;
    }
  }
  if( y > 8 ) 
  {  
   y = 8;
  }
  
  for (x=0;x < y;x++)
  {
    if (justify==0)
    {      
      DisplayChar((int)(y+x-y),trimStr[x], decimal[x]);    
    }
    else if (justify==1)
    {
      DisplayChar((int)(y-x-1),trimStr[x], decimal[x]);            
    }
  }
}


void DisplayTest(void )
{
// DisplayChar(0, 'A', 1) ;
  DisplayText("Set     ",LEFT);
}









