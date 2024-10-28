#ifndef _KEY_DRV_H_
#define _KEY_DRV_H_

#include <stdint.h>

#define KEY1_SHORT_PRESS   0X01
#define KEY1_DOUBLE_PRESS  0X51
#define KEY1_LONG_PRESS    0X81
#define KEY2_SHORT_PRESS   0X02
#define KEY2_DOUBLE_PRESS  0X52
#define KEY2_LONG_PRESS    0X82
#define KEY3_SHORT_PRESS   0X03
#define KEY3_DOUBLE_PRESS  0X53
#define KEY3_LONG_PRESS    0X83
#define KEY4_SHORT_PRESS   0X04
#define KEY4_DOUBLE_PRESS  0X54
#define KEY4_LONG_PRESS    0X84

/**
***********************************************************
* @brief ����Ӳ����ʼ��
* @param
* @return 
***********************************************************
*/
void KeyDrvInit(void);

/**
***********************************************************
* @brief ��ȡ������ֵ
* @param
* @return ����������ֵ���̰�0x01 0x02 0x03��
			                  ����0x81 0x82 0x83��
                        ˫��0x51 0x52 0x53��û�а���Ϊ0
***********************************************************
*/
uint8_t GetKeyVal(void);

#endif
