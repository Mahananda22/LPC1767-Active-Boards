#include "command_processor.h"
#include "gpio.h"
#include "uart.h"
#include "adc.h" 

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

    uint8_t adc_num, adc_channel;
    uint16_t adc_val;


    switch (cmd) {
			
	case CMD_READ_GPIO:

    // Check if second byte is 0xA0 ? treat this as a request to read ADC instead

    if (cmd_buffer[1] == 0xA0) {

        // Simulate call to CMD_READ_ADC logic

        if (buffer_index != 3) {

            SendNak(ERR_INVALID_LENGTH);

            return;

        }
 
        adc_num = cmd_buffer[1];       // Will be 0xA0

        adc_channel = cmd_buffer[2];
 
        if (adc_num != 0xA0 || adc_channel > 7) { // You can adjust this check as needed

            SendNak(ERR_INVALID_CH);

            return;

        }
 
        ADC_Init(adc_channel);

        adc_val = ADC_Read(adc_channel);
 
        response[0] = CMD_READ_GPIO;

        response[1] = RESP_ACK;                      // ACK = 0x00

        response[2] = (adc_val >> 8) & 0xFF;         // MSB

        response[3] = adc_val & 0xFF;                // LSB

        SendPacket(response, 4);                     // Checksum handled internally

        break;

    }
 
    // Normal GPIO read handling

    if (buffer_index != 3) {

        SendNak(ERR_INVALID_LENGTH);

        return;

    }
 
    port = cmd_buffer[1];

    pin  = cmd_buffer[2];

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
            if (buffer_index != 4) { SendNak(0xE3); return; }
            port = cmd_buffer[1];
            pin  = cmd_buffer[2];
            val  = cmd_buffer[3];
            if (pin > 31 || (val > 1)) { SendNak(0xE4); return; }
            GPIO_InitPin(port, pin, 1, val);
            GPIO_WritePin(port, pin, val);
            SendAck(CMD_WRITE_GPIO);
            break;
				  
				
				
        default:
            SendNak(0xE0);
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
    uart_used = (LPC_UART_TypeDef *)LPC_UART0;   // explicit cast

}

CommandRxState CommandProcessor_GetInternalState(void) {
    return rx_state;
	}
