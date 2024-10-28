
#ifndef __ANALOGOUT_APP_H
#define	__ANALOGOUT_APP_H

#include <stdint.h>


typedef enum  
{
  URms,
  IRms,
  Power,
  Pft,
  Freq, 
  Other,
} analog_class;

analog_class Assay(uint8_t data );

void AnalogOutTask(void );

#endif

