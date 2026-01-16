#include "gpio.h"
#include "LPC17xx.h"

#define MAX_PORTS 5
#define MAX_PINS 32

// Track which GPIO pins have been initialized
static uint8_t gpio_initialized[MAX_PORTS][MAX_PINS] = {0};

static LPC_GPIO_TypeDef* GetGPIOPort(uint8_t portNum) {
    switch (portNum) {
        case 0: return LPC_GPIO0;
        case 1: return LPC_GPIO1;
        case 2: return LPC_GPIO2;
        case 3: return LPC_GPIO3;
        case 4: return LPC_GPIO4;
        default: return NULL;
    }
}

void GPIO_InitPin(uint8_t portNum, uint8_t pinNum, uint8_t isOutput, uint8_t initialVal) {
    LPC_GPIO_TypeDef* gpio = GetGPIOPort(portNum);
    if (!gpio || pinNum > 31) return;

    gpio_initialized[portNum][pinNum] = 1; // Mark this pin as initialized

    if (isOutput) {
        gpio->FIODIR |= (1 << pinNum);
        if (initialVal)
            gpio->FIOSET = (1 << pinNum);
        else
            gpio->FIOCLR = (1 << pinNum);
    } else {
        gpio->FIODIR &= ~(1 << pinNum);
    }
}

void GPIO_WritePin(uint8_t portNum, uint8_t pinNum, uint8_t val) {
    LPC_GPIO_TypeDef* gpio = GetGPIOPort(portNum);
    if (!gpio || pinNum > 31) return;
    if (!gpio_initialized[portNum][pinNum]) return; // Skip if not initialized

    if (val)
        gpio->FIOSET = (1 << pinNum);
    else
        gpio->FIOCLR = (1 << pinNum);
}

uint8_t GPIO_ReadPin(uint8_t portNum, uint8_t pinNum) {
    LPC_GPIO_TypeDef* gpio = GetGPIOPort(portNum);
    if (!gpio || pinNum > 31) return 0xFF;
    if (!gpio_initialized[portNum][pinNum]) return 0xFF; // Not initialized

    return (gpio->FIOPIN & (1 << pinNum)) ? 1 : 0;
}


void Init_M1Board_GPIO_Pins(void) {
    // Output Pins
    GPIO_InitPin(1, 22, 1, 0);   // P1.22 - Output, initial LOW
    GPIO_InitPin(1, 21, 1, 0);   // P1.21 - Output, initial LOW
    GPIO_InitPin(2, 7, 1, 0);    // P2.7  - Output, initial LOW
    GPIO_InitPin(2, 6, 1, 0);    // P2.6  - Output, initial LOW
    GPIO_InitPin(2, 5, 1, 0);    // P2.5  - Output, initial LOW
    GPIO_InitPin(0, 23, 1, 0);    // P2.5  - Output, initial LOW
	  GPIO_InitPin(0, 25, 1, 0);   //P0.25
	  GPIO_InitPin(0, 11, 1, 0);   //P0.25
	  GPIO_InitPin(0, 10, 1, 0);   //P0.25
	  GPIO_InitPin(0, 1, 1, 0);   //P0.25
	  GPIO_InitPin(0, 0, 1, 0);   //P0.25
    // Input Pins
	  GPIO_InitPin(0, 19, 0, 0);   // P0.19 - Input
    GPIO_InitPin(1, 20, 0, 0);   // P1.20 - Input
    GPIO_InitPin(2, 4, 0, 0);    // P2.4  - Input
    GPIO_InitPin(2, 3, 0, 0);    // P2.3  - Input
    GPIO_InitPin(2, 2, 0, 0);    // P2.2  - Input
	 // GPIO_InitPin(0, 23, 0, 0);

	 GPIO_InitPin(1, 29, 0, 0);   //P0.25
	 GPIO_InitPin(1, 28, 0, 0);   //P0.25
	 GPIO_InitPin(1, 27, 0, 0);   //P0.25
}


