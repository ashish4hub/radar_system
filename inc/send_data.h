#ifndef SEND_DATA_H
#define SEND_DATA_H

#include "uart.h"

/* Data Transmission State */
typedef enum
{
    DATA_idle,
    DATA_send,
    DATA_send_health
}DATA_tx_t;

void send_data(DATA_tx_t state);                  // Declaration

#endif