#include "UART1.h"
#include "command_processor.h"

#define UART1_BUF_SIZE 512
#define UART1_BAUDRATE 115200

static volatile uint8_t uart1_buf[UART1_BUF_SIZE];
static volatile uint32_t uart1_idx = 0;

#include <stdint.h>
#define CMD_START_BYTE 0xAA  
typedef enum {
    UART_STATE_WAIT_FOR_START,
    UART_STATE_PROCESSING_LOOPBACK,
    UART_STATE_PROCESSING_CMD
} UART_HandlerState;

static  void UART1_PinSelect(void) {
    LPC_PINCON->PINSEL4 &= ~((3 << 0) | (3 << 2)); // Clear P2.0 and P2.1
    LPC_PINCON->PINSEL4 |=  (2 << 0) | (2 << 2);   // P2.0 TXD1, P2.1 RXD1
}

void UART1_Init(void) {
	uint32_t fdiv ;
    LPC_SC->PCONP |= (1 << 4); // UART1 Power ON
    LPC_SC->PCLKSEL0 &= ~(3 << 8);
    LPC_SC->PCLKSEL0 |=  (1 << 8); // PCLK = CCLK

    UART1_PinSelect();

    LPC_UART1->LCR = 0x83; // Enable DLAB
    fdiv = SystemCoreClock / (16 * UART1_BAUDRATE);
    LPC_UART1->DLM = (fdiv >> 8) & 0xFF;
    LPC_UART1->DLL = fdiv & 0xFF;
    LPC_UART1->LCR = 0x03; // 8-bit, 1 stop, no parity
    LPC_UART1->FCR = 0x07; // Enable FIFO
    LPC_UART1->IER = (1 << 0); // Enable RBR interrupt

    NVIC_EnableIRQ(UART1_IRQn);
}

void UART1_SendChar(char c) {
    while (!(LPC_UART1->LSR & (1 << 5)));
    LPC_UART1->THR = c;
}

void UART1_SendString(const char* str) {
    while (*str) UART1_SendChar(*str++);
}

void UART1_IRQHandler(void) {
	uint32_t i;
    while (LPC_UART1->LSR & 0x01) {
        uint8_t byte = LPC_UART1->RBR;
        uart1_buf[uart1_idx++] = byte;

        if (byte == '\0' || uart1_idx >= UART1_BUF_SIZE) {
            for (i = 0; i < uart1_idx; i++) {
                UART1_SendChar(uart1_buf[i]);
            }
            uart1_idx = 0;
        }
    }
}
