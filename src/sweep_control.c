/* Service Layer for Servo motor control */

#include "../inc/sweep_control.h"
#include "servo.h"
#include "timer.h"

/* Sweep Direction */
typedef enum
{
    DIR_forward,
    DIR_backward,
}sweep_dir_t;

/* Servo Control Structure */
typedef struct
{
    uint8_t current_step;                      // Servo step [Initialized with initial step]
    uint8_t initial_step;
    uint8_t final_step;                       // Final step
    sweep_dir_t direction;                   // Sweep direcion
    uint8_t servo_pin;                      // Servo Pin
}sweep_control_t;

/* Initial Value */
sweep_control_t sweep_control =
{
    .direction = DIR_forward,
};

/* Sweep Update Function */
void sweep_update(void)
{
    switch (sweep_control.direction)
    {
        // Forward Sweep
        case DIR_forward:

            servo_move_step(sweep_control.servo_pin,sweep_control.current_step);

            if(sweep_control.current_step >= sweep_control.final_step)
            {
                sweep_control.direction = DIR_backward;
                sweep_control.current_step = sweep_control.final_step;
            }
        break;

        // Backward Sweep
        case DIR_backward:

            servo_move_step(sweep_control.servo_pin,sweep_control.current_step);

            if(sweep_control.current_step <= sweep_control.initial_step)
            {
                sweep_control.direction = DIR_forward;
                sweep_control.current_step = sweep_control.initial_step;
            }
        break;

        default:
        break;
    }
}

/* Sweep initial position */
void sweep_initial_position(void)
{
    servo_move_step(sweep_control.servo_pin, sweep_control.initial_step);
}

/* Function for returning current step */
uint8_t sweep_current_step(void)
{
    return sweep_control.current_step;
}

/* Function for updating next step */
void sweep_next_step(void)
{
    if(sweep_control.direction == DIR_forward)
    {
        sweep_control.current_step++;
    }
    else if(sweep_control.direction == DIR_backward)
    {
        sweep_control.current_step--;
    }
}

/* Servo pin for Sweep service */
void sweep_set_internal(uint8_t servo_pin, uint8_t servo_initial_step, uint8_t servo_final_step)
{
    sweep_control.servo_pin = servo_pin;                     // Set servo pin
    sweep_control.initial_step = servo_initial_step;        // Set initial servo step
    sweep_control.current_step = servo_initial_step;       // Current step == initial step
    sweep_control.final_step = servo_final_step;          // Set final step
}