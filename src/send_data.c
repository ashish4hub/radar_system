#include "../inc/send_data.h"
#include "../inc/obstacle_detection.h"
#include "../inc/sweep_control.h"
#include "../inc/uart.h"
#include "../inc/radar_config.h"

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

        case DATA_send_health:
            USART_print("RADAR HEALTH CHECK COMPLETED SUCCESSFULLY..\n");
            USART_print("Number of sweeps executed: ");
            USART_printIN(RADAR_HEALTH_CHECK_SWEEP);
            USART_print("\nRadar Status: Normal operational state\n");
            state = DATA_idle;
        break;

        default:
        break;
    }
}