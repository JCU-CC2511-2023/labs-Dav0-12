/**************************************************************
 * main.c
 * rev 1.0 20-Sep-2023 dghun
 * Lab9
 * ***********************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

//UART DEFINITIONS
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define BACKSPACE '\177'
#define ENTER 13
#define RESET '\f'

//LED DEFINITIONS
#define RED_LED 11
#define GREEN_LED 12
#define BLUE_LED 13
#define LED_MASK (1 << RED_LED | 1 << GREEN_LED | 1 << BLUE_LED)

//TRANSMISSION DEFINITIONS
#define IDLE_TIME 500
#define TIME_0 400
#define TIME_1 600

//VARIABLE DECLERATIONS
volatile bool idle = true;
volatile char buffer[100];
volatile uint8_t ch_index = 0;
volatile bool transmitting = true;


// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
                
        switch(ch) {
          case ENTER: //Enter Clicked
            if(!transmitting) {
              if(ch_index > 0) {
                buffer[ch_index] = '\0';
                idle = false;
                transmitting = true;
              }
            }
          break;

          case BACKSPACE: //Backspace CLicked
            if(ch_index > 0) {
              ch_index = ch_index - 1;
              if (uart_is_writable(UART_ID)) {
                uart_putc(UART_ID, ch);
              }
            }
          break;

          case RESET: //Ctrl + L Clicked
            reset_buffer();
            //display_reset();
          break;

          default:
            if(ch_index < (sizeof(buffer)-1)) {
              buffer[ch_index] = ch;
              ch_index++;
              if (uart_is_writable(UART_ID)) {
                  uart_putc(UART_ID, ch);
              }
            }
          break;
        }
    }
}


int main(void) {
  stdio_init_all();
  adc_init();

  //initialise LED pins
  gpio_init_mask(LED_MASK);
  gpio_set_dir_out_masked(LED_MASK);

  //setup uart
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID, BAUD_RATE);

  // Set the TX and RX pins by using the function select on the GPIO
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Set UART flow control CTS/RTS, we don't want these, so turn them off
  uart_set_hw_flow(UART_ID, false, false);

  // Set our data format
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

  // Turn off FIFO's - we want to do this character by character
  uart_set_fifo_enabled(UART_ID, false);

  // Set up a RX interrupt
  // We need to set up the handler first
  // Select correct interrupt for the UART we are using
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  // And set up and enable the interrupt handlers
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);
  
  // Now enable the UART to send interrupts - RX only
  uart_set_irq_enables(UART_ID, true, false);


  while (true) {
    if(idle){
      light_flash(IDLE_TIME);
    }

    for(uint16_t i = 0; i < ch_index; i++) {
      if (buffer[i] & 1) {
        light_flash(TIME_1);
      } else {
        light_flash(TIME_0);
      }
    }
    idle = true;
    transmitting = false;
  }
}

void light_flash(uint8_t pulse_width) {
  gpio_put_masked(LED_MASK, LED_MASK);
  sleep_ms(pulse_width);
  gpio_put_masked(LED_MASK, 0);
  sleep_ms(pulse_width);
}

void reset_buffer(){
  memset(buffer, '\0', sizeof(buffer)); //Clears the array
  ch_index = 0;
}

// void display_reset(){
//   term_cls(); //clears screen
//   term_move_to(1,1); //move cursor to the top left

//   uart_puts(UART_ID, "CC2511 LAB 9");

//   term_move_to(1,3);

//   uart_puts(UART_ID, "Command Line ->\r\n");
// }