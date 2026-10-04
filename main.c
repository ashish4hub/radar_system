#define F_CPU 16000000UL

#include "inc/radar_control.h"

int main(void)
{
    radar_init();
    sei();

    while(1)
    {
        radar_control();
    }
}