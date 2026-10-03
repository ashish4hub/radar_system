#ifndef OBSTACLE_DETECTION_H
#define OBSTACLE_DETECTION_H

#include "timer.h"
#include "gpio.h"
#include "icu.h"
#include "util/delay.h"

void OBSTACLE_init(void);                          // Initialize obstacle detection hardware
void OBSTACLE_trigger(void);                      // Trigger pulse for distance detection
void OBSTACLE_update(void);                      // Upadte Function for distance detection
uint8_t OBSTACLE_measure_done(void);            // Function for returning measurement status
uint16_t OBSTACLE_get_distance(void);          // Function for returning measured distance

#endif