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

// Optional: helper to initialize a specific board
void Init_M4Board_GPIO_Pins(void) {
    // Input Pins
    GPIO_InitPin(0, 26, 0, 0);
    GPIO_InitPin(0, 25, 0, 0);
    GPIO_InitPin(1, 24, 0, 0);
	  GPIO_InitPin(1, 25, 0, 0);
    // Output Pins
    //GPIO_InitPin(2, 12, 1, 0);
    
}
