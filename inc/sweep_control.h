#ifndef SWEEP_CONTROL_H
#define SWEEP_CONTROL_H

#include "servo.h"
#include "timer.h"

void sweep_update(void);                     // Sweep Update function for servo
uint8_t sweep_current_step(void);           // Return current servo step
void sweep_next_step(void);                // Function for setting next step

#endif