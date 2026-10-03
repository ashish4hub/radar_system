#ifndef ICU_H
#define ICU_H

#include<stdint.h>

// Edge select
typedef enum {
    ICU_falling,
    ICU_rising
} ICU_edge_t;

// prescaler
typedef enum {
    ICU_ps_1 = 1,
    ICU_ps_8 = 2,
    ICU_ps_64 = 3,
    ICU_ps_256 = 4,
    ICU_ps_1024 = 5
} ICU_prescaler_t;

// Noise canceler
typedef enum {
    ICU_noise_off = 0,
    ICU_noise_on = 1
} ICU_noise_t;

// Configuration
typedef struct{
    ICU_edge_t edge;
    ICU_prescaler_t prescaler;
    ICU_noise_t noise;
} ICU_config_t;

/* Public APIs */
void ICU_init(const ICU_config_t *config);
void ICU_set_edge(ICU_edge_t edge);
uint8_t ICU_done(void);
uint16_t ICU_get_capture(void);
void ICU_clear(void);
void ICU_stop(void);

#endif