#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f10x.h"

#define FLASH_PAGE_SIZE  		   		0x800        // 2K
#define FLASH_END_ADDRESS					0x0803FFFF   // 256K

/**
*******************************************************************
* @function ָ����ַ��ʼ����ָ������������
* @param    readAddr,��ȡ��ַ
* @param    pBuffer,�����׵�ַ
* @param    numToRead,Ҫ���������ݸ���
* @return   
*******************************************************************
*/
bool FlashRead(uint32_t readAddr, uint8_t *pBuffer, uint32_t numToRead)
{	
	if ((readAddr + numToRead) > FLASH_END_ADDRESS)
	{
		return false;
	}
	
	uint32_t addr = readAddr;
	for (uint32_t i = 0; i < numToRead; i++) 
	{
		*pBuffer = *(uint8_t *)addr;
		addr = addr + 1;
		pBuffer++;
	}
	return true;
}

/**
*******************************************************************
* @function ָ����ַ��ʼд��ָ������������
* @param    writeAddr,д���ַ
* @param    pBuffer,�����׵�ַ
* @param    numToWrite,Ҫд������ݸ���
* @return                                                         
*******************************************************************
*/
bool FlashWrite(uint32_t writeAddr, uint8_t *pBuffer, uint32_t numToWrite)
{	
	if ((writeAddr + numToWrite) > FLASH_END_ADDRESS)
	{
		return false;
	}
	if (writeAddr % 2 == 1)   // ����(2�ֽ�)д�룬��ַҪ����
	{
		return false;
	}
	uint16_t temp;
	
	FLASH_Status  fmcState = FLASH_COMPLETE;
	
	  /* ���� */
  FLASH_Unlock();

	for (uint32_t i = 0; i < numToWrite / 2; i++) 
	{
		  /* �����־λ */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		fmcState = FLASH_ProgramHalfWord(writeAddr, *(uint16_t *)pBuffer);
		if (fmcState != FLASH_COMPLETE)
		{
			FLASH_Lock();
			return false;
		}

		pBuffer += 2;
		writeAddr += 2;
	}
	if (numToWrite % 2)
	{
		  /* �����־λ */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		temp = *pBuffer | 0xff00;
		fmcState = FLASH_ProgramHalfWord(writeAddr, temp);
	}

	FLASH_Lock();
	return true;
}

/**
*******************************************************************
* @function ������eraseAddr��ʼ��eraseAddr + numToErase��ҳ
* @param    eraseAddr,��ַ
* @param    numToErase,��Ӧд������ʱ�ĸ���
* @return                                                         
*******************************************************************
*/
bool FlashErase(uint32_t eraseAddr, uint32_t numToErase)
{
	if (numToErase == 0 || (eraseAddr + numToErase) > FLASH_END_ADDRESS)
	{
		return false;
	} 
	
	uint8_t pageNum;
	uint8_t addrOffset = eraseAddr % FLASH_PAGE_SIZE; 	// mod����������һҳ�ڵ�ƫ�ƣ���eraseAddr��FLASH_PAGE_SIZE��������������Ϊ0

	FLASH_Status  fmcState = FLASH_COMPLETE;
	
	  /* ���� */
  FLASH_Unlock();

	if (numToErase > (FLASH_PAGE_SIZE - addrOffset))           // ��ҳ
	{
		  /* �����־λ */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		fmcState = FLASH_ErasePage(eraseAddr);           // ����ҳ
		if (fmcState != FLASH_COMPLETE)
		{
			goto erase_err;
		}
		
		eraseAddr += FLASH_PAGE_SIZE - addrOffset;   // ���뵽ҳ��ַ
		numToErase -= FLASH_PAGE_SIZE - addrOffset;
		pageNum = numToErase / FLASH_PAGE_SIZE;

		while (pageNum--) 
		{
		  /* �����־λ */
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
      fmcState = FLASH_ErasePage(eraseAddr);           // ����ҳ
			if (fmcState != FLASH_COMPLETE)
			{
				goto erase_err;
			}
			eraseAddr += FLASH_PAGE_SIZE;
		}
		if (numToErase % FLASH_PAGE_SIZE != 0)
		{
		  /* �����־λ */
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
      fmcState = FLASH_ErasePage(eraseAddr);           // ����ҳ
			if (fmcState != FLASH_COMPLETE)
			{
				goto erase_err;
			}
		}
	}
	else  // û�п�ҳ
	{
      /* �����־λ */
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
      fmcState = FLASH_ErasePage(eraseAddr);           // ����ҳ
      if (fmcState != FLASH_COMPLETE)
		{
			goto erase_err;
		}
	}
	/* lock the main FMC after the erase operation */
    FLASH_Lock();
	return true;

erase_err:
	/* lock the main FMC after the erase operation */
    FLASH_Lock();
	return false;
}


#define BUFFER_SIZE                   3000
#define FLASH_TEST_ADDRESS            0x0802F004  
void FlashDrvTest(void)
{
	uint8_t bufferWrite[BUFFER_SIZE];
    uint8_t bufferRead[BUFFER_SIZE];
	
	printf("flash writing data��\n");
	for (uint16_t i = 0; i < BUFFER_SIZE; i++)
	{ 
		bufferWrite[i] = i + 1;
		printf("0x%02X ", bufferWrite[i]);
    }
	printf("\n��ʼд��\n");
	
	if (!FlashErase(FLASH_TEST_ADDRESS, BUFFER_SIZE))
	{
		printf("Flashд���ݹ��ϣ����Ų飡\n");
		return;
	}

	if (!FlashWrite(FLASH_TEST_ADDRESS, bufferWrite, BUFFER_SIZE))
	{
		printf("Flashд���ݹ��ϣ����Ų飡\n");
		return;
	}
	
	printf("��ʼ��ȡ\n");
	if (!FlashRead(FLASH_TEST_ADDRESS, bufferRead, BUFFER_SIZE))
	{
		printf("Flash�����ݹ��ϣ����Ų飡\n");
		return;
	}
	for (uint16_t i = 0; i < BUFFER_SIZE; i++)
	{
        if (bufferRead[i] != bufferWrite[i]){
            printf("0x%02X ", bufferRead[i]);
            printf("Flash���Թ��ϣ����Ų飡\n");
            return;
        }
        printf("0x%02X ", bufferRead[i]);

    }
	printf("\nFlash����ͨ����\n");
}
