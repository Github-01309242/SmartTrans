
#include "stm32f10x.h" 
#include <stdint.h>
#include "delay.h"

//CS(NSS)引脚 片选选普通GPIO即可 
#define      att7022_SPI_CS_PORT                 GPIOA
#define      att7022_SPI_CS_PIN                  GPIO_Pin_4

//SCK引脚 
#define      att7022_SPI_SCK_PORT                GPIOA   
#define      att7022_SPI_SCK_PIN                 GPIO_Pin_5
//MISO引脚  
#define      att7022_SPI_MISO_PORT               GPIOA 
#define      att7022_SPI_MISO_PIN                GPIO_Pin_6
//MOSI引脚
#define      att7022_SPI_MOSI_PORT               GPIOA 
#define      att7022_SPI_MOSI_PIN                GPIO_Pin_7
//RESET引脚
#define      att7022_SPI_RESET_PORT              GPIOD 
#define      att7022_SPI_RESET_PIN               GPIO_Pin_0

#define  		 SPI_att7022_RESET_LOW()     				 GPIO_ResetBits( att7022_SPI_RESET_PORT, att7022_SPI_RESET_PIN )
#define  		 SPI_att7022_RESET_HIGH()    				 GPIO_SetBits( att7022_SPI_RESET_PORT, att7022_SPI_RESET_PIN )

#define  		 SPI_att7022_CS_LOW()     					 GPIO_ResetBits( att7022_SPI_CS_PORT, att7022_SPI_CS_PIN )
#define  		 SPI_att7022_CS_HIGH()    					 GPIO_SetBits( att7022_SPI_CS_PORT, att7022_SPI_CS_PIN )

#define  		 SPI_att7022_SCK_LOW()     					 GPIO_ResetBits( att7022_SPI_SCK_PORT, att7022_SPI_SCK_PIN )
#define  		 SPI_att7022_SCK_HIGH()    					 GPIO_SetBits( att7022_SPI_SCK_PORT, att7022_SPI_SCK_PIN )

#define  		 SPI_att7022_MOSI_LOW()     				 GPIO_ResetBits( att7022_SPI_MOSI_PORT, att7022_SPI_MOSI_PIN )
#define  		 SPI_att7022_MOSI_HIGH()    				 GPIO_SetBits( att7022_SPI_MOSI_PORT, att7022_SPI_MOSI_PIN )


static void gpio_drv_init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_MISO_PIN  ;  //SPI2的MISO浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(att7022_SPI_MISO_PORT ,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =att7022_SPI_MOSI_PIN;	//MOSI推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(att7022_SPI_MOSI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_SCK_PIN ;	//SCK推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(att7022_SPI_SCK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_CS_PIN;	 //SPI2的NSS（PB12）为推免输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(att7022_SPI_CS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_RESET_PIN;	 //reset为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(att7022_SPI_RESET_PORT,&GPIO_InitStructure);
}

static void att7022cu_init(void)
{
		SPI_att7022_CS_HIGH();
	  SPI_att7022_SCK_LOW();
	  SPI_att7022_MOSI_LOW();
		SPI_att7022_RESET_HIGH();     
}

void att7022cu_drv_init(void )
{
  gpio_drv_init();
  att7022cu_init(); 
}

void ResetATT7022(void)
{
      SPI_att7022_RESET_LOW();
			DelayNms(20);
			SPI_att7022_RESET_HIGH();
			DelayNms(50);
}


static void writeatt7022(u8 com,u32 data)
{
	u8 i;       
  SPI_att7022_CS_HIGH();
  SPI_att7022_SCK_LOW();
	SPI_att7022_CS_LOW();
	com |= 0x80;   

	for(i=0;i<8;i++)
	{
		SPI_att7022_SCK_HIGH();
		DelayNus(5);
		if( com & 0x80 )          
		  SPI_att7022_MOSI_HIGH();
		else
			SPI_att7022_MOSI_LOW();
    
    SPI_att7022_SCK_LOW();	
    DelayNus(3);
    com<<=1;               
	}
	DelayNus(5);
	for(i=0;i<24;i++)
	{
    SPI_att7022_SCK_HIGH();
    DelayNus(5);
    if(data&0x00800000)
    {
     SPI_att7022_MOSI_HIGH();
    }
    else
    {
     SPI_att7022_MOSI_LOW();
    }
    SPI_att7022_SCK_LOW();
    DelayNus(3);
    data<<=1;
   }               
	SPI_att7022_CS_HIGH();	
    
} 


