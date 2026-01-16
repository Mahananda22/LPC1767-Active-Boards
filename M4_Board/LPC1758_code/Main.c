#include "UART2.h"
#include "pinsel.h"
#include "gpio.h"
#include "command_processor.h"
#define BAUD_RATE 115200

int main(void) {
    SystemInit();
	  PinSelect_Init();
    UART1_Init(BAUD_RATE);
    UART0_Init(BAUD_RATE);
	Init_M4Board_GPIO_Pins();
	CommandProcessor_Init();  
 //   NVIC_EnableIRQ(UART0_IRQn);  // Enable interrupt for UART0
 //   NVIC_EnableIRQ(UART1_IRQn);  
    while (1) {}


		}
