#ifndef _NOR_FLASH_DRV_H_
#define _NOR_FLASH_DRV_H_

#include <stdint.h>


void NorflashDrvInit(void);
void ReadNorflashID(uint8_t *mid, uint16_t *did);
/**
*******************************************************************
* @function 指定地址开始读出指定个数的数据
* @param    readAddr,读取地址
* @param    pBuffer,数组首地址
* @param    len,要读出的数据个数
* @return   
*******************************************************************
*/
void ReadNorflashData(uint32_t readAddr, uint8_t *pBuffer, uint32_t len);
/**
*******************************************************************
* @function 指定地址开始写入指定个数的数据，调用前需要擦除flash
* @param    writeAddr,写入地址
* @param    pBuffer,数组首地址
* @param    len,要写入的数据个数
* @return                                                         
*******************************************************************
*/
void WriteNorflashData(uint32_t writeAddr, uint8_t *pBuffer, uint32_t len);

/**
*******************************************************************
* @function 擦除从eraseAddr开始到eraseAddr + len所对应的扇区
* @param    eraseAddr,地址
* @param    len,对应写入数据的个数
* @return                                                         
*******************************************************************
*/
void EraseNorflashForWrite(uint32_t eraseAddr, uint32_t len);

/**
*******************************************************************
* @function 芯片擦除函数                                                       
*******************************************************************
*/
void EraseNorflash(void );

void NorflashDrvTest(void);


#endif
