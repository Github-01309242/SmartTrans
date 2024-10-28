
/* ���Һ��ģ���ͺţ�JLX12864G-1353-PC
   ���нӿڣ�
   �������ֿ�IC��JLX-GB2312�����ú���
   ����IC��:UC1701X
   ����Ѷ���ӣ���ַ  http://www.jlxlcd.cn;
*/

#include "stm32f10x.h"
#include "delay.h"
#include "lcd12864_drv.h"
#include "picture.h"
#include <stdint.h>

//-----------------����ӿڶ���---------------- 
#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_1)  //BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_1)

//-----------------LCD����IC�ӿڶ���---------------- 
#define LCD_CS_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_4)  //CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOA,GPIO_Pin_5)  //RET
#define LCD_RES_Set()  GPIO_SetBits(GPIOA,GPIO_Pin_5)

#define LCD_RS_Clr()   GPIO_ResetBits(GPIOA,GPIO_Pin_6)  //RS
#define LCD_RS_Set()   GPIO_SetBits(GPIOA,GPIO_Pin_6)

#define LCD_SDA_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)  //SDA
#define LCD_SDA_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define LCD_SCK_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_4)  //SCK
#define LCD_SCK_Set() GPIO_SetBits(GPIOC,GPIO_Pin_4)

//-----------------�ֿ�IC�ӿڶ���---------------- 
#define ROM_CS_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_5)  //ROM_CS
#define ROM_CS_Set() GPIO_SetBits(GPIOC,GPIO_Pin_5)

#define ROM_SCK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_0)  //ROM_SCK
#define ROM_SCK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_0)
 		     
#define ROM_IN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_10)  //ROM_IN
#define ROM_IN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_10)

#define ROM_OUT()	   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) 

static void LCD12864_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 	
	/*��������ʱ��*/
  RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE );	
	/*ѡ������*/															   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	
	/*��������ģʽΪͨ���������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*������������ */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIO_PORT*/
  GPIO_Init (GPIOA, &GPIO_InitStructure );	
  GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
  
  /*��������ʱ��*/
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC, ENABLE );	
	/*ѡ������*/															   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;	
	/*��������ģʽΪͨ���������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*������������ */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIO_PORT*/
  GPIO_Init (GPIOC, &GPIO_InitStructure );	
  GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);
  
	/*��������ʱ��*/
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE );	
	/*ѡ������*/															   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_10;	
	/*��������ģʽΪͨ���������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*������������ */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIO_PORT*/
  GPIO_Init ( GPIOB, &GPIO_InitStructure );	
  GPIO_SetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_10);
  
	/*��������ʱ��*/
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE );	
	/*ѡ������*/															   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
	/*��������ģʽΪͨ�ø�������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	/*������������ */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIO_PORT*/
  GPIO_Init (GPIOB, &GPIO_InitStructure );	
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
static void LCD_WR_DATA(uint8_t dat)
{
  LCD_CS_Clr ();
  LCD_RS_Set ();
  for(uint8_t i=0;i<8;i++)
   {
    LCD_SCK_Clr ();
    if(dat&0x80) LCD_SDA_Set ();
    else LCD_SDA_Clr ();;
    LCD_SCK_Set ();
    dat<<=1;
   }
   LCD_CS_Set ();
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
static void LCD_WR_REG(uint8_t  dat)
{
	LCD_CS_Clr ();
	LCD_RS_Clr ();
  for(uint8_t i=0; i<8; i++)
  {
    LCD_SCK_Clr ();
    if(dat&0x80) LCD_SDA_Set ();
    else LCD_SDA_Clr ();;
    LCD_SCK_Set ();
    DelayNus (2);
    dat<<=1;
  }
  LCD_CS_Set ();
}

/**
***********************************************************
* @brief LcdӲ����ʼ��
* @param
* @return 
***********************************************************
*/
void Lcd12864DrvInit(void)
{
  LCD12864_GPIO_Config ();	
	ROM_CS_Set ();
	LCD_RES_Clr ();    /*�͵�ƽ��λ*/
	DelayNms (20);
	LCD_RES_Set ();		 /*��λ���*/
	DelayNms (20);       
	LCD_WR_REG(0xe2);	 /*��λ*/
	DelayNms(5);
	LCD_WR_REG(0x2c);  /*��ѹ����1*/
	DelayNms(5);	
	LCD_WR_REG(0x2e);  /*��ѹ����2*/
	DelayNms(5);
	LCD_WR_REG(0x2f);  /*��ѹ����3*/
	DelayNms(5);
	LCD_WR_REG(0x24);  /*�ֵ��Աȶȣ������÷�Χ0x20��0x27*/
	LCD_WR_REG(0x81);  /*΢���Աȶ�*/
	LCD_WR_REG(0x1d);  /*0x28,΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f*/
	LCD_WR_REG(0xa2);  /*1/9ƫѹ�ȣ�bias��*/
	LCD_WR_REG(0xc8);  /*��ɨ��˳�򣺴��ϵ���*/
	LCD_WR_REG(0xa0);  /*��ɨ��˳�򣺴�����*/
	LCD_WR_REG(0x40);  /*��ʼ�У���һ�п�ʼ*/
	LCD_WR_REG(0xaf);  /*����ʾ*/
  
  
  clear_screen ();
  TurnOnScreen ();
}


static void lcd_address(uint8_t  page,uint8_t  column)
{
	column=column-0x01;
	LCD_WR_REG(0xb0+page-1);            /*����ҳ��ַ*/
	LCD_WR_REG(0x10+(column>>4&0x0f));	/*�����е�ַ�ĸ�4λ*/
	LCD_WR_REG(column&0x0f);	          /*�����е�ַ�ĵ�4λ*/	
}

/*ȫ������*/
void clear_screen(void )
{
	uint8_t  i,j;
	ROM_CS_Set ();
	for(j=0;j<8;j++)
	{
		lcd_address(j+1,1);
		for(i=0;i<128;i++)
		{
		  	LCD_WR_DATA(0x00);
		}
	}
}


/*��������*/
void clear_area(LcdArea_t *area )
{
	uint8_t  i,j;
	ROM_CS_Set ();
	for(j=area->y1  ;j<area->y2  ;j++)
	{
		lcd_address(j,area->x1);
		for(i=area->x1  ;i<area->x2  ;i++)
		{
		  	LCD_WR_DATA(0x00);
		}
	}
}


void LcdDrawPicture(uint16_t x, uint16_t y, PictureDesc_t *picDesc)
{
	uint8_t  i,j;
  uint8_t *picture;
  picture = (uint8_t *)picDesc->picture;
	for(j=y;j<y +( picDesc->height / 8) + 1;j++)
	{
		lcd_address(j,x*8);
		for (i=0;i<picDesc->width ;i++)
		{	
			LCD_WR_DATA( *picture );					/*д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1*/
			picture++;
		}
	}

}

/*��ʾ16x16����ͼ�񡢺��֡���Ƨ�ֻ�16x16���������ͼ��*/
static void display_graphic_16x16(uint8_t page,unsigned char column,unsigned char *dp)
{
	uint8_t i,j;
 	LCD_CS_Clr ();	
	ROM_CS_Set (); 	
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{	
			LCD_WR_DATA(*dp);					/*д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1*/
			dp++;
		}
	}
	LCD_CS_Set ();
}

