
#ifndef _SWITCH_DRV_H_
#define _SWITCH_DRV_H_

#include <stdint.h>
//bit(0,0,0,0,INA,INB,INC,IND)
/**4-20mA**/
#define a_Calibration             0x00           //0b00000000  

#define a_CustomizedTesting       0x0C           //0b00001100   ��ֵ����

/**�����ɼ�**/
//��ѹ��������У��
#define r_Calib_UIP               0x03           //0b00000011

//��λУ��
#define r_Calib_Phase             0x07           //0b00000111

/**�ͻ�ʹ������״̬**/
#define r_Customer                0x0F           //0b00001111

/**
***********************************************************
* @brief ���뿪��Ӳ����ʼ��
* @param
* @return 
***********************************************************
*/
void SwitchDrvInit(void);

/**
***********************************************************
* @brief ��ȡ���뿪��״̬
* @param
* @return 0-У׼״̬
          1-��Чֵ״̬
***********************************************************
*/
uint8_t GetSwitchStatus(void);

#endif

