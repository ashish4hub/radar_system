#include "../inc/obstacle_detection.h"

static volatile uint16_t start_time;             // store pulse start time
static volatile uint16_t end_time;              // store pulse end time
static volatile uint16_t duration;             // Pulse duration
static volatile uint16_t distance;            // store calulated distance
static volatile uint8_t measurement_done;    // measuremnt done flag

/* State */
typedef enum{
    obstacle_wait_rising,
    obastacle_wait_falling
} obstacle_state_t;

obstacle_state_t obstacle_state;

/* Initialization */
void OBSTACLE_init(void){

    /* ICU configuration */
    ICU_config_t icu_config = {
        .edge = ICU_rising,
        .noise = ICU_noise_on,
        .prescaler = ICU_ps_8,
    };

    ICU_init(&icu_config);

    gpio_init(7,GPIO_OUTPUT);         // TRIG pin set as output

    obstacle_state = obstacle_wait_rising;        // Initial edge set to Risisng

    ICU_set_edge(ICU_rising);         // Internal ICU edge set to Risisng for pulse start time
}

/* Trigger pulse */
void OBSTACLE_trigger(void)
{
    gpio_write(7,GPIO_HIGH);
    _delay_us(10);
    gpio_write(7,GPIO_LOW);
    measurement_done = 0;                 // Clear Measurement completion flag on trigger
}

/* Obstacle Update */
void OBSTACLE_update(void)
{
    if(ICU_done())
    {
        switch (obstacle_state)
        {
        /* Record Start time when rising edge is detected */
        case obstacle_wait_rising:
            start_time = ICU_get_capture();
            ICU_clear();
            ICU_set_edge(ICU_falling);
            obstacle_state = obastacle_wait_falling;
            break;

        /* Record End time when falling edge is detected */
        case obastacle_wait_falling:
            end_time = ICU_get_capture();
            duration = end_time- start_time;
            distance = duration / 116;
            measurement_done = 1;                             // Set complition flag when measurement is done
            ICU_set_edge(ICU_rising);
            obstacle_state = obstacle_wait_rising;
            ICU_clear();
        
        default:
            break;
        }
    }
}

/* Measurement Completion Flag */
uint8_t OBSTACLE_measure_done(void){
    return measurement_done;
}

/* Return Distance/Measurement API */
uint16_t OBSTACLE_get_distance(void){
    return distance;
}

