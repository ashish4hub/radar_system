/*
---------------------------BOOT HEALTH CHECK ROUTINE-------------------------------
* If RADAR_HEALTH_CHECK_ENABLED is enabled the this routine runs
* Runs the sweep at different speeds to ensure movement
*/

#include "../inc/boot_health_check.h"
#include "../inc/radar_config.h"
#include <stdint.h>
#include "../inc/sweep_control.h"
#include "../inc/timer.h"
#include "../inc/send_data.h"


typedef struct {
    uint32_t TIME_ref;                       // Time reference
    uint16_t HEALTH_sweep_speed;             // Speed of sweep
    bool HEALTH_check_done;                // FLAG
    uint8_t SWEEPS_required;              // Number of sweeps to perform during health check
    uint8_t SWEEPS_performed;            // Number of sweeps performed during health check
}BOOT_HEALTH_check_t;

BOOT_HEALTH_check_t HEALTH_check = {

    .SWEEPS_required = RADAR_HEALTH_CHECK_SWEEP,
    .HEALTH_check_done = false,
    .TIME_ref = 0,
    .HEALTH_sweep_speed = 400,
    .SWEEPS_performed = 0
};

typedef enum {
    HEALTH_CHECK_IDLE,
    HEALTH_CHECK_IDLE_WAIT,
    HEALTH_CHECK_START,
    HEALTH_CHECK_STEP_WAIT,
    HEALTH_CHECK_SWEEP_COUNT,
    //HEALTH_CHECK_REPEAT
} HEALTH_check_state_t;

HEALTH_check_state_t HEALTH_check_state = HEALTH_CHECK_IDLE;

void RUN_HEALTH_CHECK(void)
{
    if(HEALTH_check.HEALTH_check_done == false)
    {
        switch (HEALTH_check_state)
        {
            case HEALTH_CHECK_IDLE:
                sweep_initial_position();
                HEALTH_check.TIME_ref = get_ticks();
                HEALTH_check_state = HEALTH_CHECK_IDLE_WAIT;
            break;

            case HEALTH_CHECK_IDLE_WAIT:
                if(nb_wait_ms(&HEALTH_check.TIME_ref,HEALTH_check.HEALTH_sweep_speed))
                {
                    HEALTH_check_state = HEALTH_CHECK_START;
                }
            break;

            case HEALTH_CHECK_START:
                sweep_update();
                HEALTH_check.TIME_ref = get_ticks();
                HEALTH_check_state = HEALTH_CHECK_STEP_WAIT;
            break;
            
            case HEALTH_CHECK_STEP_WAIT:
                if(nb_wait_ms(&HEALTH_check.TIME_ref,HEALTH_check.HEALTH_sweep_speed))
                {
                    sweep_next_step();
                    HEALTH_check.HEALTH_sweep_speed /= 2;
                    HEALTH_check_state = HEALTH_CHECK_SWEEP_COUNT;
                }
            break;

            case HEALTH_CHECK_SWEEP_COUNT:
                if(sweep_current_step() == 25)
                {
                    HEALTH_check.SWEEPS_performed++;
                    HEALTH_check_state = HEALTH_CHECK_START;
                }
            break;
        }
    }
    else if (HEALTH_check.SWEEPS_performed >= RADAR_HEALTH_CHECK_SWEEP)
    {
        HEALTH_check.HEALTH_check_done = true;
    }
}

bool HEALTH_CHECK_STATUS(void)
{
    return HEALTH_check.HEALTH_check_done;
}