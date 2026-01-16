#ifndef __ADC_H__
#define __ADC_H__
 
#include <stdint.h>
 
void ADC_Init(uint8_t channel);
uint16_t ADC_Read(uint8_t channel);
 
#endif // __ADC_H__