//#include "gpio.h"
//#include "LPC17xx.h"

//#define MAX_PORTS 5
//#define MAX_PINS 32

//// Track which GPIO pins have been initialized
//static uint8_t gpio_initialized[MAX_PORTS][MAX_PINS] = {0};

//static LPC_GPIO_TypeDef* GetGPIOPort(uint8_t portNum) {
//    switch (portNum) {
//        case 0: return LPC_GPIO0;
//        case 1: return LPC_GPIO1;
//        case 2: return LPC_GPIO2;
//        case 3: return LPC_GPIO3;
//        case 4: return LPC_GPIO4;
//        default: return NULL;
//    }
//}

//void GPIO_InitPin(uint8_t portNum, uint8_t pinNum, uint8_t isOutput, uint8_t initialVal) {
//    LPC_GPIO_TypeDef* gpio = GetGPIOPort(portNum);
//    if (!gpio || pinNum > 31) return;

//    gpio_initialized[portNum][pinNum] = 1; // Mark this pin as initialized

//    if (isOutput) {
//        gpio->FIODIR |= (1 << pinNum);
//        if (initialVal)
//            gpio->FIOSET = (1 << pinNum);
//        else
//            gpio->FIOCLR = (1 << pinNum);
//    } else {
//        gpio->FIODIR &= ~(1 << pinNum);
//    }
//}

//void GPIO_WritePin(uint8_t portNum, uint8_t pinNum, uint8_t val) {
//    LPC_GPIO_TypeDef* gpio = GetGPIOPort(portNum);
//    if (!gpio || pinNum > 31) return;
//    if (!gpio_initialized[portNum][pinNum]) return; // Skip if not initialized

//    if (val)
//        gpio->FIOSET = (1 << pinNum);
//    else
//        gpio->FIOCLR = (1 << pinNum);
//}

//uint8_t GPIO_ReadPin(uint8_t portNum, uint8_t pinNum) {
//    LPC_GPIO_TypeDef* gpio = GetGPIOPort(portNum);
//    if (!gpio || pinNum > 31) return 0xFF;
//    if (!gpio_initialized[portNum][pinNum]) return 0xFF; // Not initialized

//    return (gpio->FIOPIN & (1 << pinNum)) ? 1 : 0;
//}
//void Init_M3Board_GPIO_Pins(void) {
//    // Output Pins
//    GPIO_InitPin(0, 9, 1, 0);    // P0.9 - Output, initial LOW
//    GPIO_InitPin(2, 2, 1, 0);    // P2.2 - Output, initial LOW
//    GPIO_InitPin(2, 3, 1, 0);    // P2.3 - Output, initial LOW
//    GPIO_InitPin(2, 11, 1, 0);
//	  GPIO_InitPin(2, 13, 1, 0);
//	  GPIO_InitPin(0, 11, 1, 0);
//	  GPIO_InitPin(3, 25, 1, 0);
//	  GPIO_InitPin(3, 26, 1, 0);
//    // Input Pins
//    GPIO_InitPin(0, 8, 0, 0);    // P0.8 - Input
//    GPIO_InitPin(0, 6, 0, 0);    // P0.6 - Input
//    GPIO_InitPin(0, 7, 0, 0);    // P0.7 - Input
//	 GPIO_InitPin(0, 10, 0, 0);
//	 GPIO_InitPin(0, 1, 0, 0);
//    GPIO_InitPin(0, 0, 0, 0);
//	 GPIO_InitPin(1, 29, 0, 0);
//	 GPIO_InitPin(1, 28, 0, 0);
//	 GPIO_InitPin(1, 22, 0, 0);
//	 GPIO_InitPin(1, 20, 0, 0);
//	GPIO_InitPin(1, 21, 0, 0);
//	 GPIO_InitPin(1, 19, 0, 0);
//}

