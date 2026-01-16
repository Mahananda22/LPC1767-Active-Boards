#ifndef PINSELECTS_H
#define PINSELECTS_H

#include "LPC17xx.h"

/* UART0 Pin Configuration: P0.2 (TXD0), P0.3 (RXD0) */
static inline void PinSelect_UART0(void) {
    LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));
    LPC_PINCON->PINSEL0 |= (1 << 4) | (1 << 6);   
	
}

/* UART1 Pin Configuration: P2.0 (TXD1), P2.1 (RXD1) */
static inline void PinSelect_UART1(void) {
    LPC_PINCON->PINSEL4 &= ~((3 << 0) | (3 << 2)); 
    LPC_PINCON->PINSEL4 |= (2 << 0) | (2 << 2);   
}


static inline void PinSelect_Init(void) {
    PinSelect_UART0();
    PinSelect_UART1();
 
}

#endif // PINSELECTS_H


