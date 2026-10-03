/* Service Layer for Servo motor control */

#include "sweep_control.h"

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
    uint32_t last_update;                    // System clock timestamp for Last servo step update
    sweep_dir_t direction;                  // Sweep direcion
    uint16_t interval;                     // Sweep speed
    uint8_t servo_pin;                    // Servo Pin
}sweep_control_t;

/* Initial values */
sweep_control_t sweep_control =
{
    .current_step = 5,
    .direction = DIR_forward,
    .interval = 500,
    .servo_pin = 5
};

uint32_t Forward_step_wait = 0;               // Time reference for forward step update
uint32_t Backward_step_wait = 0;             // Time reference for backward step update

/* Sweep Update Function */
void sweep_update(void)
{
    switch (sweep_control.direction)
    {

    // Forward Sweep
    case DIR_forward:

        servo_move_step(sweep_control.servo_pin,sweep_control.current_step);

        if(nb_wait_ms(&Forward_step_wait,sweep_control.interval))
        {
            sweep_control.current_step++;
        }
        if(sweep_control.current_step > 25)
        {
            sweep_control.direction = DIR_backward;
            sweep_control.current_step = 25;
        }
        break;

        // Backward Sweep
        case DIR_backward:

            servo_move_step(sweep_control.servo_pin,sweep_control.current_step);

            if(nb_wait_ms(&Backward_step_wait,sweep_control.interval))
            {
                sweep_control.current_step--;
            }
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