
#include "stm32f10x.h" 
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "delay.h"
#include "eeprom_drv.h"
#include "ht7036_drv.h"
#include "systeminf.h"
#include "store_app.h"


//CS(NSS)���� Ƭѡѡ��ͨGPIO���� 
#define      ht7036_SPI_CS_PORT                 GPIOA
#define      ht7036_SPI_CS_PIN                  GPIO_Pin_4

//SCK���� 
#define      ht7036_SPI_SCK_PORT                GPIOA   
#define      ht7036_SPI_SCK_PIN                 GPIO_Pin_5
//MISO����  
#define      ht7036_SPI_MISO_PORT               GPIOA 
#define      ht7036_SPI_MISO_PIN                GPIO_Pin_6
//MOSI����
#define      ht7036_SPI_MOSI_PORT               GPIOA 
#define      ht7036_SPI_MOSI_PIN                GPIO_Pin_7
//RESET����
#define      ht7036_SPI_RESET_PORT              GPIOA
#define      ht7036_SPI_RESET_PIN               GPIO_Pin_12

#define  		 SPI_ht7036_RESET_LOW()     				GPIO_ResetBits( ht7036_SPI_RESET_PORT, ht7036_SPI_RESET_PIN )
#define  		 SPI_ht7036_RESET_HIGH()    				GPIO_SetBits( ht7036_SPI_RESET_PORT, ht7036_SPI_RESET_PIN )

#define  		 SPI_ht7036_CS_LOW()     					  GPIO_ResetBits( ht7036_SPI_CS_PORT, ht7036_SPI_CS_PIN )
#define  		 SPI_ht7036_CS_HIGH()    					  GPIO_SetBits( ht7036_SPI_CS_PORT, ht7036_SPI_CS_PIN )

#define  		 SPI_ht7036_SCK_LOW()     					GPIO_ResetBits( ht7036_SPI_SCK_PORT, ht7036_SPI_SCK_PIN )
#define  		 SPI_ht7036_SCK_HIGH()    					GPIO_SetBits( ht7036_SPI_SCK_PORT, ht7036_SPI_SCK_PIN )

#define  		 SPI_ht7036_MOSI_LOW()     				  GPIO_ResetBits( ht7036_SPI_MOSI_PORT, ht7036_SPI_MOSI_PIN )
#define  		 SPI_ht7036_MOSI_HIGH()    				  GPIO_SetBits( ht7036_SPI_MOSI_PORT, ht7036_SPI_MOSI_PIN )


static void gpio_drv_init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//SPI2��MISO��������
	GPIO_InitStructure.GPIO_Pin = ht7036_SPI_MISO_PIN  ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ht7036_SPI_MISO_PORT ,&GPIO_InitStructure);
	//MOSI�������
	GPIO_InitStructure.GPIO_Pin =ht7036_SPI_MOSI_PIN;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(ht7036_SPI_MOSI_PORT,&GPIO_InitStructure);
	//SCK�������
	GPIO_InitStructure.GPIO_Pin = ht7036_SPI_SCK_PIN ;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(ht7036_SPI_SCK_PORT,&GPIO_InitStructure);
	//SPI2��NSSΪ�������
	GPIO_InitStructure.GPIO_Pin = ht7036_SPI_CS_PIN;	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(ht7036_SPI_CS_PORT,&GPIO_InitStructure);
	//resetΪ�������
	GPIO_InitStructure.GPIO_Pin = ht7036_SPI_RESET_PIN;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(ht7036_SPI_RESET_PORT,&GPIO_InitStructure);
}

static void ht7036cu_init(void)
{
		SPI_ht7036_CS_HIGH();
	  SPI_ht7036_SCK_LOW();
	  SPI_ht7036_MOSI_LOW();
		SPI_ht7036_RESET_HIGH();     
}


static void writeht7036(u8 com,u32 data)
{
	u8 i;       
  SPI_ht7036_CS_HIGH();
  SPI_ht7036_SCK_LOW();
	SPI_ht7036_CS_LOW();
	com |= 0x80;   

	for(i=0;i<8;i++)
	{
		SPI_ht7036_SCK_HIGH();
		DelayNus(5);
		if( com & 0x80 )          
		  SPI_ht7036_MOSI_HIGH();
		else
			SPI_ht7036_MOSI_LOW();
    
    SPI_ht7036_SCK_LOW();	
    DelayNus(3);
    com<<=1;               
	}
	DelayNus(5);
	for(i=0;i<24;i++)
	{
    SPI_ht7036_SCK_HIGH();
    DelayNus(5);
    if(data&0x00800000)
    {
     SPI_ht7036_MOSI_HIGH();
    }
    else
    {
     SPI_ht7036_MOSI_LOW();
    }
    SPI_ht7036_SCK_LOW();
    DelayNus(3);
    data<<=1;
   }               
	SPI_ht7036_CS_HIGH();	
    
} 


