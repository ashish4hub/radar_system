#include "../inc/send_data.h"

uint16_t current_distnace = 0;
uint8_t current_step = 0;

/* Data transmission state machine */
void send_data(DATA_tx_t state)
{
    switch (state)
    {
    case DATA_idle:
        break;

    case DATA_send:
        USART_printIN(sweep_current_step());
        USART_print(",");
        USART_printIN(OBSTACLE_get_distance());
        USART_print("\n");
        state = DATA_idle;
        break;

    default:
        break;
    }
}