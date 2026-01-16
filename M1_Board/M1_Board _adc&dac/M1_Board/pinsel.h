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
 
/* CAN1 Pin Configuration: P0.21 (RD1), P0.22 (TD1) */
static inline void PinSelect_CAN1(void) {
    LPC_PINCON->PINSEL1 &= ~((3 << 10) | (3 << 12)); 
    LPC_PINCON->PINSEL1 |= (3 << 10) | (3 << 12);   
}
 
static inline void PinSelect_CAN2(void) {
    LPC_PINCON->PINSEL0 &= ~((3 << 8) | (3 << 10));       // Clear bits for P0.4 and P0.5
    LPC_PINCON->PINSEL0 |=  (2 << 8) | (2 << 10);         // Set function 2 for RD2 and TD2
}
 
 
 
static inline void PinSelect_Init(void) {
    PinSelect_UART0();
    PinSelect_UART1();
    PinSelect_CAN1();
    PinSelect_CAN2();
}
 
#endif // PINSELECTS_H
 

