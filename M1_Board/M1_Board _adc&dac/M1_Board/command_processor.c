#include "command_processor.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "adc.h" 
#include "dac.h"
#define CMD_BUFFER_SIZE 32
#define ERR_INVALID_LENGTH 0xE1
#define ERR_INVALID_PIN    0xE2
#define ERR_UNINIT_PIN     0xE3
#define ERR_INVALID_CH 0xE7

static uint8_t cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t buffer_index;
static CommandRxState rx_state;
static LPC_UART_TypeDef* uart_used;

static void SendEscapedByte(uint8_t byte, uint8_t* cs) {
    *cs ^= byte;
    if (byte == PROTO_SOF) {
        UART_SendByte(uart_used, PROTO_ESC);
        UART_SendByte(uart_used, PROTO_ESC_SOF);
    } else if (byte == PROTO_EOF) {
        UART_SendByte(uart_used, PROTO_ESC);
        UART_SendByte(uart_used, PROTO_ESC_EOF);
    } else if (byte == PROTO_ESC) {
        UART_SendByte(uart_used, PROTO_ESC);
        UART_SendByte(uart_used, PROTO_ESC_ESC);
    } else {
        UART_SendByte(uart_used, byte);
    }
}




void SendPacket(uint8_t* data, uint8_t len) {
    uint8_t cs = 0;
    UART_SendByte(uart_used, PROTO_SOF);
    for (uint8_t i = 0; i < len; i++) {
        SendEscapedByte(data[i], &cs);
    }
    SendEscapedByte(cs, &(uint8_t){0});
    UART_SendByte(uart_used, PROTO_EOF);
}


static void SendAck(uint8_t cmd) {
    uint8_t response[2] = { cmd, RESP_ACK };
    SendPacket(response, 2);
}

static void SendNak(uint8_t code) {
    uint8_t response[2] = { code, RESP_NAK };
    SendPacket(response, 2);
}

static void ProcessCommand(void) {
    uint8_t cmd = cmd_buffer[0];
    uint8_t port, pin, val;
    uint8_t response[5];

//    uint8_t adc_num, adc_channel;
    uint16_t adc_val;


    switch (cmd) {
			

case CMD_READ_GPIO:
    if (buffer_index != 3) {
        SendNak(ERR_INVALID_LENGTH);
        return;
    }

    port = cmd_buffer[1];
    pin  = cmd_buffer[2];

    // Check if this is a special case for external ADC read
    if (port == 0xF8) {
        uint8_t adc_id = pin;

        if (adc_id != 0x01 && adc_id != 0x02) {
            SendNak(ERR_INVALID_CH);
            return;
        }

        uint16_t adc_val = LTC1864_Read();  // Later, you can switch to ADC2_Read() for adc_id == 0x02

        response[0] = CMD_READ_GPIO;  // Still using 0x52 as command
        response[1] = RESP_ACK;
        response[2] = (adc_val >> 8) & 0xFF;
        response[3] = adc_val & 0xFF;
        SendPacket(response, 4);
        return;
    }

    // Standard GPIO read
    if (port > 4 || pin > 31) {
        SendNak(ERR_INVALID_PIN);  // Invalid port/pin
        return;
    }

    val = GPIO_ReadPin(port, pin);
    if (val == 0xFF) {
        SendNak(ERR_UNINIT_PIN);   // Pin was not initialized
        return;
    }

    response[0] = CMD_READ_GPIO;
    response[1] = val;
    SendPacket(response, 2);
    break;
		
case CMD_WRITE_GPIO:

			if (cmd_buffer[1] == 0xF2) {
    if (buffer_index != 5) {  // CMD + F2 + channel + MSB + LSB
        SendNak(0xE3);
        return;
    }
        uint16_t channel = cmd_buffer[2];
        uint16_t valueA  = (cmd_buffer[3] << 8) | cmd_buffer[4];

        if (channel > 1) {
            SendNak(0xE4);  // Invalid DAC channel
            return;
        }

       TLV5618_WriteBoth(channel,valueA);
        SendAck(CMD_WRITE_GPIO);
    } else {
        // GPIO write command: [CMD_WRITE_GPIO, port, pin, val, checksum]
        if (buffer_index != 4) {
            SendNak(0xE3);  // Invalid length
            return;
        }

        uint8_t port = cmd_buffer[1];
        uint8_t pin  = cmd_buffer[2];
        uint8_t val  = cmd_buffer[3];

        if (pin > 31 || val > 1) {
            SendNak(0xE4);  // Invalid pin or value
            return;
        }

        GPIO_InitPin(port, pin, 1, val);
        GPIO_WritePin(port, pin, val);
        SendAck(CMD_WRITE_GPIO);
    }
    break;
	}
}

void CommandProcessor_ProcessByte(uint8_t byte, LPC_UART_TypeDef* uart) {
    static uint8_t cs = 0;
    uart_used = uart;

    switch (rx_state) {
        case STATE_WAIT_FOR_SOF:
            if (byte == PROTO_SOF) {
                buffer_index = 0;
                cs = 0;
                rx_state = STATE_RECEIVING;
            }
            break;

        case STATE_RECEIVING:
            if (byte == PROTO_ESC) {
                rx_state = STATE_ESCAPE;
            } else if (byte == PROTO_EOF) {
                if (buffer_index < 1) { SendNak(0xE6); rx_state = STATE_WAIT_FOR_SOF; return; }
                uint8_t received_cs = cmd_buffer[buffer_index - 1];
                uint8_t calc_cs = 0;
                for (uint8_t i = 0; i < buffer_index - 1; i++) calc_cs ^= cmd_buffer[i];
                if (calc_cs == received_cs) {
                    buffer_index--;
                    ProcessCommand();
                } else {
                    SendNak(0xE5);
                }
                rx_state = STATE_WAIT_FOR_SOF;
            } else {
                if (buffer_index < CMD_BUFFER_SIZE) {
                    cmd_buffer[buffer_index++] = byte;
                   // cs ^= byte;
                } else {
                    rx_state = STATE_WAIT_FOR_SOF;
                }
            }
            break;

        case STATE_ESCAPE:
            switch (byte) {
                case 0x01: byte = 0xAA; break;  // ESCAPE_AA
                case 0x02: byte = 0x55; break;  // ESCAPE_55
                case 0x03: byte = 0x7D; break;  // ESCAPE_7D
							  default: rx_state = STATE_WAIT_FOR_SOF; return;
                
            }
            if (buffer_index < CMD_BUFFER_SIZE) {
                cmd_buffer[buffer_index++] = byte;
                cs ^= byte;
                rx_state = STATE_RECEIVING;
            } else {
                rx_state = STATE_WAIT_FOR_SOF;
            }
            break;
					}
			
}


		
void CommandProcessor_Init(void) {
    buffer_index = 0;
    rx_state = STATE_WAIT_FOR_SOF;
    uart_used = LPC_UART0;
}

CommandRxState CommandProcessor_GetInternalState(void) {
    return rx_state;
	}
