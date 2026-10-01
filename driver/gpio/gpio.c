#include "gpio.h"

/* Register */
typedef struct 
{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *pin;
    uint8_t bit;
}gpio_reg_t;

/* GPIO Register Pin map */
static gpio_reg_t gpio_map[] =
{
    {&DDRD, &PORTD, &PIND, PD0},
    {&DDRD, &PORTD, &PIND, PD1},
    {&DDRD, &PORTD, &PIND, PD2},
    {&DDRD, &PORTD, &PIND, PD3},
    {&DDRD, &PORTD, &PIND, PD4},
    {&DDRD, &PORTD, &PIND, PD5},
    {&DDRD, &PORTD, &PIND, PD6},
    {&DDRD, &PORTD, &PIND, PD7},
    {&DDRB, &PORTB, &PINB, PB0},
    {&DDRB, &PORTB, &PINB, PB1},
    {&DDRB, &PORTB, &PINB, PB2},
    {&DDRB, &PORTB, &PINB, PB3},
    {&DDRB, &PORTB, &PINB, PB4},
    {&DDRB, &PORTB, &PINB, PB5}
};

/* GPIO Initialization function */
void gpio_init (uint8_t pin, GPIO_dir_t direction)
{
    const gpio_reg_t *gpio_ini = &gpio_map[pin];

    if(direction == GPIO_INPUT) 
    {
        *(gpio_ini->ddr) &= ~(1 << (gpio_ini->bit));
    }
    else
    {
        *(gpio_ini->ddr) |= (1 << (gpio_ini->bit));
    }
}

/* GPIO write function */
void gpio_write (uint8_t pin, GPIO_state_t state)
{
    const gpio_reg_t *gpio_wrt = &gpio_map[pin];

    if(state == GPIO_HIGH)
    {
        *(gpio_wrt->port) |= (1 << (gpio_wrt->bit));
    }
    else
    {
        *(gpio_wrt->port) &= ~(1 << (gpio_wrt->bit));
    }
}

/* GPIO Read function */
uint8_t gpio_read (uint8_t pin)
{
    const gpio_reg_t *gpio_r = &gpio_map[pin];

    if(*(gpio_r->pin) & (1 << gpio_r->bit)) return GPIO_HIGH;

    return GPIO_LOW;
}

