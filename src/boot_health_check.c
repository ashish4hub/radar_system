/*
---------------------------BOOT HEALTH CHECK ROUTINE-------------------------------
* If RADAR_HEALTH_CHECK_ENABLED is enabled the system runs this routine on boot
* Runs the sweep at different speeds to verify movement
* Exits the HEALTH_CHECK mode if required number of sweeps are performed
*/

#include "../inc/boot_health_check.h"
#include "../inc/radar_config.h"
#include <stdint.h>
#include "../inc/sweep_control.h"
#include "../inc/timer.h"
#include "../inc/send_data.h"

/* Internal */
typedef struct {
    uint32_t TIME_ref;                       // Time reference
    uint16_t HEALTH_sweep_speed;             // Speed of sweep
    bool HEALTH_check_done;                // FLAG
    uint8_t SWEEPS_required;              // Number of sweeps to perform during health check
    uint8_t SWEEP_ref;
    uint8_t SWEEPS_performed;            // Number of sweeps performed during health check
}BOOT_HEALTH_check_t;

/* Internal value initializations */
BOOT_HEALTH_check_t HEALTH_check = {

    .SWEEPS_required = RADAR_HEALTH_CHECK_SWEEP,
    .HEALTH_check_done = false,
    .TIME_ref = 0,
    .HEALTH_sweep_speed = 400,
    .SWEEPS_performed = 0,
    .SWEEP_ref = 1
};

/* HEALTH_CHECK Routine states */
typedef enum {
    HEALTH_CHECK_IDLE,
    HEALTH_CHECK_IDLE_WAIT,
    HEALTH_CHECK_START,
    HEALTH_CHECK_STEP_WAIT,
    HEALTH_CHECK_SWEEP_COUNT,
    //HEALTH_CHECK_REPEAT
} HEALTH_check_state_t;

static HEALTH_check_state_t HEALTH_check_state = HEALTH_CHECK_IDLE;         // Initial state


/*
---------------------------------HEALTH_CHECK ROUTINE------------------------------------
* Built to ensure if the RADAR is making sweeps as expected or not
* Test the RADAR movement and sweep at different speed levels
* This is a SOFTWARE only CHECK routine cannot verify if the RADAR physically moved or not
*/
void RUN_HEALTH_CHECK(void)
{
    if(HEALTH_check.HEALTH_check_done == false)
    {
        switch (HEALTH_check_state)
        {
            /* Position Radar at 0 degree */
            case HEALTH_CHECK_IDLE:
                sweep_initial_position();
                HEALTH_check.TIME_ref = get_ticks();
                HEALTH_check_state = HEALTH_CHECK_IDLE_WAIT;
            break;

            /* Wait for Radar to settle at IDLE position */
            case HEALTH_CHECK_IDLE_WAIT:
                if(nb_wait_ms(&HEALTH_check.TIME_ref,HEALTH_check.HEALTH_sweep_speed))
                {
                    HEALTH_check_state = HEALTH_CHECK_START;
                }
            break;

            /* Start HEALTH_CHECK Routine */
            case HEALTH_CHECK_START:
                sweep_update();
                HEALTH_check.TIME_ref = get_ticks();
                HEALTH_check_state = HEALTH_CHECK_STEP_WAIT;
            break;

            /* Wait before advancing the step */
            case HEALTH_CHECK_STEP_WAIT:
                if(nb_wait_ms(&HEALTH_check.TIME_ref,HEALTH_check.HEALTH_sweep_speed))
                {
                    sweep_next_step();
                    HEALTH_check_state = HEALTH_CHECK_SWEEP_COUNT;
                }
            break;

            /*
            * Count the number of sweeps performed
            * Exit HEALTH_CHECK mode if required sweeps are performed
            * Set the HEALTH_CHECK completion flag
            */
            case HEALTH_CHECK_SWEEP_COUNT:
            
                if(sweep_executed() == HEALTH_check.SWEEP_ref)
                {
                    HEALTH_check.SWEEPS_performed++;
                    HEALTH_check.SWEEP_ref++;
                    HEALTH_check.HEALTH_sweep_speed /= 2;

                    /* Break if required number of sweeps performed */
                    if(HEALTH_check.SWEEPS_performed >= RADAR_HEALTH_CHECK_SWEEP)
                    {
                        HEALTH_check.HEALTH_check_done = true;
                        break;
                    }
                }
                HEALTH_check_state = HEALTH_CHECK_START;
            break;
        }
    }
}

/* Return HEAL_CHECK status */
bool HEALTH_CHECK_STATUS(void)
{
    return HEALTH_check.HEALTH_check_done;
}