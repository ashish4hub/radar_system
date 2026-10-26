#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_init(void);
uint32_t get_ticks (void);
uint8_t nb_wait_ms(uint32_t *prv_time ,uint32_t time);

 #endif
