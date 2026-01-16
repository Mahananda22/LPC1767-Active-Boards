#include "uart.h"
#include "command_processor.h"

volatile UART_HandlerState uart0_handler_state = UART_STATE_WAIT_FOR_START;
volatile UART_HandlerState uart1_handler_state = UART_STATE_WAIT_FOR_START;

volatile uint32_t i = 0;
volatile uint32_t j = 0;
uint8_t bufftest[1000];
uint8_t bufftest1[1000];

void UART0_Init(uint32_t baudrate) {
    uint32_t pclk_val, div_val;

    LPC_SC->PCONP |= (1 << 3);
    LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(0x3 << 6)) | (0x01 << 6);

    LPC_UART0->LCR = 0x83;
    pclk_val = SystemCoreClock;

    switch ((LPC_SC->PCLKSEL0 >> 6) & 0x03) {
        case 0x00: pclk_val /= 4; break;
        case 0x01: pclk_val /= 1; break;
        case 0x02: pclk_val /= 2; break;
        default:   pclk_val /= 8; break;
    }

    div_val = (pclk_val / 16) / baudrate;
    LPC_UART0->DLL = div_val & 0xFF;
    LPC_UART0->DLM = (div_val >> 8) & 0xFF;

    LPC_UART0->LCR = 0x03;
    LPC_UART0->FCR = 0x07;
    LPC_UART0->IER = (1 << 0);

    NVIC_EnableIRQ(UART0_IRQn);
}

void UART1_Init(uint32_t baudrate) {
    uint32_t pclk_val, div_val;

    LPC_SC->PCONP |= (1 << 4);
    LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(0x3 << 8)) | (0x01 << 8);

    LPC_UART1->LCR = 0x83;
    pclk_val = SystemCoreClock;

    switch ((LPC_SC->PCLKSEL0 >> 8) & 0x03) {
        case 0x00: pclk_val /= 4; break;
        case 0x01: pclk_val /= 1; break;
        case 0x02: pclk_val /= 2; break;
        default:   pclk_val /= 8; break;
    }

    div_val = (pclk_val / 16) / baudrate;
    LPC_UART1->DLL = div_val & 0xFF;
    LPC_UART1->DLM = (div_val >> 8) & 0xFF;

    LPC_UART1->LCR = 0x03;
    LPC_UART1->FCR = 0x07;
    LPC_UART1->IER = (1 << 0);

    NVIC_EnableIRQ(UART1_IRQn);
}

void UART0_SendChar(char c) {
    while (!(LPC_UART0->LSR & (1 << 5)));
    LPC_UART0->THR = c;
}

void UART1_SendChar(char c) {
    while (!(LPC_UART1->LSR & (1 << 5)));
    LPC_UART1->THR = c;
}

void UART0_SendString(const char* str) {
    while (*str) {
        UART0_SendChar(*str++);
    }
}

void UART1_SendString(const char* str) {
    while (*str) {
        UART1_SendChar(*str++);
    }
}
void UART_SendByte(LPC_UART_TypeDef* UARTx, uint8_t byte) {
    while (!(UARTx->LSR & (1 << 5)));  // Wait until THR empty
    UARTx->THR = byte;
}

void SendPacketLoopBack(LPC_UART_TypeDef* UARTx, uint8_t* data, uint32_t length) {
    for (uint32_t k = 0; k < length; k++) {
        while (!(UARTx->LSR & (1 << 5)));
        UARTx->THR = data[k];
    }
}

void UART0_IRQHandler(void) {
    uint32_t iir = LPC_UART0->IIR;

    if ((iir & 0x01) == 0) {
        uint32_t int_id = (iir >> 1) & 0x07;

        if (int_id == 0x02 || int_id == 0x06) {
            while (LPC_UART0->LSR & (1 << 0)) {
                uint8_t byte = LPC_UART0->RBR;

                if (uart0_handler_state == UART_STATE_WAIT_FOR_START) {
                    if (byte == CMD_START_BYTE) {
                        uart0_handler_state = UART_STATE_PROCESSING_CMD;
                        CommandProcessor_ProcessByte(byte, LPC_UART0);
                    } else {
                        uart0_handler_state = UART_STATE_PROCESSING_LOOPBACK;
                        bufftest[i++] = byte;
                        if (byte == '\0') {
                            SendPacketLoopBack(LPC_UART0, bufftest, i);
                            uart0_handler_state = UART_STATE_WAIT_FOR_START;
                            i = 0;
                        }
                    }
                } else if (uart0_handler_state == UART_STATE_PROCESSING_LOOPBACK) {
                    bufftest[i++] = byte;
                    if (byte == '\0') {
                        SendPacketLoopBack(LPC_UART0, bufftest, i);
                        uart0_handler_state = UART_STATE_WAIT_FOR_START;
                        i = 0;
                    }
                } else if (uart0_handler_state == UART_STATE_PROCESSING_CMD) {
                    CommandProcessor_ProcessByte(byte, LPC_UART0);
                    if (CommandProcessor_GetInternalState() == STATE_WAIT_FOR_SOF) {
                        uart0_handler_state = UART_STATE_WAIT_FOR_START;
                    }
                }

                if (i >= sizeof(bufftest)) {
                    i = 0;
                    uart0_handler_state = UART_STATE_WAIT_FOR_START;
                }
            }
        }
    }
}

void UART1_IRQHandler(void) {
    uint32_t iir = LPC_UART1->IIR;

    if ((iir & 0x01) == 0) {
        uint32_t int_id = (iir >> 1) & 0x07;

        if (int_id == 0x02 || int_id == 0x06) {
            while (LPC_UART1->LSR & (1 << 0)) {
                uint8_t byte = LPC_UART1->RBR;

                if (uart1_handler_state == UART_STATE_WAIT_FOR_START) {
                    uart1_handler_state = UART_STATE_PROCESSING_LOOPBACK;
                }

                bufftest1[j++] = byte;
                if (byte == '\0') {
                    SendPacketLoopBack((LPC_UART_TypeDef *)LPC_UART1, bufftest1, j);
                    uart1_handler_state = UART_STATE_WAIT_FOR_START;
                    j = 0;
                }

                if (j >= sizeof(bufftest1)) {
                    j = 0;
                    uart1_handler_state = UART_STATE_WAIT_FOR_START;
                }
            }
        }
    }
}