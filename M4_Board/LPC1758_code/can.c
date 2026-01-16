#include "LPC17xx.h"
#include "can.h"

void CAN_Delay(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 5000; j++);
    }
}

void CAN_Filter_Bypass(void) {
    LPC_CANAF->AFMR = 0x02;
}

void CAN_ConfigBaudrate(uint8_t canId, uint32_t baudrate) {
    const uint32_t PCLK_CAN = 30000000;
    uint32_t brp_val, tseg1_val, tseg2_val, sjw_val;

    if (baudrate == 500000) {
        brp_val = (PCLK_CAN / (baudrate * 12)) - 1;
        tseg1_val = 7;
        tseg2_val = 2;
        sjw_val = 0;
    } else if (baudrate == 125000) {
        brp_val = (PCLK_CAN / (baudrate * 15)) - 1;
        tseg1_val = 9;
        tseg2_val = 3;
        sjw_val = 0;
    } else {
        return;
    }

    uint32_t btr = (sjw_val << 24) | (tseg2_val << 20) | (tseg1_val << 16) | brp_val;

    if (canId == 1) {
        LPC_CAN1->BTR = btr;
    } else if (canId == 2) {
        LPC_CAN2->BTR = btr;
    }
}

static inline void PinSelect_CAN1(void) {
    LPC_PINCON->PINSEL0 &= ~((3 << 0) | (3 << 2)); // Clear P0.0 and P0.1
    LPC_PINCON->PINSEL0 |=  (1 << 0) | (1 << 2);   // P0.0 = RD1, P0.1 = TD1 (Function 1)
}

void CAN1_Init(void) {
    LPC_SC->PCONP |= (1 << 13);            // Power on CAN1
    LPC_SC->PCLKSEL1 &= ~(3 << 26);        // PCLK_CAN1 = CCLK/4 (default)

    PinSelect_CAN1();                      // Configure P0.0 and P0.1 for CAN1

    LPC_CAN1->MOD = 1;                     // Enter reset mode
    while (!(LPC_CAN1->MOD & 1));         // Wait for reset mode

    LPC_CAN1->CMR = (1 << 3) | (1 << 4);   // Clear TX and RX
    LPC_CAN1->GSR = 0;
    LPC_CAN1->IER = 0;                    // Disable interrupts during setup
    LPC_CAN1->MOD |= (1 << 2);            // Self-test mode (optional)

    CAN_ConfigBaudrate(1, 500000);        // Set baudrate to 500 kbps

    LPC_CAN1->IER = (1 << 0)  |  // RIE: Receive interrupt
                    (1 << 1)  |  // TIE1: Transmit interrupt
                    (1 << 17) |  // BEIE: Bus Error
                    (1 << 21) |  // DOIE: Data Overrun
                    (1 << 22) |  // EPIE: Error Passive
                    (1 << 23);   // ALIE: Arbitration Lost

//        LPC_CAN1->MOD &= ~1;
//        while (LPC_CAN1->MOD & 1);
//        LPC_CAN1->MOD |= (1 << 2);               // Wait until normal mode

          LPC_CAN1->MOD |= (1 << 2); // Set to Normal mode
          while (!(LPC_CAN1->MOD & (1 << 2))); // Wait for mode change
          NVIC_EnableIRQ(CAN_IRQn);             // Enable CAN1 IRQ
}



int CAN1_Send(uint32_t id, uint8_t *data, uint8_t len) {
    if (!(LPC_CAN1->GSR & (1 << 2))) {
        return 0;
    }

    LPC_CAN1->TID1 = id;
    LPC_CAN1->TFI1 = (len & 0xF) << 16;

    LPC_CAN1->TDA1 = ((uint32_t)data[3] << 24) |
                     ((uint32_t)data[2] << 16) |
                     ((uint32_t)data[1] << 8)  |
                     ((uint32_t)data[0] << 0);

    LPC_CAN1->TDB1 = ((uint32_t)data[7] << 24) |
                     ((uint32_t)data[6] << 16) |
                     ((uint32_t)data[5] << 8)  |
                     ((uint32_t)data[4] << 0);

    LPC_CAN1->CMR = (1 << 0);
    return 1;
}

/*int CAN2_Send(uint32_t id, uint8_t *data, uint8_t len) {
    if (!(LPC_CAN2->GSR & (1 << 2))) {
        return 0;
    }

    LPC_CAN2->TID1 = id;
    LPC_CAN2->TFI1 = (len & 0xF) << 16;

    LPC_CAN2->TDA1 = ((uint32_t)data[3] << 24) |
                     ((uint32_t)data[2] << 16) |
                     ((uint32_t)data[1] << 8)  |
                     ((uint32_t)data[0] << 0);

    LPC_CAN2->TDB1 = ((uint32_t)data[7] << 24) |
                     ((uint32_t)data[6] << 16) |
                     ((uint32_t)data[5] << 8)  |
                     ((uint32_t)data[4] << 0);

    LPC_CAN2->CMR = (1 << 0);
    return 1;
}*/

//void CAN1_CheckBusStatus(void) {
//    if (LPC_CAN1->GSR & (1 << 7)) {
//        LPC_CAN1->MOD = 1;
//        while (!(LPC_CAN1->MOD & 1));

//        CAN_Delay(10);

//        LPC_CAN1->MOD &= ~1;
//        while (LPC_CAN1->MOD & 1);

//        LPC_CAN1->MOD |= (1 << 2);
//    }
//}
void CAN1_CheckBusStatus(void) {
    // Check for Bus-Off (bit 7 of GSR), don't reset unless really needed
    if (LPC_CAN1->GSR & (1 << 7)) {
       // UART0_SendString("CAN1 Bus-Off! Attempting recovery...\r\n");

        // Enter reset mode
        LPC_CAN1->MOD = 1;
        while (!(LPC_CAN1->MOD & 1));

        // Clear error counters and status
      //  LPC_CAN1->GSR = 0;

        // Leave reset mode (normal operation)
        LPC_CAN1->MOD &= ~1;
        while (LPC_CAN1->MOD & 1);
		
       
		   LPC_CAN1->MOD |= (1 << 2);
        // DO NOT re-enable Self-Test mode here
    }
}


/*void CAN2_CheckBusStatus(void) {
    if (LPC_CAN2->GSR & (1 << 7)) {
        LPC_CAN2->MOD = 1;
        while (!(LPC_CAN2->MOD & 1));

        CAN_Delay(10);

        LPC_CAN2->MOD &= ~1;
        while (LPC_CAN2->MOD & 1);

        LPC_CAN2->MOD |= (1 << 2);
    }
}
*/

 
