/* UART driver */
/* Hardware abstraction layer for serial communicattion using USART */
/* MCU = ATmega328P */

/*--------------------APIs--------------------*/
/*
* USART_init() ----> Initialize USART with desired BAUD
* USART_print() ----> Print string
* USART_printIN() ----> Print integer
* USART_read_line() ----> Builds string from received characters and stores in 'line' array -> Returns 1 when ready for reading
* line ----> Character array where string is stored
*/


#define F_CPU 16000000UL

#include "uart.h"

volatile char RX_buffer[Rx_buffer_size];
volatile uint8_t RX_head = 0;
volatile uint8_t RX_tail = 0;

volatile uint8_t Tx_buffer[Tx_buffer_size];
volatile uint8_t TX_tail = 0;
volatile uint8_t TX_head = 0;


/* USART initialization */
void USART_init(uint32_t BAUD){
  uint16_t BRC = ((F_CPU / (16UL * BAUD)) - 1UL);
  UBRR0H = (BRC >> 8);
  UBRR0L = BRC;
  UCSR0B = (1 << TXEN0) | (1 << RXCIE0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


/*-------------------------------TX-------------------------------*/

/* 
* Store characters in TX buffer 
* Enable interrupt for transmission
* If interrupt is off kickstart transmission manually  
*/
void USART_TX_byte(uint8_t data){

    uint8_t next_head;

    while(1){
        next_head = (TX_head + 1) % Tx_buffer_size;
        if(next_head != TX_tail){
            break;
        }
    }

    Tx_buffer[TX_head] = data;
    TX_head = next_head;

    // If interrupt is OFF -> UART is idle -> kickstart manually
    if(!(UCSR0B & (1 << UDRIE0))){
        UDR0 = Tx_buffer[TX_tail];
        TX_tail = (TX_tail + 1) % Tx_buffer_size;
    }

    UCSR0B |= (1 << UDRIE0);
}

/* String print function */
void USART_print(const char *str){
  while(*str){
    USART_TX_byte(*str++);;
  }
}

/* USART integer print function */
void USART_printIN(int num){
  char buffer[10];

  int i = 0;
  
  // Handling 0
  if(num == 0){
    USART_TX_byte('0');
    return;
  }

  // Handling negative
  if(num < 0){
    USART_TX_byte('-');
    num = -num;
  }

  // Converting digits to string (in reverse order) 
  while(num > 0){
    buffer[i++] = (num % 10) + '0';
    num /= 10;
  }

  // Printing in correct order
  while(i--){
    USART_TX_byte(buffer[i]);
  }
}

/*
* Disable interrupt if buffer is empty and return
* If buffer has characters, starts transmission from TX buffer 
*/
ISR(USART_UDRE_vect){

  if(TX_head == TX_tail){
    UCSR0B &= ~(1 << UDRIE0);
    return;
  }
  UDR0 = Tx_buffer[TX_tail];
  TX_tail = (TX_tail + 1) % Tx_buffer_size;
}


/*---------------------------------------RX---------------------------------------*/

/* RX interrupt function */
ISR(USART_RX_vect){
  char data = UDR0; // Read received byte in data 
  uint8_t next = (RX_head + 1) % Rx_buffer_size;
  
  //Drop byte is buffer is full
  if(next != RX_tail){
    RX_buffer[RX_head] = data;
    RX_head = next;
  }
}

/* If data is availabe for reading */
uint8_t USART_rx_avail(void){
  return (RX_head != RX_tail);
}

/* Get RX data */
char USART_get_data(void){
  if(RX_head == RX_tail) return 0;
  char data = RX_buffer[RX_tail];
  RX_tail = (RX_tail + 1) % Rx_buffer_size;
  return data;
}
