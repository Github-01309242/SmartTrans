
#include "stm32f10x.h"
#include <stdint.h>

#include "led_drv.h"

#define READ_SPI_MISO()          GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)	

#define SET_SPI_MOSI()           GPIO_SetBits(GPIOB, GPIO_Pin_14)          
#define CLR_SPI_MOSI()           GPIO_ResetBits(GPIOB, GPIO_Pin_14) 

#define SET_SPI_NSS()            GPIO_SetBits(GPIOB, GPIO_Pin_12)         
#define CLR_SPI_NSS()            GPIO_ResetBits(GPIOB, GPIO_Pin_12)

#define SET_SPI_SCK()            GPIO_SetBits(GPIOB, GPIO_Pin_15)          
#define CLR_SPI_SCK()            GPIO_ResetBits(GPIOB, GPIO_Pin_15) 

static void Gpio_Iint(void )
{
	GPIO_InitTypeDef GPIO_InitStructure; 	
	/*��������ʱ��*/
  RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOB, ENABLE );	
	/*ѡ������*/															   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_12 ;	
	/*��������ģʽΪͨ���������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*������������ */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIO_PORT*/
  GPIO_Init ( GPIOB, &GPIO_InitStructure );	
  
  
  //Ĭ��״̬�ߵ�ƽ
  GPIO_SetBits(GPIOB, GPIO_Pin_15) ;
  GPIO_SetBits(GPIOB, GPIO_Pin_14) ;
  GPIO_SetBits(GPIOB, GPIO_Pin_12) ;
  
  	/*ѡ������*/															   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;	
	/*��������ģʽΪͨ����������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	/*������������ */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIO_PORT*/
  GPIO_Init ( GPIOB, &GPIO_InitStructure );	

}  

static void SPIInit(void )
{
 Gpio_Iint();
  
 SET_SPI_NSS ();
 CLR_SPI_SCK (); 
}

static void SPIStart(void)
{
	CLR_SPI_NSS();
}

static void SPIStop(void)
{
	SET_SPI_NSS();
}

/**ģʽ0***/
static uint8_t SPIReadWriteByte(uint8_t senddata)
{
  uint8_t revData = 0x00;
  for(uint8_t i = 0; i < 8; i++)
  {
    if( senddata & 0x80  )
    {
      SET_SPI_MOSI();
    }
    else 
    {  
      CLR_SPI_MOSI ();   
    }
    senddata <<= 1;
    SET_SPI_SCK ();    //������
    revData <<= 1;
    revData |= READ_SPI_MISO();
    CLR_SPI_SCK ();    //�½���
  } 
  return revData;
}

#define DUMMY_BYTE							        0xFF

#define NORFLASH_WRITE_ENABLE						0x06
#define NORFLASH_WRITE_DISABLE						0x04
#define NORFLASH_READ_STATUS_REGISTER_1				0x05
#define NORFLASH_READ_STATUS_REGISTER_2				0x35
#define NORFLASH_WRITE_STATUS_REGISTER				0x01
#define NORFLASH_PAGE_PROGRAM						0x02
#define NORFLASH_QUAD_PAGE_PROGRAM					0x32
#define NORFLASH_BLOCK_ERASE_64KB					0xD8
#define NORFLASH_BLOCK_ERASE_32KB					0x52
#define NORFLASH_SECTOR_ERASE_4KB					0x20
#define NORFLASH_CHIP_ERASE							0xC7
#define NORFLASH_ERASE_SUSPEND						0x75
#define NORFLASH_ERASE_RESUME						0x7A
#define NORFLASH_POWER_DOWN							0xB9
#define NORFLASH_HIGH_PERFORMANCE_MODE				0xA3
#define NORFLASH_CONTINUOUS_READ_MODE_RESET			0xFF
#define NORFLASH_RELEASE_POWER_DOWN_HPM_DEVICE_ID	0xAB
#define NORFLASH_MANUFACTURER_DEVICE_ID				0x90
#define NORFLASH_READ_UNIQUE_ID						0x4B
#define NORFLASH_JEDEC_ID							0x9F
#define NORFLASH_READ_DATA							0x03
#define NORFLASH_FAST_READ							0x0B
#define NORFLASH_FAST_READ_DUAL_OUTPUT				0x3B
#define NORFLASH_FAST_READ_DUAL_IO					0xBB
#define NORFLASH_FAST_READ_QUAD_OUTPUT				0x6B
#define NORFLASH_FAST_READ_QUAD_IO					0xEB
#define NORFLASH_OCTAL_WORD_READ_QUAD_IO			0xE3

