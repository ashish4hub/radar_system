#ifndef BOOT_HEALTH_CHECK_H
#define BOOT_HEALTH_CHECK_H

#include <stdbool.h>

void RUN_HEALTH_CHECK(void);                          // State-machine based health_check functrion
bool HEALTH_CHECK_STATUS(void);                      // Returns current health check status (true/false)

#endif