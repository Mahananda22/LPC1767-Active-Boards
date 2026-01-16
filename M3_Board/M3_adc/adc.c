#include "LPC17xx.h"
#include "adc.h"
 
void ADC_Init(uint8_t channel)
{
    // Power ON ADC peripheral
    LPC_SC->PCONP |= (1 << 12);
 
    // Select pin function for given channel
    switch (channel)
    {
        case 0: // P0.23 - AD0.0
            LPC_PINCON->PINSEL1 &= ~(3 << 14);
            LPC_PINCON->PINSEL1 |=  (1 << 14);
            break;
        case 1: // P0.24 - AD0.1
            LPC_PINCON->PINSEL1 &= ~(3 << 16);
            LPC_PINCON->PINSEL1 |=  (1 << 16);
            break;
        case 2: // P0.25 - AD0.2
            LPC_PINCON->PINSEL1 &= ~(3 << 18);
            LPC_PINCON->PINSEL1 |=  (1 << 18);
            break;
        case 3: // P0.26 - AD0.3 // ADC number is 0 and channel is 3
            LPC_PINCON->PINSEL1 &= ~(3 << 20);
            LPC_PINCON->PINSEL1 |=  (1 << 20);
            break;
        default:
            return;
    }
 
    // Set PCLK for ADC to CCLK/4 (default)
    LPC_SC->PCLKSEL0 &= ~(3 << 24);
 
    // Enable ADC and set clock divider (max 13MHz)
    LPC_ADC->ADCR = (1 << 21) | (4 << 8);  // Enable | CLKDIV = 4 (CCLK / (4+1))
}
 
uint16_t ADC_Read(uint8_t channel)
{
    LPC_ADC->ADCR &= ~(0xFF);            // Clear channel selection
    LPC_ADC->ADCR |= (1 << channel);     // Select the desired ADC channel
 
    LPC_ADC->ADCR &= ~(7 << 24);         // Clear start bits
    LPC_ADC->ADCR |=  (1 << 24);         // Start conversion now
 
    while (!(LPC_ADC->ADGDR & (1 << 31)));  // Wait until DONE bit is set
 
    return (LPC_ADC->ADGDR >> 4) & 0xFFF;   // Return 12-bit ADC result
}