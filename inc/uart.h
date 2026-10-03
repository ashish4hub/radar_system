#ifndef UART_H
#define UART_H

#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdint.h>

#define Tx_buffer_size 64
#define Rx_buffer_size 64
#define line_size 32

 void USART_init(uint32_t BAUD);                 /* Initialize USART with desired baud rate */
 void USART_print(const char *str);             /* Print character or string */
 void USART_printIN(int num);                  /* Print integers */
 uint8_t USART_rx_avail(void);                /* Data available for reading */
 char USART_get_data(void);                  /* Returns received data */
 void USART_TX_byte(uint8_t data);          /* */

 #endif