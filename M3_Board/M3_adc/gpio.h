#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stddef.h>
#include "lpc17xx.h"

/* GPIO Functions */
void GPIO_InitPin(uint8_t portNum, uint8_t pinNum, uint8_t isOutput, uint8_t initialVal);
void GPIO_WritePin(uint8_t portNum, uint8_t pinNum, uint8_t val);
uint8_t GPIO_ReadPin(uint8_t portNum, uint8_t pinNum);
void Init_GPIO_ReadWrite_Pins(void);
void Init_M3Board_GPIO_Pins(void) ;
#endif
