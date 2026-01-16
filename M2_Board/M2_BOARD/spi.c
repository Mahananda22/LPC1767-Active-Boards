#include "LPC17xx.h"
#include "spi.h"



void delay_ns(uint32_t ns) {
    for (volatile uint32_t i = 0; i < ns * 10; i++) {
        __NOP();
    }
}

#define CS_PIN_DAC1  (1 << 29)  // P1.29
#define CS_PIN_DAC2  (1 << 25)  // P1.25

void SPI0_Init(void) {
    // Enable power for SSP0
    LPC_SC->PCONP |= (1 << 21);

    // Set peripheral clock for SSP0 to CCLK/2
    LPC_SC->PCLKSEL1 &= ~(3 << 10);
    LPC_SC->PCLKSEL1 |=  (2 << 10);

    // Configure SPI0 pins
    LPC_PINCON->PINSEL0 &= ~(3 << 30);  // Clear P0.15
    LPC_PINCON->PINSEL0 |=  (2 << 30);  // SCK0

    LPC_PINCON->PINSEL1 &= ~(3 << 2);   // Clear P0.17
    LPC_PINCON->PINSEL1 |=  (2 << 2);   // MISO0

    LPC_PINCON->PINSEL1 &= ~(3 << 4);   // Clear P0.18
    LPC_PINCON->PINSEL1 |=  (2 << 4);   // MOSI0

    // SPI0 configuration: 8-bit data, SPI mode 1
    LPC_SSP0->CR0 = (7 << 0) | (1 << 7);
    LPC_SSP0->CPSR = 2;                 // Prescaler
    LPC_SSP0->CR1 = (1 << 1);           // Enable SSP0

    // Configure CS for DAC1 (P1.29)
    LPC_PINCON->PINSEL3 &= ~(3 << 26);  // P1.29 as GPIO
    LPC_GPIO1->FIODIR   |= CS_PIN_DAC1; // Output
    LPC_GPIO1->FIOSET    = CS_PIN_DAC1; // Set HIGH

    // Configure CS for DAC2 (P1.25)
    LPC_PINCON->PINSEL3 &= ~(3 << 18);  // P1.25 as GPIO
    LPC_GPIO1->FIODIR   |= CS_PIN_DAC2; // Output
    LPC_GPIO1->FIOSET    = CS_PIN_DAC2; // Set HIGH
}
