#include <stdio.h>
#include "lpc17xx.h"
#include "can.h"
#include <stdint.h>
#include <stddef.h>
#include "gpio.h"
#include "command_processor.h"
#include "pinsel.h"
#include "uart.h"
#include "adc.h"

#define BAUD_RATE 115200
#define CMD_START_BYTE 0xAA


volatile CAN_Message_t can1_rx_msg_buffer;
volatile CAN_Message_t can2_rx_msg_buffer;

volatile uint8_t new_can1_message_flag = 0;
volatile uint8_t new_can2_message_flag = 0;

volatile uint32_t can1_TxRdy_Flag = 0;
volatile uint32_t can2_TxRdy_Flag = 0;

void Init_GPIO_ReadWrite_Pins(void);
void CAN1_CheckBusStatus(void);
void CAN2_CheckBusStatus(void);
void CAN_IRQHandler(void);

void CAN_IRQHandler(void) {
    uint32_t can1_icr = LPC_CAN1->ICR;
    uint32_t can2_icr = LPC_CAN2->ICR;

    if (can1_icr) {
        if (can1_icr & (1 << 0)) {
            if (new_can1_message_flag == 0) {
                can1_rx_msg_buffer.id = LPC_CAN1->RID;
                can1_rx_msg_buffer.len = (LPC_CAN1->RFS >> 16) & 0xF;
                if (can1_rx_msg_buffer.len > 8) can1_rx_msg_buffer.len = 8;

                uint32_t dataA = LPC_CAN1->RDA;
                uint32_t dataB = LPC_CAN1->RDB;

                for (uint8_t k = 0; k < 4; k++) can1_rx_msg_buffer.data[k] = (dataA >> (8 * k)) & 0xFF;
                for (uint8_t k = 0; k < 4; k++) can1_rx_msg_buffer.data[k + 4] = (dataB >> (8 * k)) & 0xFF;

                new_can1_message_flag = 1;
            } else {
                UART0_SendString("CAN1 RX Buffer Overrun!\r\n");
            }
            LPC_CAN1->CMR = (1 << 2);
        }
        if (can1_icr & (1 << 1)) can1_TxRdy_Flag = 1;
        if (can1_icr & (1 << 17)) UART0_SendString("CAN1 HW Overrun!\r\n");
        if (can1_icr & (1 << 21)) UART0_SendString("CAN1 Error Warning!\r\n");
        if (can1_icr & (1 << 22)) UART0_SendString("CAN1 Bus-Off!\r\n");
    }

    if (can2_icr) {
        if (can2_icr & (1 << 0)) {
            if (new_can2_message_flag == 0) {
                can2_rx_msg_buffer.id = LPC_CAN2->RID;
                can2_rx_msg_buffer.len = (LPC_CAN2->RFS >> 16) & 0xF;
                if (can2_rx_msg_buffer.len > 8) can2_rx_msg_buffer.len = 8;

                uint32_t dataA = LPC_CAN2->RDA;
                uint32_t dataB = LPC_CAN2->RDB;

                for (uint8_t k = 0; k < 4; k++) can2_rx_msg_buffer.data[k] = (dataA >> (8 * k)) & 0xFF;
                for (uint8_t k = 0; k < 4; k++) can2_rx_msg_buffer.data[k + 4] = (dataB >> (8 * k)) & 0xFF;

                new_can2_message_flag = 1;
            } else {
                UART0_SendString("CAN2 RX Buffer Overrun!\r\n");
            }
            LPC_CAN2->CMR = (1 << 2);
        }
        if (can2_icr & (1 << 1)) can2_TxRdy_Flag = 1;
        if (can2_icr & (1 << 17)) UART0_SendString("CAN2 HW Overrun!\r\n");
        if (can2_icr & (1 << 21)) UART0_SendString("CAN2 Error Warning!\r\n");
        if (can2_icr & (1 << 22)) UART0_SendString("CAN2 Bus-Off!\r\n");
    }
}

int main(void) {
    uint8_t tx_data[8];
//uint16_t adc_value;
    PinSelect_Init();
    UART0_Init(BAUD_RATE);
    UART1_Init(BAUD_RATE);
	ADC_Init(3);
  CAN1_Init();
  CAN2_Init();
	Init_M3Board_GPIO_Pins();
	
 // Init_GPIO_ReadWrite_Pins();
    __enable_irq();
    CAN_Filter_Bypass();
//char buf[50];
    while (1) {
//	          adc_value = ADC_Read(3); // Replace with your ADC read function

//        // Convert ADC value to string and send via UART0
//        snprintf(buf, sizeof(buf), "ADC Value: %u\r\n", adc_value);
//        UART0_SendString(buf);
        CAN1_CheckBusStatus();
        CAN2_CheckBusStatus();

        if (new_can1_message_flag) {
            for (int i = 0; i < can1_rx_msg_buffer.len; i++) {
                tx_data[i] = can1_rx_msg_buffer.data[i];
            }
            if (tx_data[0] == 0x01) {
                CAN_Delay(1000);
                CAN1_Send(0x00, tx_data, can1_rx_msg_buffer.len);
            }
            new_can1_message_flag = 0;
        }

        if (new_can2_message_flag) {
            for (int i = 0; i < can2_rx_msg_buffer.len; i++) {
                tx_data[i] = can2_rx_msg_buffer.data[i];
            }
            if (tx_data[0] == 0x02) {
                CAN_Delay(1000);
                CAN2_Send(0x00, tx_data, can2_rx_msg_buffer.len);
            }
            new_can2_message_flag = 0;
        }

        CAN_Delay(1000);
    }
}
