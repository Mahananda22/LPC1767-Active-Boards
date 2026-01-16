#include "LPC17xx.h"
#include "spi.h"
#include "dac.h"

#define CS_PIN (1 << 29) // P1.29
#define CS_PIN1 (1 << 25) // P1.25

void TLV5618_U27(uint16_t channel,uint16_t valueA) {
    // Write DAC B to buffer
//    uint16_t dataB = (0 << 15) | (1<< 14) | (0 << 13) | (0 << 12) | (valueB & 0x0FFF);
//    LPC_GPIO1->FIOCLR = CS_PIN1;
//    while (!(LPC_SSP0->SR & (1 << 1)));
//    LPC_SSP0->DR = (dataB >> 8) & 0xFF;
//    while (!(LPC_SSP0->SR & (1 << 4)));
//    while (!(LPC_SSP0->SR & (1 << 1)));
//    LPC_SSP0->DR = dataB & 0xFF;
//    while (!(LPC_SSP0->SR & (1 << 4)));
//    LPC_GPIO1->FIOSET = CS_PIN1;
//    delay_ns(100);  // Hold time
 
 
	  uint16_t dataA = (0 << 15) | (1 << 14) | (0 << 13) | (0 << 12) | (valueA & 0x0FFF);
    LPC_GPIO1->FIOCLR = CS_PIN1;
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = (dataA >> 8) & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = dataA & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    delay_ns(100);
    LPC_GPIO1->FIOSET = CS_PIN1;
}

void TLV5618_U16(uint16_t channel,uint16_t valueA) {
    // Write DAC B to buffer
//    uint16_t dataB = (0 << 15) | (1 << 14) | (0 << 13) | (0 << 12) | (valueA & 0x0FFF);
//    LPC_GPIO1->FIOCLR = CS_PIN;
//    while (!(LPC_SSP0->SR & (1 << 1)));
//    LPC_SSP0->DR = (dataB >> 8) & 0xFF;
//    while (!(LPC_SSP0->SR & (1 << 4)));
//    while (!(LPC_SSP0->SR & (1 << 1)));
//    LPC_SSP0->DR = dataB & 0xFF;
//    while (!(LPC_SSP0->SR & (1 << 4)));
//    LPC_GPIO1->FIOSET = CS_PIN;
//    delay_ns(100);  // Hold time
 
	  uint16_t dataA = (0 << 15) | (1 << 14) | (0 << 13) | (0 << 12) | (valueA & 0x0FFF);
    LPC_GPIO1->FIOCLR = CS_PIN;
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = (dataA >> 8) & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = dataA & 0xFF;
    while (!(LPC_SSP0->SR & (1 << 4)));
    delay_ns(100);
    LPC_GPIO1->FIOSET = CS_PIN;
}