/* TIMER Driver for generating non blocking delays */
/* TIMER0 configured in CTC Mode */
/* MCU = ATmega328P */

/*
---------------APIs---------------
* timer_init() ----> Timer initialization
* nb_wait_ms() ----> API for generating non blocking delay
* get_ticks() ----> Get current ticks (time) for reference in nb_wait_ms()
*/

#define F_CPU 16000000UL

#include "../inc/timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t tick = 0;

/* Timer0 for 1ms interrupt */
void timer_init(void){
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS00) | (1 << CS01);    //Prescaler 64
    OCR0A = 249;       // 1ms (16000000 / 64 / 1000 - 1)
    TIMSK0 = (1 << OCIE0A);    // Enable compare interrupt
}

/* ISR increament global tick */
ISR(TIMER0_COMPA_vect){
    tick++;
}

/* Return current ticks (Time in ms) */
uint32_t get_ticks(void){
    uint32_t t;
    uint8_t sreg = SREG;      // Save global interrupt state
    cli();
    t = tick;
    SREG = sreg;            // Restore global interrupt state
    return t;
}

/* Update current time in reference variable */
uint8_t nb_wait_ms(uint32_t *prv_time,uint32_t time){
    uint32_t now = get_ticks();
    if(now - *prv_time >=  time){
        *prv_time = now;      // Update current time
        return 1;            // Time reached
    }
    return 0;              // Time not reached (still waiting)
}