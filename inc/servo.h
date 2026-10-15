#ifndef SERVO_H
#define SERVO_H

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "gpio.h"

/* Public APIs */

void servo_init(void);                                       // Initialize Servo Control Hardware
void servo_move_step(uint8_t pin, uint8_t step);            // Set PIN and STEP
#endif