#define NORFLASH_PAGE_SIZE    					256                            //һҳ�Ĵ�С���ֽ�
#define NORFLASH_SECTOR_SIZE 						4096                           //������С���ֽ�
#define NORFLASH_BLOCK_SIZE  						(16 * NORFLASH_SECTOR_SIZE)     //16������ = 1����
#define NORFLASH_SIZE        						(128 * NORFLASH_BLOCK_SIZE)      //128���� = 1��оƬ


void NorflashDrvInit(void)
{
	SPIInit();
}

void ReadNorflashID(uint8_t *mid, uint16_t *did)
{
  SPIStart ();
  SPIReadWriteByte (NORFLASH_JEDEC_ID);
  *mid = SPIReadWriteByte (DUMMY_BYTE);
  *did = SPIReadWriteByte (DUMMY_BYTE) << 8;
  *did |= SPIReadWriteByte (DUMMY_BYTE);
  SPIStop ();
}

/**
*******************************************************************
* @function ָ����ַ��ʼ����ָ������������
* @param    readAddr,��ȡ��ַ
* @param    pBuffer,�����׵�ַ
* @param    len,Ҫ���������ݸ���
* @return   
*******************************************************************
*/
void ReadNorflashData(uint32_t readAddr, uint8_t *pBuffer, uint32_t len)
{
  SPIStart ();
  SPIReadWriteByte(NORFLASH_READ_DATA);
  SPIReadWriteByte( (readAddr & 0xFF0000)  >>16);
  SPIReadWriteByte( (readAddr & 0x00FF00)  >>8);
  SPIReadWriteByte( (readAddr & 0x0000FF)  );
  
  for(uint32_t  i = 0; i < len; i++)
  {
  *(pBuffer+i) = SPIReadWriteByte(DUMMY_BYTE);  
  }
  SPIStop ();
}


/**
*******************************************************************
* @function дʹ�� 
*******************************************************************
*/
static void EnableNorflashWrite(void)
{
	SPIStart();
	SPIReadWriteByte(NORFLASH_WRITE_ENABLE);
	SPIStop();
}


/**
*******************************************************************
* @function ��ֹдʹ�� 
*******************************************************************
*/
static void DisableNorflashWrite(void)
{
	SPIStart();
	SPIReadWriteByte(NORFLASH_WRITE_DISABLE);
	SPIStop();
}

/**
*******************************************************************
* @function �ȴ�д���
*******************************************************************
*/
static void WaitNorflashWriteEnd(void)
{
  uint32_t timeout=100000;
	SPIStart();
	SPIReadWriteByte(NORFLASH_READ_STATUS_REGISTER_1);
  /* has completed, the BUSY bit will be cleared to a 0 state*/
	while ((SPIReadWriteByte(DUMMY_BYTE) & 0x01) == 0x01)
	{
		timeout--;
		if (timeout == 0)
		{
			break;
		}
	}
  
	SPIStop();
}
  

/**
*******************************************************************
* @function дһ������
* @param    addr,д���ַ
* @param    pBuffer,�����׵�ַ
* @param    len,Ҫд������ݸ���
* @return   
*******************************************************************
*/
static void ProgramNorflashPage(uint32_t addr, uint8_t *pBuffer, uint32_t len)
{
  EnableNorflashWrite();
  SPIStart ();
  SPIReadWriteByte (NORFLASH_PAGE_PROGRAM);
  SPIReadWriteByte( (addr & 0xFF0000)  >>16);
  SPIReadWriteByte( (addr & 0x00FF00)  >>8);
  SPIReadWriteByte( (addr & 0x0000FF)  );

  while (len--)
  {
    SPIReadWriteByte(*pBuffer);
    pBuffer++;
  }
  
  SPIStop ();
  WaitNorflashWriteEnd ();
}

