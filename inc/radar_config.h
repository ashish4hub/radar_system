#ifndef RADAR_CONFIG_H
#define RADAR_CONFIG_H

/*
* Radar health check is enabled by defining RADAR_HEALTH_CHECK_ENABLED 1
* Disable by writing it 0
*/
#define RADAR_HEALTH_CHECK_ENABLED 1

/* 
* Number sweeps to perform during the health check
* 8 / 2 = 4 complete 0 to 180 and 180 to 0 degree sweeps
* Current health check runs 4 complete sweeps during health check at different speed levels
*/
#define RADAR_HEALTH_CHECK_SWEEP 8

#endif