/*��ʾ8x16����ͼ��ASCII, ��8x16����������ַ�������ͼ��*/
static void display_graphic_8x16(uint8_t page,unsigned char column,unsigned char *dp)
{
	uint8_t i,j;
	LCD_CS_Clr ();	
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<8;i++)
		{	
			LCD_WR_DATA(*dp);					/*д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1*/
			dp++;
		}
	}
	LCD_CS_Set ();
}

/****��ָ�����Ѷ�ֿ�IC***/
static void send_command_to_ROM( uint8_t  dat )
{
	uint8_t  i;
	for(i=0;i<8;i++ )
	{
		if(dat&0x80)
			ROM_IN_Set ();
		else
			ROM_IN_Clr ();
			dat = dat<<1;
			ROM_SCK_Clr ();
			ROM_SCK_Set ();
	}
}

/****�Ӿ���Ѷ�ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�***/
static uint8_t  get_data_from_ROM( )
{
	
	uint8_t i;
	uint8_t ret_data=0;
	ROM_SCK_Set ();
	for(i=0;i<8;i++)
	{
		ROM_SCK_Clr ();
		ret_data=ret_data<<1;
		if( ROM_OUT () )
			ret_data=ret_data+1;
		else
			ret_data=ret_data+0;
		ROM_SCK_Set ();
	}
	return(ret_data);
}

/*����ص�ַ��addrHigh����ַ���ֽ�,addrMid����ַ���ֽ�,addrLow����ַ���ֽڣ�����������DataLen���ֽڵ����ݵ� pBuff�ĵ�ַ*/
/*������ȡ*/
static void get_n_bytes_data_from_ROM(unsigned char addrHigh,unsigned char addrMid,unsigned char addrLow,unsigned char *pBuff,unsigned char DataLen )
{
	unsigned char i;
	ROM_CS_Clr (); ;
	LCD_CS_Set ();	
	ROM_SCK_Clr ();
	send_command_to_ROM(0x03);
	send_command_to_ROM(addrHigh);
	send_command_to_ROM(addrMid);
	send_command_to_ROM(addrLow);
	for(i = 0; i < DataLen; i++ )
  {
	 *(pBuff+i) = get_data_from_ROM();
	}
  ROM_CS_Set ();
}

