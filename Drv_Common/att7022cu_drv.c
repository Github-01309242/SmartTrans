
#include "stm32f10x.h" 
#include <stdint.h>
#include "delay.h"

//CS(NSS)���� Ƭѡѡ��ͨGPIO���� 
#define      att7022_SPI_CS_PORT                 GPIOA
#define      att7022_SPI_CS_PIN                  GPIO_Pin_4

//SCK���� 
#define      att7022_SPI_SCK_PORT                GPIOA   
#define      att7022_SPI_SCK_PIN                 GPIO_Pin_5
//MISO����  
#define      att7022_SPI_MISO_PORT               GPIOA 
#define      att7022_SPI_MISO_PIN                GPIO_Pin_6
//MOSI����
#define      att7022_SPI_MOSI_PORT               GPIOA 
#define      att7022_SPI_MOSI_PIN                GPIO_Pin_7
//RESET����
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
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_MISO_PIN  ;  //SPI2��MISO��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(att7022_SPI_MISO_PORT ,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =att7022_SPI_MOSI_PIN;	//MOSI�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(att7022_SPI_MOSI_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_SCK_PIN ;	//SCK�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(att7022_SPI_SCK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_CS_PIN;	 //SPI2��NSS��PB12��Ϊ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(att7022_SPI_CS_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = att7022_SPI_RESET_PIN;	 //resetΪ�������
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

//�������Ĵ�������
uint32_t  ReadSampleRegister(uint8_t  cmd)
{
 writeatt7022(0xC6, 0x0000aa);  //�����������ݼĴ�������
 DelayNus(50); 
 return readatt7022(cmd);
}


//дУ��Ĵ���
void WriteAdjustRegister(uint8_t  addr,u32 data)
{
  writeatt7022(0xC9,0x000000);  //ʹ��д
  DelayNus(50);
  writeatt7022(addr,data);      //д��
  DelayNus(50);
  writeatt7022(0xC9,0x000001);  //�ر�д
  DelayNus(200);
}
//��У��Ĵ���
uint32_t  ReadAdjustRegister(uint8_t  addr)
{
  uint32_t tmpbuf;
  writeatt7022(0xC6,0x00005a);  //ʹ�ܽϱ�Ĵ���������
  DelayNms(10);
  tmpbuf = readatt7022(addr) ; 
  DelayNms(100);
  return tmpbuf;
}

 /**
���㹫ʽ�� 
Ugain=Ur/Urms-1 
��� Ugain>=0���� Ugain=INT[Ugain*2^23] 
���� Ugain<0���� Ugain=INT[2^24+Ugain*2^23]

ʵ�������ѹ��Чֵ Ur=220 
������ѹ��Чֵ Urms=DateU*2^10/2^23 
                   =0x29999A/2^13=332.800048828125 
�� Ugain=Ur/Urms-1=-0.338942404682 
���� Ugain=2^24-0.338942404682*2^23=13933961=0xD49D89
**/
//===============================================================
// ��ѹУ������
// PhaseType : ���ѹ����
// StandValue: ��׼��ѹֵ
// AdjustBuff: ����У������
//===============================================================
uint32_t  AdjustVoltageFrun(float StandValue,uint8_t  PhaseType)
{
//    uint32_t VoltageAdjustRegister[3]={0x00,0x00,0x00};
    float RealVol=0 ;  //��ʵ��ѹֵ
    float Ugain=0;
    uint32_t  Uadjust;
        
    ClearAdjustRegister();
    WriteAdjustRegister(0x3f,0x465500);
    DelayNms(1000);

    RealVol = ReadSampleRegister (0X0D+PhaseType ) /8192.0;    //��ӦA ���ѹ��Чֵ�Ĵ���
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



