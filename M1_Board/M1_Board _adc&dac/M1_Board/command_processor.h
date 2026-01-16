#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <stdint.h>
#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "adc.h"
#include "dac.h"

#define CMD_PROC_START_BYTE_1 0xAA
#define CMD_READ_GPIO         0x52
#define CMD_WRITE_GPIO        0x53
#define RESP_ACK              0x00
#define RESP_NAK              0xFF

#define CMD_READ_EXT_ADC     0x41
#define ERR_INVALID_LENGTH   0x01
#define ERR_CHECKSUM         0x02
#define ERR_INVALID_CH       0x03

#define PROTO_SOF             0xAA
#define PROTO_EOF             0x55
#define PROTO_ESC             0x7D   
#define PROTO_ESC_SOF         0x01
#define PROTO_ESC_EOF         0x02
#define PROTO_ESC_ESC         0x03

typedef enum {
    STATE_WAIT_FOR_SOF,
    STATE_RECEIVING,
    STATE_ESCAPE
} CommandRxState;
void CommandProcessor_Init(void);
void CommandProcessor_ProcessByte(uint8_t byte, LPC_UART_TypeDef* uart);
CommandRxState CommandProcessor_GetInternalState(void);

#endif