static u32 readht7036(u8 com)
{       
  u8 i,bit;  
  u32 data=0;
  SPI_ht7036_CS_HIGH();
  DelayNus(1);
  SPI_ht7036_SCK_LOW();
  DelayNus(1);
  SPI_ht7036_CS_LOW();

  DelayNus(1);
	for(i=0;i<8;i++)  
	{
   SPI_ht7036_SCK_HIGH();
   DelayNus(1);
    if(com & 0x80)
    { 
      SPI_ht7036_MOSI_HIGH();
    }		
		else
		{	
		  SPI_ht7036_MOSI_LOW();
    }         
   com<<=1;	              
	 SPI_ht7036_SCK_LOW();
	 DelayNus(1);
	}	
	
  DelayNus(5);
	for(i=0;i<24;i++)    
	{        
    SPI_ht7036_SCK_HIGH();        
    bit= GPIO_ReadInputDataBit(ht7036_SPI_MISO_PORT,ht7036_SPI_MISO_PIN );   
    DelayNus(1);		
    SPI_ht7036_SCK_LOW();
    data=(data<<1)|bit;
    DelayNus(2);
   }          
	SPI_ht7036_CS_HIGH();
  return data;         
}

void Resetht7036(void)
{
  writeht7036(0xD3,0x000000); 
  DelayNms (200);
}

static void ClearAdjustRegister(void)
{    
  writeht7036(0xC3,0x000000); 
  DelayNms(200);
}

//�������Ĵ�������
uint32_t  ReadSampleRegister(uint8_t  cmd)
{
 writeht7036(0xC6, 0x00000000);  //�����������ݼĴ�������
 DelayNus(50); 
 return readht7036(cmd);
}


//дУ��Ĵ���
void WriteAdjustRegister(uint8_t  addr,u32 data)
{
  writeht7036(0xC9,0x00005A);  //ʹ��д
  DelayNus(50);
  writeht7036(addr,data);      //д��
  DelayNus(50);
  writeht7036(0xC9,0x000000);  //�ر�д
  DelayNus(200);
}
//��У��Ĵ���
uint32_t  ReadAdjustRegister(uint8_t  addr)
{
  uint32_t tmpbuf;
  writeht7036(0xC6,0x00005A);  //ʹ�ܽϱ�Ĵ���������
  DelayNms(10);
  tmpbuf = readht7036(addr) ; 
  DelayNms(100);
  return tmpbuf;
}

#define HFCONST      109                  // ��Ƶ���峣�� HFConst��INT[2.592*10^10*G*G*Vu*Vi/(EC*Un*Ib)] 

static void EMU_init(void )
{
  uint32_t  Device_ID = 0;                
  uint32_t  Sum;                          //u32 Meter_Checksum = 0x0001DE92;	//���ģ��Ĵ���У���
  
  Resetht7036();
  ClearAdjustRegister();
  WriteAdjustRegister(0x01,0xB97E);       //����ģʽ�Ĵ���
  WriteAdjustRegister(0x03,0xF884);       //EMUcfg   ��������		 0xF884 �͹����й� Smode=1
  WriteAdjustRegister(0x02,0x0100);       //ABC�����ͨ��ADC����Ŵ�1��  �����ѹͨ��ADC����Ŵ�2�� 
  WriteAdjustRegister(0x31,0x3437);       //ģ��ģ��Ĵ���
  WriteAdjustRegister(0x6D,0xFF00);       //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
  WriteAdjustRegister(0x6E,0x0DB8);       //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
  WriteAdjustRegister(0x6F,0xD1DA);       //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
  WriteAdjustRegister(0x1E,HFCONST);      // HFconst=3200  	
  WriteAdjustRegister(0x35,0x080F); 	    //   I/O
  WriteAdjustRegister(0xC5,0x0002);			  //��ͬ��
  
  Device_ID = ReadSampleRegister(0x00);	  // Device ID = D0x7122A0
  printf("оƬID  %#x\r\n",Device_ID);  
  
  Sum = ReadSampleRegister(0x5E);         //У��Ĵ���Ч��� 
  printf("У���  %#x\r\n",Sum); 
} 

void ht7036cu_drv_init(void )
{
  gpio_drv_init();
  ht7036cu_init(); 
  EMU_init();
}

