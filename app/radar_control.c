/*
Application Layer
*/

#include "../inc/radar_control.h"

/* States */
typedef enum
{
    RADAR_IDLE,
    RADAR_IDLE_WAIT,
    RADAR_MOVE,
    RADAR_SETTLE,
    RADAR_TRIGGER,
    RADAR_WAIT_ECHO,
    RADAR_SEND,
    RADAR_UPDATE
}RADAR_STATE_t;

RADAR_STATE_t RADAR_STATE = RADAR_IDLE;          // Initial RADAR operation state

/* Radar Control internal */
typedef struct
{
    uint32_t RADAR_wait_Ref;                             // Time Reference for waiting
    uint16_t RADAR_settle_time;                         // Time for Radar settlement
    uint8_t RADAR_echo_timeout;                        // Echo wait time period
    uint8_t SERVO_pin;                                // Servo Pin
    uint8_t SERVO_initial_step;                      // Initial step 0 degree
    uint8_t SERVO_final_step;                       // Final step 180 degree
}RADAR_t;

/* Internal */
RADAR_t RADAR_control = {
    .RADAR_wait_Ref = 0,
    .RADAR_settle_time = 50,                  // Settle time for RADAR / Sweep speed [Can be changed as desired]
    .RADAR_echo_timeout = 35,                 // Echo wait time period
    .SERVO_pin = 5,                          // Servo Pin [Can be changed to any GPIO Pin] [Check gpio driver or documentation for pin map]
    .SERVO_initial_step = 5,                // Servo initial 0 degree position [Change according to specific servo calibration 1 step == 0.1ms pulse]
    .SERVO_final_step = 25                 // Servo final 180 degree position [Change according to specific servo calibration 1 step == 0.1ms pulse]
};

/* Radar initialization
. Initialize Required peripherals
*/
void radar_init(void)
{
    timer_init();
    OBSTACLE_init();
    servo_init();
    USART_init(9600);
    gpio_init(RADAR_control.SERVO_pin,GPIO_OUTPUT);
    sweep_set_internal(RADAR_control.SERVO_pin, RADAR_control.SERVO_initial_step, RADAR_control.SERVO_final_step);
}

/* State-based Radar Control Function */
void radar_control(void)
{
    switch (RADAR_STATE)
    {
        /* Idle Radar state -> Position Radar at Initial position 0 degree */
        case RADAR_IDLE:
        sweep_initial_position();
        RADAR_control.RADAR_wait_Ref = get_ticks();
        RADAR_STATE = RADAR_IDLE_WAIT;
        break;

        /* Wait state for Radar to reach initial position 0 degree */
        case RADAR_IDLE_WAIT:
        if(nb_wait_ms(&RADAR_control.RADAR_wait_Ref,RADAR_control.RADAR_settle_time))
        {
            RADAR_STATE = RADAR_MOVE;
        }
        break;

        /* Update step and Move Servo */
        case RADAR_MOVE:
            sweep_update();
            RADAR_control.RADAR_wait_Ref = get_ticks();    // Record time for settle after movement started
            RADAR_STATE = RADAR_SETTLE;
        break;
        
        /* Wait for the Radar to settle at required position */
        case RADAR_SETTLE:
            if(nb_wait_ms(&RADAR_control.RADAR_wait_Ref,RADAR_control.RADAR_settle_time))
            {
                RADAR_STATE = RADAR_TRIGGER;
            }
        break;
    
        /* Trigger Ultrasonic sensor once */
        case RADAR_TRIGGER:
            OBSTACLE_trigger();
            RADAR_control.RADAR_wait_Ref = get_ticks();    // Record time for echo timeout when triggered
            RADAR_STATE = RADAR_WAIT_ECHO;
        break;

        /* Wait for Echo */
        case RADAR_WAIT_ECHO:
            OBSTACLE_update();
            if(OBSTACLE_measure_done())
            {
                RADAR_STATE = RADAR_SEND;
            }
            /* Abort measurement and advance sweep if echo is not detected whithin the timeout period */
            else
            {
                if(nb_wait_ms(&RADAR_control.RADAR_wait_Ref,RADAR_control.RADAR_echo_timeout))
                {
                    OBSTACLE_detection_abort();       // Abort measurement/detection if timeout
                    RADAR_STATE = RADAR_UPDATE;
                }
            }
        break;

        /* Send step and measurement data over serial */
        case RADAR_SEND:
            send_data(DATA_send);
            RADAR_STATE = RADAR_UPDATE;
        break;

        /* Update step value */
        case RADAR_UPDATE:
            sweep_next_step();
            RADAR_STATE = RADAR_MOVE;
        break;

        default:
        break;
    }
}