static u32 readatt7022(u8 com)
{       
  u8 i,bit;  
  u32 data=0;
  SPI_att7022_CS_HIGH();
  DelayNus(1);
  SPI_att7022_SCK_LOW();
  DelayNus(1);
  SPI_att7022_CS_LOW();

  DelayNus(1);
	for(i=0;i<8;i++)  
	{
   SPI_att7022_SCK_HIGH();
   DelayNus(1);
    if(com & 0x80)
    { 
      SPI_att7022_MOSI_HIGH();
    }		
		else
		{	
		  SPI_att7022_MOSI_LOW();
    }         
   com<<=1;	              
	 SPI_att7022_SCK_LOW();
	 DelayNus(1);
	}	
	
  DelayNus(5);
	for(i=0;i<24;i++)    
	{        
    SPI_att7022_SCK_HIGH();        
    bit= GPIO_ReadInputDataBit(GPIOA,att7022_SPI_MISO_PIN );   
    DelayNus(1);		
    SPI_att7022_SCK_LOW();
    data=(data<<1)|bit;
    DelayNus(2);
   }          
	SPI_att7022_CS_HIGH();
  return data;         
}

static void ClearAdjustRegister(void)
{    
  writeatt7022(0xC3,0x00000000); 
  DelayNms(200);
}

//读计量寄存器参数
uint32_t  ReadSampleRegister(uint8_t  cmd)
{
 writeatt7022(0xC6, 0x0000aa);  //读出计量数据寄存器参数
 DelayNus(50); 
 return readatt7022(cmd);
}


//写校表寄存器
void WriteAdjustRegister(uint8_t  addr,u32 data)
{
  writeatt7022(0xC9,0x000000);  //使能写
  DelayNus(50);
  writeatt7022(addr,data);      //写入
  DelayNus(50);
  writeatt7022(0xC9,0x000001);  //关闭写
  DelayNus(200);
}
//读校表寄存器
uint32_t  ReadAdjustRegister(uint8_t  addr)
{
  uint32_t tmpbuf;
  writeatt7022(0xC6,0x00005a);  //使能较表寄存器读操作
  DelayNms(10);
  tmpbuf = readatt7022(addr) ; 
  DelayNms(100);
  return tmpbuf;
}

 /**
计算公式： 
Ugain=Ur/Urms-1 
如果 Ugain>=0，则 Ugain=INT[Ugain*2^23] 
否则 Ugain<0，则 Ugain=INT[2^24+Ugain*2^23]

实际输入电压有效值 Ur=220 
测量电压有效值 Urms=DateU*2^10/2^23 
                   =0x29999A/2^13=332.800048828125 
则 Ugain=Ur/Urms-1=-0.338942404682 
所以 Ugain=2^24-0.338942404682*2^23=13933961=0xD49D89
**/
//===============================================================
// 电压校正函数
// PhaseType : 相电压类型
// StandValue: 标准电压值
// AdjustBuff: 保存校表数据
//===============================================================
uint32_t  AdjustVoltageFrun(float StandValue,uint8_t  PhaseType)
{
//    uint32_t VoltageAdjustRegister[3]={0x00,0x00,0x00};
    float RealVol=0 ;  //真实电压值
    float Ugain=0;
    uint32_t  Uadjust;
        
    ClearAdjustRegister();
    WriteAdjustRegister(0x3f,0x465500);
    DelayNms(1000);

    RealVol = ReadSampleRegister (0X0D+PhaseType ) /8192.0;    //对应A 相电压有效值寄存器
    Ugain = StandValue / RealVol - 1 ;
    if( Ugain >= 0)
    {
    Uadjust = ( uint32_t )( Ugain * 8388608 );
    }
    else if ( Ugain < 0)
    {    
    Uadjust = (uint32_t )( 16777216 + Ugain * 8388608 );
    }
    
    return Uadjust;
    
}



