

#include "LPC17xx.h"
#include <stdint.h>

typedef struct {
    uint32_t id;
    uint8_t len;
    uint8_t data[8];
} CAN_Message_t;

void CAN1_Init(void);
void CAN2_Init(void);
void CAN1_cfgBaudrate(uint32_t can_mod_id, uint32_t baudrate);
int CAN1_Send(uint32_t id, uint8_t *data, uint8_t len);
int CAN2_Send(uint32_t id, uint8_t *data, uint8_t len);
void CAN_Filter_Bypass(void);
void CAN_Delay(uint32_t ms);


