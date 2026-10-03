/* Service Layer for Servo motor control */

#include "../inc/sweep_control.h"

/* Sweep Direction */
typedef enum
{
    DIR_forward,
    DIR_backward
}sweep_dir_t;

/* Servo control structure */
typedef struct
{
    uint8_t current_step;                     // Servo steps
    sweep_dir_t direction;                  // Sweep direcion
    uint8_t servo_pin;                    // Servo Pin
}sweep_control_t;

/* Initial values */
sweep_control_t sweep_control =
{
    .current_step = 5,
    .direction = DIR_forward,
    .servo_pin = 5
};

/* Sweep Update Function */
void sweep_update(void)
{
    switch (sweep_control.direction)
    {

    // Forward Sweep
    case DIR_forward:

        servo_move_step(sweep_control.servo_pin,sweep_control.current_step);

        if(sweep_control.current_step > 25)
        {
            sweep_control.direction = DIR_backward;
            sweep_control.current_step = 25;
        }
        break;

        // Backward Sweep
        case DIR_backward:

            servo_move_step(sweep_control.servo_pin,sweep_control.current_step);

            if(sweep_control.current_step < 5)
            {
                sweep_control.direction = DIR_forward;
                sweep_control.current_step = 5;
            }
            break;
    
    default:
        break;
    }
}

/* Function for returning current step */
uint8_t sweep_current_step(void)
{
    return sweep_control.current_step;
}

/* Function for getting next step */
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