
#ifndef __Max7219_DRV_H
#define	__Max7219_DRV_H

#include <stdint.h>

#define LEFT 0      // 显示数据靠左
#define RIGHT 1     // 显示数据靠右

//LED显示"0——9"一共十个数值的编码
static uint8_t const  table[]={0x7e,0x30,0x6d,0x79,0x33,0x5b,0x5f,0x70,0x7f,0x7b};

void Max7219Init(void );
/**
***********************************************************
* @brief MAX7219抗干扰保护程序
* @param
* @return
***********************************************************
*/
void Anti_interference_protection(void );
void Write7219(uint8_t Addr, uint8_t Data);

void DisplayText(char *text, int justify);

void DisplayTest(void );

#endif

