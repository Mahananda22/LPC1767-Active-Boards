#include "LPC17xx.h"
#include "dac.h"

#define CS_PIN (1 << 16)  // P0.16 for Chip Select
void TLV5618_WriteBoth(uint16_t channel, uint16_t valueA);

static void delay_ns(uint32_t ns) {
    // Approximate delay for ns - implement precise timing as needed
    for (volatile uint32_t i = 0; i < ns * 10; i++) {
        __NOP();
    }
}

void TLV5618_WriteBoth(uint16_t channel, uint16_t valueA) {
    // Step 1: Write to DAC B (buffer)
    uint16_t dataB = (1 << 15) | (1 << 14) | (0 << 13) | (0 << 12) | (valueA & 0x0FFF);
    LPC_GPIO3->FIOCLR = CS_PIN; // CS low
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = (dataB >> 8) & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = dataB & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    LPC_GPIO3->FIOSET = CS_PIN; // CS high

    delay_ns(100);

    // Step 2: Write to DAC A and update both outputs
    uint16_t dataA = (1 << 15) | (1 << 14) | (0 << 13) | (0 << 12) | (valueA & 0x0FFF);
    LPC_GPIO3->FIOCLR = CS_PIN;
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = (dataA >> 8) & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = dataA & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    delay_ns(100);
    LPC_GPIO3->FIOSET = CS_PIN;
}


