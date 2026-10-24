/* Servo Driver for ATmega328P */

/*
-------------------------------Hardware Configuration & Calculation----------------------

. Requested Pulse duration is scheduled by timer
. The ISR internally use gpio driver for pulse generation
. Timer2 configured in CTC mode with prescaler 8
. 16 MHz / 8 = 2 MHz
. [us = MicroSecond, ms = MilliSecond]
. (1 / 2 MHz) * [Top Compare Value = 200] = 100 us
. Timer2 Interrupt fire every 100 us/0.1ms
. 1 tick_100us = 100 us or 0.1 ms

*/

/*
------------------------------------------API Usage-----------------------------------------

. servo_init(); : Initialize Timer2 with the required configuration [CTC Mode, Presacler, Output Compare Interrupt, Top value].
. servo_move_step(); : Set servo pin (any digital I/O Pin) and Step [1 step == 100 microseconds].

*/

#include "../inc/servo.h"

static volatile uint8_t tick_100us = 0;                 // Store pulse

typedef struct
{
    uint8_t servo_pin;
    uint8_t servo_pulse_duration;
} SERVO_t;

static volatile SERVO_t SERVO;

/* Timer2 initialization fucntion for servo */
void servo_init()
{
    TCCR2A |= (1 << WGM21);             // CTC Mode
    TCCR2B |= (1 <<CS21);              // Prescaler 8
    
    TIMSK2 |= (1 << OCIE2A);        // Output Compare A Interrupt Enable

    TCNT2 = 0;                    // Initial count start from 0

    OCR2A = 199;                // Output Compare TOP value
}

/* Servo Step Move */
void servo_move_step(uint8_t pin, uint8_t step)
{
    SERVO.servo_pin = pin;                              // Set servo pin
    SERVO.servo_pulse_duration = step;                 // Set Pulse Duration
}

/* Interrupt function */
ISR(TIMER2_COMPA_vect)
{
    if(tick_100us == 0)
    {
        gpio_write(SERVO.servo_pin, GPIO_HIGH);
    }
    else if(tick_100us == SERVO.servo_pulse_duration)
    {
        gpio_write(SERVO.servo_pin, GPIO_LOW);
    }
    tick_100us++;

    if(tick_100us == 200)
    {
        tick_100us = 0;
    }
}