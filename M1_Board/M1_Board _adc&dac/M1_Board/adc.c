#include "LPC17xx.h"
#include "spi.h"
#include "adc.h"

int16_t LTC1864_Read(void) {
    uint8_t msb, lsb;
    uint16_t result;

    LPC_GPIO0->FIOCLR = CS_PIN;

    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = 0x00;
    while (!(LPC_SSP0->SR & (1 << 2)));
    msb = LPC_SSP0->DR;

    while (!(LPC_SSP0->SR & (1 << 1)));
    LPC_SSP0->DR = 0x00;
    while (!(LPC_SSP0->SR & (1 << 2)));
    lsb = LPC_SSP0->DR;

    LPC_GPIO0->FIOSET = CS_PIN;

    result = ((msb << 8) | lsb) >> 4;
    return result;
}
