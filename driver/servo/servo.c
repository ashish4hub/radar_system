/* TIMER2 Configured in CTC mode */

/*
.
.
.
.
.
.
.
.
.
.
.
*/

#include "servo.h"

#define MIN_TICK 5
#define MAX_TICK 25

#define CENTER (MIN_TICK + MAX_TICK) / 2            // 90 degree

/* Servo states */
typedef enum
{
    SERVO_low,
    SERVO_high
}SERVO_state_t;

volatile uint8_t tick_100us = 0;                 // Store pulse
static volatile SERVO_state_t SERVO_state;      // State for controlling servo state

/* Timer2 initialization fucntion for servo */
void servo_init()
{
    TCCR2A |= (1 << WGM21);      // CTC Mode
    TCCR2B |= (1 <<CS21);       // Prescaler 8
    
    TIMSK2 |= (1 << OCIE2A);        // Output Compare A Interrupt Enable

    TCNT2 = 0;              // Initial count start from 0
    
    OCR2A = 199;
}

/* Interrupt function */
ISR(TIMER2_COMPA_vect)
{
    tick_100us++;
    if(tick_100us == 200)
    {
        tick_100us = 0;
    }
}

/* Angle to Tick/Pulse conversion */
static uint8_t servo_ang_cnv (uint8_t angle)
{
    if(angle == 0)              // 0 degree
    {
        return 5;
    }
    else if(angle == 45)        // 45 degree
    {
        return 10;
    }
    else if(angle == 90)        // 90 degree
    {
        return 15;
    }
    else if(angle == 135)       // 135 degree
    {
        return 20;
    }
    else if(angle == 180)       // 180 degree
    {
        return 25;
    }
    return 0;
}

/* Servo Update */
static void servo_update(uint8_t pulse)
{
    if(tick_100us == 0)
    {
        SERVO_state = SERVO_high;
    }
    else if (tick_100us == pulse)
    {
        SERVO_state = SERVO_low;
    }
    
}

/* Servo step move API (5 to 25) */
void servo_move_step(uint8_t pin, uint8_t step)
{
    servo_update(step);

    if(SERVO_state == SERVO_high){
        gpio_write(pin,GPIO_HIGH);
    }
    else if(SERVO_state == SERVO_low)
    {
        gpio_write(pin,GPIO_LOW);
    }
}

/* Servo angle postion API (0 to 180 ) */
void servo_move_angle(uint8_t pin, uint8_t angle)
{
    servo_update(servo_ang_cnv(angle));

    if(SERVO_state == SERVO_high)
    {
        gpio_write(pin,GPIO_HIGH);
    }
    else if(SERVO_state == SERVO_low)
    {
        gpio_write(pin,GPIO_LOW);
    }
}