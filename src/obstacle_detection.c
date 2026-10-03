#include "../inc/obstacle_detection.h"

static volatile uint16_t start_time;             // store pulse start time
static volatile uint16_t end_time;              // store pulse end time
static volatile uint16_t duration;             // Pulse duration
static volatile uint16_t distance;            // store calulated distance
static volatile uint8_t measurement_done;    // measuremnt done flag
static uint32_t trig_wait = 0;              // Trigger wait 

/* State */
typedef enum{
    HCSR04_wait_rising,
    HCSR04_wait_falling
} HCSR04_state_t;

HCSR04_state_t state;

/* HCSR04 initialization */
void OBSTACLE_init(void){

    /* ICU configuration */
    ICU_config_t icu_config = {
        .edge = ICU_rising,
        .noise = ICU_noise_on,
        .prescaler = ICU_ps_8,
    };

    ICU_init(&icu_config);


    gpio_init(7,GPIO_OUTPUT);         // TRIG pin set as output
    state = HCSR04_wait_rising;
    ICU_set_edge(ICU_rising);
}

/* Trigger pulse */
void OBSTACLE_trigger(void){

    gpio_write(7,GPIO_HIGH);
    _delay_us(10);
    gpio_write(7,GPIO_LOW);
}

/* Distance update */
void OBSTACLE_update(void){
    if(ICU_done()){

        if(state == HCSR04_wait_rising){
            start_time = ICU_get_capture();
            ICU_clear();
            ICU_set_edge(ICU_falling);
            state = HCSR04_wait_falling;
        }
        else {
            end_time = ICU_get_capture();
            duration = end_time- start_time;
            distance = duration / 116;
            measurement_done = 1;
            ICU_set_edge(ICU_rising);
            state = HCSR04_wait_rising;
        }
        ICU_clear();
    }
}

/* Done API */
uint8_t OBSTACLE_measure_done(void){
    return measurement_done;
}

/* Distance API */
uint16_t OBSTACLE_get_distance(void){
    measurement_done = 0;
    return distance;
}

