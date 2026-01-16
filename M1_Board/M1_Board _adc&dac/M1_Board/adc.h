#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#define CS_PIN (1 << 16)  // P0.16 for Chip Select
int16_t LTC1864_Read(void);

#endif
