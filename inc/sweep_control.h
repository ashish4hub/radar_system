#ifndef SWEEP_CONTROL_H
#define SWEEP_CONTROL_H

#include "servo.h"
#include "timer.h"

void sweep_update(void);                            // Sweep Update function for servo
uint8_t sweep_current_step(void);                  // Return current servo step
void sweep_next_step(void);                       // Function for setting next step
void sweep_set_pin(uint8_t pin);                 // Set servo pin for sweep
void sweep_idle(void);                          // Position Radar at initial point

#endif