unsigned long fontaddr=0;
static void display_GB2312_string(unsigned char y,unsigned char x,unsigned char *text )
{
	unsigned char i= 0;
	unsigned char addrHigh,addrMid,addrLow ;
	unsigned char fontbuf[32];			
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{						
			/*GB2312�����ھ���Ѷ�ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0*/
			/*���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ*/
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (unsigned long)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			addrMid = (fontaddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			addrLow = fontaddr&0xff;	           /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*ȡ32���ֽڵ����ݣ��浽"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);  /*��ʾ��LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=2;
			x+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{						
			/*15x16��GB2312��׼�����ֿ��ھ���Ѷ�ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0*/
			/*���ڵ���8λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ*/
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (unsigned long)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			addrMid = (fontaddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			addrLow = fontaddr&0xff;	           /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*ȡ32���ֽڵ����ݣ��浽"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);  /*��ʾLCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=2;
			x+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{		
     /*8x16��ASCII������ַ��ھ���Ѷ�ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/      
			unsigned char fontbuf[16];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 ); /*ȡ16���ֽڵ����ݣ��浽"fontbuf[32]"*/			
			display_graphic_8x16(y,x,fontbuf);       /*��ʾ8x16��ASCII�ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ��fontbuf[]Ϊ����*/
			i+=1;
			x+=8;
		}
		else
    {
			i++;
    }      
	}
}

///******************************************************************************
//����˵������ʾ����
//������ݣ�m������nָ��
//����ֵ��  ��
//******************************************************************************/
//u32 mypow(u8 m,u8 n)
//{
//	u32 result=1;	 
//	while(n--)result*=m;
//	return result;
//}

///******************************************************************************
//����˵������ʾ����
//������ݣ�
//����ֵ��  ��
//******************************************************************************/
//void LCD_ShowIntNum(unsigned char x,unsigned char y,u16 num,uint8_t len,uint8_t sizey)
//{ 
//  uint8_t  temp_Int;  
//	u8 t=0;
//	u8 enshow=0;
//	u8 sizex=sizey/2;
//	for(t=0;t<len;t++)
//	{
//		temp_Int = (num/mypow(10,len-t-1))%10 ;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp_Int==0)
//			{
//        display_GB2312_string(y,x*sizex+t*sizex,(unsigned char *)(&temp_Int) );
//				continue;
//			}else enshow=1; 
//		 	 
//		}
//    temp_Int += 0x30;
//	 	display_GB2312_string(y,x*sizex+t*sizex,(unsigned char *)(&temp_Int) );
//	}
//} 

/******************************************************************************
����˵������ʾһλ������
������ݣ�
����ֵ��  ��
******************************************************************************/
//void LCD_ShowFloatNum(unsigned char x,unsigned char y,float num,uint8_t len,uint8_t sizey)
//{         	
//  uint8_t  temp_Float;
//	u8 t=0,sizex=0;
//	u16 num1=0;
//	sizex = sizey/2;
//	num1=num*10;
//	for(t=0;t<len;t++)
//	{
//		temp_Float=(num1/mypow(10,len-t-1))%10;
//		if(t==(len-1))
//		{
//			display_GB2312_string(y,x*sizex+(len-1)*sizex,(unsigned char *)".");
//			t++;
//			len+=1;
//		}
//	 	temp_Float += 0x30;
//	 	display_GB2312_string(y,x*sizex+t*sizex,(unsigned char *)(&temp_Float) );
//	}
//}

/******************************************************************************
����˵������ʾ����
������ݣ�
����ֵ��  ��
******************************************************************************/

void LcdDrawChinese(unsigned char x,unsigned char y,unsigned  char * string)
{
  u8 sizex;
  sizex = 16/2;
  display_GB2312_string(y,x*sizex,string ); 
}

/******************************************************************************
����˵������ʾ�ַ���
������ݣ�
����ֵ��  ��
******************************************************************************/

void LcdDrawString(unsigned char x,unsigned char y,unsigned  char * string)
{
  u8 sizex;
  sizex = 16/2;
  display_GB2312_string(y,x*sizex,string ); 
}

void TurnOnScreen(void )
{
  LCD_BLK_Set();
} 

void TurnOffScreen(void )
{
  LCD_BLK_Clr();
}


