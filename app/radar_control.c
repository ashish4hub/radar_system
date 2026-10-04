#include "../inc/radar_control.h"

typedef enum
{
    RADAR_MOVE,
    RADAR_SETTLE,
    RADAR_MEASURE,
    RADAR_SEND
}RADAR_CONTROL_t;

RADAR_CONTROL_t RADAR_STATE = RADAR_MOVE;                       // RADAR operation state
uint32_t RADAR_settle_wait = 0;                   // Reference for RADAR settle time wait
uint32_t Trigger_wait = 0;                       // Reference for trigger wait time
uint8_t Current_sweep_step = 0;                      // Storing current servo step
uint16_t current_distance = 0;                      // Storing current measured distance

/* Radar initialization */
void radar_init(void)
{
    timer_init();
    OBSTACLE_init();
    servo_init();
    USART_init(9600);
    gpio_init(5,GPIO_OUTPUT);
}


/* Radar Controlling Function */
void radar_control(void)
{
    sweep_update();

    switch (RADAR_STATE)
    {

    case RADAR_MOVE:
        sweep_update();
        RADAR_STATE = RADAR_SETTLE;
        break;

    case RADAR_SETTLE:
        if(nb_wait_ms(&RADAR_settle_wait,5))
        {
            RADAR_STATE = RADAR_MEASURE;
        }
        break;
    
    case RADAR_MEASURE:
            if(nb_wait_ms(&Trigger_wait,60))
            {
                OBSTACLE_trigger();
            }
            OBSTACLE_update();
            if(OBSTACLE_measure_done())
            {
                current_distance = OBSTACLE_get_distance();
                RADAR_STATE = RADAR_SEND;
            }
            break;

    case RADAR_SEND:
        send_data(DATA_send);
        sweep_next_step();
        RADAR_STATE = RADAR_MOVE;
        break;
    
    default:
        break;
    }
}