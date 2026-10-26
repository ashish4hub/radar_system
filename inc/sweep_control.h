#ifndef SWEEP_CONTROL_H
#define SWEEP_CONTROL_H

#include <stdint.h>
#include "servo.h"

void sweep_update(void);                                                                                   // Sweep Update function for servo
uint8_t sweep_current_step(void);                                                                         // Return current servo step
void sweep_next_step(void);                                                                              // Function for setting next step
void sweep_set_internal(uint8_t pin, uint8_t servo_initial_step, uint8_t servo_final_step);             // Set servo pin and starting step value for sweep
void sweep_initial_position(void);                                                                     // Move Radar to initial position 0 degree
float sweep_executed(void);                                                                           // Return number of sweeps executed

#endif