/**
*******************************************************************
* @function ָ����ַ��ʼд��ָ�����������ݣ�����ǰ��Ҫ����flash
* @param    writeAddr,д���ַ
* @param    pBuffer,�����׵�ַ
* @param    len,Ҫд������ݸ���
* @return                                                         
*******************************************************************
*/
void WriteNorflashData(uint32_t writeAddr, uint8_t *pBuffer, uint32_t len)
{ 
	uint8_t pageNum; 
	uint8_t addrOffset = writeAddr % NORFLASH_PAGE_SIZE; 	// mod����������һҳ�ڵ�ƫ�ƣ���writeAddr��NORFLASH_PAGE_SIZE��������������Ϊ0

	if (len > (NORFLASH_PAGE_SIZE - addrOffset))    // ��ҳ
	{
		ProgramNorflashPage(writeAddr, pBuffer, NORFLASH_PAGE_SIZE - addrOffset);  // д����ҳ
		writeAddr += NORFLASH_PAGE_SIZE - addrOffset;   // ���뵽ҳ��ַ
		pBuffer += NORFLASH_PAGE_SIZE - addrOffset;
		len -= NORFLASH_PAGE_SIZE - addrOffset;
		pageNum = len / NORFLASH_PAGE_SIZE;

		while (pageNum--) 
		{
			ProgramNorflashPage(writeAddr, pBuffer, NORFLASH_PAGE_SIZE);  // д��ҳ����
			writeAddr += NORFLASH_PAGE_SIZE;
			pBuffer += NORFLASH_PAGE_SIZE;
			len -= NORFLASH_PAGE_SIZE;
		}

		ProgramNorflashPage(writeAddr, pBuffer, len);  // д������ҳ��ʣ������
	}
	else  // û�п�ҳ
	{
		ProgramNorflashPage(writeAddr, pBuffer, len);
	}
}

/**
*******************************************************************
* @function ��������
*******************************************************************
*/
static void EraseNorflashSector(uint32_t eraseAddr)
{
	EnableNorflashWrite();
	SPIStart();
	SPIReadWriteByte(NORFLASH_SECTOR_ERASE_4KB);
	SPIReadWriteByte((eraseAddr & 0xFF0000) >> 16);
	SPIReadWriteByte((eraseAddr & 0x00FF00) >> 8);
	SPIReadWriteByte(eraseAddr & 0xFF);
	SPIStop();
	WaitNorflashWriteEnd();
  DisableNorflashWrite ();
}

/**
*******************************************************************
* @function ������eraseAddr��ʼ��eraseAddr + len����Ӧ������
* @param    eraseAddr,��ַ
* @param    len,��Ӧд�����ݵĸ���
* @return                                                         
*******************************************************************
*/
void EraseNorflashForWrite(uint32_t eraseAddr, uint32_t len)
{
	if (len == 0)
	{
		return;
	} 
	
	uint8_t sectorNum;
	uint8_t addrOffset = eraseAddr % NORFLASH_SECTOR_SIZE; 	// mod����������һ�����ڵ�ƫ�ƣ���eraseAddr��NORFLASH_SECTOR_SIZE��������������Ϊ0

	if (len > (NORFLASH_SECTOR_SIZE - addrOffset))      // ������
	{
		EraseNorflashSector(eraseAddr);                   // ��������
		eraseAddr += NORFLASH_SECTOR_SIZE - addrOffset;   // ���뵽������ַ
		len -= NORFLASH_SECTOR_SIZE - addrOffset;
		sectorNum = len / NORFLASH_SECTOR_SIZE;

		while (sectorNum--) 
		{
			EraseNorflashSector(eraseAddr);
			eraseAddr += NORFLASH_SECTOR_SIZE;
		}
		if (len % NORFLASH_SECTOR_SIZE != 0)
		{
			EraseNorflashSector(eraseAddr);
		}
	}
	else  // û�п�����
	{
		EraseNorflashSector(eraseAddr);
	}
}

/**
*******************************************************************
* @function оƬ��������                                                       
*******************************************************************
*/
void EraseNorflash(void )
{
  EnableNorflashWrite();
	SPIStart();
	SPIReadWriteByte(NORFLASH_CHIP_ERASE);
	SPIStop();
	WaitNorflashWriteEnd();
  DisableNorflashWrite();

}


#define   BUFFER_SIZE              4096
#define   FLASH_ADDRESS            4096

void NorflashDrvTest(void)
{
	uint8_t mid = 0;
	uint16_t did = 0;
	uint8_t  bufferWrite[BUFFER_SIZE];
	uint8_t  bufferRead[BUFFER_SIZE];

	ReadNorflashID(&mid, &did);
	
  for (uint16_t i = 0; i < BUFFER_SIZE; i++)
  { 
    bufferWrite[i]= i ;
  }
	
  for (uint16_t j = 0; j < 10; j++)
  {
    
    EraseNorflashForWrite(FLASH_ADDRESS*j, BUFFER_SIZE);
    WriteNorflashData(FLASH_ADDRESS*j, bufferWrite, BUFFER_SIZE);
      
    ReadNorflashData(FLASH_ADDRESS*j, bufferRead, BUFFER_SIZE);
    
//      for (uint16_t i = 0; i < BUFFER_SIZE; i++)
//      {
//            if (bufferRead[i] != bufferWrite[i])
//            {
//             
//              return;
//            }
//      }
  }  
  
  TurnOnLed (1);
  
}