//===============ԭ��ת���ɲ���==============================
void DToBm(long value,unsigned char *buff,unsigned char num)
{
    if(value>0)
  	{
  		if(num==3)
  			value=value&0x007fffff;
  		if(num==2)
  			value=value&0x00007fff;
  		if(num==1)
  			value=value&0x0000007f;
  	}
  	else if(value<=0)
  	{
   		if(num==3)
   			value=value|0x800000;
   		if(num==2)
   			value=value|0x8000;
   		if(num==1)
   			value=value|0x80;
  	}
  
  	*buff    =(unsigned char)(value>>16);
  	*(buff+1)=(unsigned char)(value>>8);
  	*(buff+2)=(unsigned char)value;
}
//==================����ת����ԭ��========================
long BmToYm(long value)
{
  if((value|0xff7fffff)==0xff7fffff);
  else if((value&0x00800000)==0x00800000)
      value=(~value+1)&0x007fffff;
    
	return value;   
}


 /**
  ���㹫ʽ�� 
  Ugain=Ur/Urms-1 
  ��� Ugain>=0���� Ugain=INT[Ugain*2^15] 
  ���� Ugain<0���� Ugain=INT[2^16+Ugain*2^15]

  ʵ�������ѹ��Чֵ Ur=220 
  ������ѹ��Чֵ Urms=DateU/2^13 
                     =0x29999A/2^13=332.800048828125 
  �� Ugain=Ur/Urms-1=-0.338942404682 
  ���� Ugain=2^16-0.338942404682*2^15
**/
//===============================================================
// ��ȡ��Ҫд���ѹ����Ĵ���������
// PhaseType : ���ѹ����
// StandValue: ��׼��ѹֵ
// AdjustBuff: ����У������
//===============================================================
uint32_t  Uadjust[3];
uint32_t*  Get_UgainData(float StandValue)
{
  float RealVol=0 ;  //��ʵ��ѹֵ
  float Ugain=0;
          
  for(uint8_t i = 0; i < 3; i++)
  {
    WriteAdjustRegister (0X17 + i ,0x0000);
    DelayNms (1000);
    
    RealVol = ReadSampleRegister (0X0D + i ) / 8192;    //��ӦA ���ѹ��Чֵ�Ĵ���
    Ugain = StandValue / RealVol - 1 ;
    if( Ugain >= 0)
    {
    Uadjust[i] = ( uint32_t )( Ugain * 32768 );
    }
    else if ( Ugain < 0)
    {    
    Uadjust[i] = (uint32_t )( 65536+ Ugain * 32768 );
    }
  } 
    return Uadjust;  
}

//===============================================================
// ��ȡ��Ҫд���������Ĵ���������
// PhaseType : 
// StandValue: ��׼����ֵ
// AdjustBuff: ����У������
//===============================================================

uint32_t  Iadjust[3];
uint32_t* Get_IgainData(float StandValue)
{
  float RealValue=0.0f ;  //��ʵ��ѹֵ
  float Igain=0;
          
  for(uint8_t i = 0; i < 3; i++)
  {
    WriteAdjustRegister (0X1A + i ,0x0000);
    DelayNms (1000);
    
    RealValue = ReadSampleRegister (0X10 + i ) / 8192 / UI_K;    
    Igain = StandValue / RealValue - 1 ;
    if( Igain >= 0)
    {
    Iadjust[i] = ( uint32_t )( Igain * 32768 );
    }
    else if ( Igain < 0)
    {    
    Iadjust[i] = (uint32_t )( 65536+ Igain * 32768 );
    }
  } 
    return Iadjust;  
}

//===============================================================
// ��ȡ��Ҫд���й���������Ĵ���������
// PhaseType : 
// StandValue: ��׼����ֵ
// AdjustBuff: ����У������
//===============================================================
   
uint32_t  Padjust[3];

uint32_t* Get_PgainData(float StandValue )
{
  float RealValue=0.0f ;  //��ʵ��ѹֵ
  float Pgain=0;
  float Err=0;      
  for(uint8_t i = 0; i < 3; i++)
  {
    WriteAdjustRegister (0X04 + i ,0x0000);
    DelayNms (1500);
    
    RealValue = (float )(BmToYm(ReadSampleRegister (0X01 + i))); 
    RealValue = (RealValue * P_K) ;
    
    Err = (RealValue - StandValue) / StandValue;
    Pgain = -Err / (1 + Err) ;
    if( Pgain >= 0)
    {
    Padjust[i] = ( uint32_t )( Pgain * 32768 );
    }
    else if ( Pgain < 0)
    {    
    Padjust[i] = (uint32_t )( 65536+ Pgain * 32768 );
    }
  } 
  return Padjust;  
}

//===============================================================
// ��ȡ��Ҫд����λУ���Ĵ���������
// �ڹ�������cos(��)=1ʱ�����������Ѿ�У����֮���ٽ�����λ��������λУ����cos(��)=0.5Lʱ����У��
//===============================================================  
uint32_t  Phaseadjust[3];

uint32_t* Get_PhasegainData(float StandValue )
{
  float RealValue=0.0f ;      //��ʵֵ
  float Pgain=0;
  float Err=0;      
  uint32_t RegValue;
  
  for(uint8_t i = 0; i < 3; i++)
  {
    DelayNms (500);

    RegValue = ReadSampleRegister (0X01 + i ) ;
    RealValue = RegValue * (1)* P_K ; 
        
    Err = (RealValue - StandValue) / StandValue;

    Pgain = -Err / 1.732 ;

    if( Pgain >= 0)
    {  
    Phaseadjust[i] = ( uint32_t )( Pgain * 32768 );      
    }
    else if ( Pgain < 0)
    {          
    Phaseadjust[i] = (uint32_t )( 65536+ Pgain * 32768 );     
    }
  }
  
  return Phaseadjust;  
}







