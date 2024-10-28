#ifndef _NOR_FLASH_DRV_H_
#define _NOR_FLASH_DRV_H_

#include <stdint.h>


void NorflashDrvInit(void);
void ReadNorflashID(uint8_t *mid, uint16_t *did);
/**
*******************************************************************
* @function ָ����ַ��ʼ����ָ������������
* @param    readAddr,��ȡ��ַ
* @param    pBuffer,�����׵�ַ
* @param    len,Ҫ���������ݸ���
* @return   
*******************************************************************
*/
void ReadNorflashData(uint32_t readAddr, uint8_t *pBuffer, uint32_t len);
/**
*******************************************************************
* @function ָ����ַ��ʼд��ָ�����������ݣ�����ǰ��Ҫ����flash
* @param    writeAddr,д���ַ
* @param    pBuffer,�����׵�ַ
* @param    len,Ҫд������ݸ���
* @return                                                         
*******************************************************************
*/
void WriteNorflashData(uint32_t writeAddr, uint8_t *pBuffer, uint32_t len);

/**
*******************************************************************
* @function ������eraseAddr��ʼ��eraseAddr + len����Ӧ������
* @param    eraseAddr,��ַ
* @param    len,��Ӧд�����ݵĸ���
* @return                                                         
*******************************************************************
*/
void EraseNorflashForWrite(uint32_t eraseAddr, uint32_t len);

/**
*******************************************************************
* @function оƬ��������                                                       
*******************************************************************
*/
void EraseNorflash(void );

void NorflashDrvTest(void);


#endif