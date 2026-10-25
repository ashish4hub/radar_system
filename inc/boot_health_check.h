#ifndef BOOT_HEALTH_CHECK
#define BOOT_HEALTH_CHECK

#define RADAR_HEALTH_CHECK_ENABLED 1
#define RADAR_HEALTH_CHECK_SWEEP 4

#include <stdint.h>
#include <stdbool.h>
#include "sweep_control.h"
#include "timer.h"
#include "send_data.h"

void RUN_HEALTH_CHECK(void);

#endif