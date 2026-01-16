//#ifndef UART2_LOOPBACK_H
//#define UART2_LOOPBACK_H

//#include "lpc17xx.h"
//#include <stdint.h>

//void UART0_Init(void);
//void UART0_SendChar(char c);
//void UART0_SendString(const char* str);

//#endif
#ifndef UART_H
#define UART_H

#include "lpc17xx.h"
#include <stdint.h>
#define CMD_START_BYTE 0xAA  
typedef enum {
    UART_STATE_WAIT_FOR_START,
    UART_STATE_PROCESSING_LOOPBACK,
    UART_STATE_PROCESSING_CMD
} UART_HandlerState;


extern volatile UART_HandlerState uart0_handler_state;
extern volatile UART_HandlerState uart1_handler_state;
extern volatile uint32_t i;
extern volatile uint32_t j;
extern uint8_t bufftest[1000];
extern uint8_t bufftest1[1000];

void UART0_Init(uint32_t baudrate);
void UART1_Init(uint32_t baudrate);
void UART0_SendChar(char c);
void UART1_SendChar(char c);
void UART0_SendString(const char* str);
void UART1_SendString(const char* str);
void SendPacketLoopBack(LPC_UART_TypeDef* UARTx, uint8_t* data, uint32_t length);
void UART_SendByte(LPC_UART_TypeDef* UARTx, uint8_t byte);
#endif
