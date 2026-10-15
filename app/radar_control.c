#include "../inc/radar_control.h"

typedef enum
{
    RADAR_MOVE,
    RADAR_SETTLE,
    RADAR_MEASURE,
    RADAR_SEND,
    RADAR_UPDATE
}RADAR_STATE_t;

RADAR_STATE_t RADAR_STATE = RADAR_MOVE;          // RADAR operation state

/* Radar Control internal */
typedef struct
{
    uint32_t RADAR_settle_wait_Ref;                   // Reference for RADAR settle time wait
    uint32_t RADAR_settle_time;                      // Time for Radar settlement
    uint8_t SERVO_pin;                              // Servo Pin
}RADAR_t;

RADAR_t RADAR_control = {
    .RADAR_settle_wait_Ref = 0,             // Timer reference for settlement wait
    .RADAR_settle_time = 100,              // Settle time for RADAR / Sweep speed [Can be changed as desired]
    .SERVO_pin = 5                        // Servo Pin [Can be changed to any GPIO Pin]

};

/* Radar initialization */
void radar_init(void)
{
    timer_init();
    OBSTACLE_init();
    servo_init();
    USART_init(9600);
    gpio_init(RADAR_control.SERVO_pin,GPIO_OUTPUT);
    sweep_set_pin(RADAR_control.SERVO_pin);
}

/* Radar Controlling Function */
void radar_control(void)
{

    switch (RADAR_STATE)
    {

        /* Upadte step and Move Servo */
        case RADAR_MOVE:
            sweep_update();
            RADAR_STATE = RADAR_SETTLE;
        break;
        
        /* Wait for the Radar to settle at required position */
        case RADAR_SETTLE:
            if(nb_wait_ms(&RADAR_control.RADAR_settle_wait_Ref,RADAR_control.RADAR_settle_time))
            {
                RADAR_STATE = RADAR_MEASURE;
            }
        break;
    
        /* Trigger Ultrasonic sensor once */
        case RADAR_MEASURE:
            OBSTACLE_trigger();
            OBSTACLE_update();
            if(OBSTACLE_measure_done())
            {
                RADAR_STATE = RADAR_SEND;
            }
        
        break;
    
        /* Send step and measurement data over serial */
        case RADAR_SEND:
            send_data(DATA_send);
            RADAR_STATE = RADAR_UPDATE;
        break;
    
        /* Upadte step value */
        case RADAR_UPDATE:
            sweep_next_step();
            RADAR_STATE = RADAR_MOVE;
        break;

        default:
        break;
    }
}