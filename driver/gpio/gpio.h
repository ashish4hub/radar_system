#ifndef GPIO_H
#define GPIO_H

#include <stdlib.h>
#include <avr/io.h>

/* Pin Direction */
typedef enum 
{
    GPIO_INPUT,
    GPIO_OUTPUT
}GPIO_dir_t;

/* Pin state */
typedef enum
{
    GPIO_LOW,
    GPIO_HIGH
}GPIO_state_t;

/* API */
void gpio_init(uint8_t pin, GPIO_dir_t direction);            // Initialize GPIO PIN in required direction or mode (I/O)
void gpio_write (uint8_t pin, GPIO_state_t state);           // Set PIN HIGH / LOW
uint8_t gpio_read (uint8_t pin);                               // Read PIN


#endif