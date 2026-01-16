#ifndef UART1_LOOPBACK_H
#define UART1_LOOPBACK_H

#include "lpc17xx.h"
#include <stdint.h>

void UART1_Init(void);
void UART1_SendChar(char c);
void UART1_SendString(const char* str);

#endif
