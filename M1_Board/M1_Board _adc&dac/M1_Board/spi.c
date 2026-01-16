#include "LPC17xx.h"
#include "spi.h"

//#define CS_PIN (1 << 16)  // P0.16 for Chip Select

//void SPI0_Init(void) {
//    LPC_SC->PCONP |= (1 << 21);      // Powers SSP0
//    LPC_SC->PCLKSEL1 &= ~(3 << 10);  // Clear bits 11:10 for SSP0
//    LPC_SC->PCLKSEL1 |=  (2 << 10);  // Set bits to '10' CCLK / 2

//    LPC_PINCON->PINSEL0 &= ~((3 << 30));         // Clear P0.15
//    LPC_PINCON->PINSEL0 |=  (2 << 30);           // SCK0
//    LPC_PINCON->PINSEL1 &= ~((3 << 2));          // Clear P0.17
//    LPC_PINCON->PINSEL1 |=  (2 << 2);            // MISO0
//    LPC_PINCON->PINSEL1 &= ~((3 << 4));          // Clear P0.18
//    LPC_PINCON->PINSEL1 |=  (2 << 4);            // MOSI0

//    LPC_SSP0->CR0 = (7 << 0) | (1 << 7);          // 8-bit data, SPI mode 1
//    LPC_SSP0->CPSR = 2;                           // Prescaler to PCLK / 2
//    LPC_SSP0->CR1 = (1 << 1);                     // Enable SSP0

//    LPC_GPIO0->FIODIR |= CS_PIN;  // CS pin as GPIO output
//    LPC_GPIO0->FIOSET = CS_PIN;   // Deselect (HIGH)
#define CS_PIN (1 << 26)  // Chip Select on P3.26

void SPI0_Init(void) {
    LPC_SC->PCONP |= (1 << 21); // Power up SSP0
    LPC_SC->PCLKSEL1 &= ~(3 << 10);
    LPC_SC->PCLKSEL1 |=  (2 << 10); // PCLK = CCLK/2

    // Configure SCLK0 -> P0.18
    LPC_PINCON->PINSEL1 &= ~(3 << 4);
    LPC_PINCON->PINSEL1 |=  (2 << 4);

    // Configure MISO0 -> P0.17
    LPC_PINCON->PINSEL1 &= ~(3 << 2);
    LPC_PINCON->PINSEL1 |=  (2 << 2);

    // Configure MOSI0 -> P0.15
    LPC_PINCON->PINSEL0 &= ~(3 << 30);
    LPC_PINCON->PINSEL0 |=  (2 << 30);

    // SPI Mode 1: CPOL = 0, CPHA = 1, 8-bit data
    LPC_SSP0->CR0 = (7 << 0) | (1 << 7);
    LPC_SSP0->CPSR = 2;       // Prescaler: PCLK / 2
    LPC_SSP0->CR1 = (1 << 1); // Enable SSP0

    // Configure CS pin (P3.26) as GPIO output
    LPC_PINCON->PINSEL7 &= ~(3 << 20); 
    LPC_GPIO3->FIODIR |= CS_PIN;
    LPC_GPIO3->FIOSET = CS_PIN; // Set CS high (inactive)
}
