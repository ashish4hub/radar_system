/* Input Capture Unit (ICU) Driver */

#define F_CPU 16000000UL

#include "icu.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint16_t icu_capture = 0;
static volatile uint8_t icu_done = 0; 

/* ICU initialization */
void ICU_init(const ICU_config_t *config){

    // Normal mode
    TCCR1A |= 0;
    TCCR1B |=0;
    TIMSK1 |= 0;

    // Configure Edge
    TCCR1B |= (config->edge << ICES1);

    // Configure Noise Canceler
    TCCR1B |= (config->noise << ICNC1);

    // Configure prescaler
    TCCR1B |= config->prescaler;

    // Clearing old flags
    TIFR1 |= (1 << ICF1);

    // Enable interrupt
    TIMSK1 |= (1 << ICIE1);
}

/* ISR */
ISR(TIMER1_CAPT_vect){
    icu_capture = ICR1;
    icu_done = 1;
}

/* Status API */
uint8_t ICU_done(void){
    return icu_done;
}

/* Read capture value */
uint16_t ICU_get_capture(void){
    return icu_capture;
}

/* Clear flag API */
void ICU_clear(void){
    icu_done = 0;
    TIFR1 |= (1 <<ICF1);
}

/* Edge Changing API */
void ICU_set_edge(ICU_edge_t edge){

    if(edge == ICU_rising){
        TCCR1B |= (1 << ICES1);
    }
    else {
        TCCR1B &= ~(1 << ICES1);
    }
}

/* ICU stop function */
void ICU_stop(void){
    TCCR1B &= ~((1 << CS12) |
               (1 << CS11) |
               (1 << CS10)) ;
    
    TIMSK1 &= ~(1 << ICIE1);
    icu_done = 0;
}