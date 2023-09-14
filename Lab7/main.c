// /**************************************************************
//  * main.c
//  * rev 1.0 05-Sep-2023 dghun
//  * Lab7
//  * ***********************************************************/

// #include "pico/stdlib.h"
// #include <stdbool.h>
// #include <stdio.h>
// #include <stdint.h>
// #include "hardware/pwm.h"
// #include "hardware/uart.h"
// #include "terminal.h"
// #include <string.h>

// #define R_LED 11
// #define G_LED 12
// #define B_LED 13
// #define R_BUTTON 2
// #define B_BUTTON 3
// #define G_BUTTON 4

// #define UART_ID uart0
// #define UART_ID_IRQ UART0_IRQ

// // We are using pins 0 and 1, but see the GPIO function select table in the
// // datasheet for information on which other pins cindexUART_IDan be used.
// #define UART_TX_PIN 0
// #define UART_RX_PIN 1

// #define BAUD_RATE 115200

// uint8_t red_val, blue_val, green_val;

// volatile char buffer[100];
// volatile uint8_t chindex = 0;

// volatile bool input_ready = false;

// void copy_input(uint8_t current_character) {
//   buffer[chindex] = current_character; // save the character
//   chindex++; // increment the index
// }

// void on_uart_rx(void) {
//   while (uart_is_readable(UART_ID)) {
//     uint8_t ch = uart_getc(UART_ID);
//     copy_input(ch); // => buffer
//     if((ch == '\r') || (ch == '\n')) {
//       input_ready = true;
//     }
//     if(uart_is_writable(UART_ID)){
//       uart_putc(UART_ID, ch);
//     }
//   }
// }

// int main(void) {
//   //initialise the stdio sub systems
//   stdio_init_all();

//   // Set the TX and RX pins by using the function select on the GPIO
//   // Set datasheet for more information on function select
//   gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
//   gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

//   //initialise uart
//   uart_init(UART_ID, BAUD_RATE);
//   irq_set_exclusive_handler(UART_ID_IRQ, on_uart_rx);
//   irq_set_enabled(UART_ID_IRQ, true);

//   //pin initialisation
//   gpio_init(R_LED);
//   gpio_init(G_LED);
//   gpio_init(R_LED);

//   gpio_set_function(R_LED, GPIO_FUNC_PWM);
//   gpio_set_function(B_LED, GPIO_FUNC_PWM);
//   gpio_set_function(G_LED, GPIO_FUNC_PWM);

//   pwm_set_enabled(pwm_gpio_to_slice_num(R_LED), true); // enables the pwm
//   pwm_set_enabled(pwm_gpio_to_slice_num(B_LED), true);
//   pwm_set_enabled(pwm_gpio_to_slice_num(G_LED), true);

//   uint8_t colour_amount;
//   char led_colour[10];

//   while (true) {
//     while (!input_ready) {
//       __asm("wfi"); //wait in low power mode
//     }  



//     // if (sscanf(buffer, "red %hhu", &colour_amount) > 0) {
//     //   red_val = colour_amount;
//     // } else if (sscanf(buffer, "blue %hhu", &colour_amount) > 0) {
//     //   blue_val = colour_amount;
//     // } else if (sscanf(buffer, "green %hhu", &colour_amount) > 0) {
//     //   green_val = colour_amount;
//     // }

//     //reset
//     input_ready = false;
//   }
// }

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "terminal.h"
#include <string.h>

#define R_LED 11
#define G_LED 12
#define B_LED 13
#define R_BUTTON 2
#define G_BUTTON 3
#define B_BUTTON 4

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#define BACKSPACE '\177'
#define ENTER 13
#define RESET '\f'

#define C_LINE 7
#define COLOUR_MAX 255
#define COLOUR_MIN 0

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

volatile bool input_ready = false;

volatile char buffer[100];
volatile uint8_t ch_index = 0;

volatile uint8_t red_val, blue_val, green_val = 0;

volatile bool manual_toggle_override = false;

volatile uint8_t debounce_counter = 0;


// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
                
        switch(ch) {
          case ENTER: //Enter Clicked
            if(ch_index > 0) {
              buffer[ch_index] = '\0';
              input_ready = true;
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
            rgb_value_display();
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

// button interrupt handler
void button_pressed_handler(uint8_t gpio_called) {

    uint8_t button_list[3] = {R_BUTTON, G_BUTTON, B_BUTTON};
    uint8_t *pointer_arr[3] = {&red_val, &green_val, &blue_val};

    for(uint8_t i = 0; i < sizeof(button_list); i++){
      if(button_list[i] == gpio_called){
        if(*pointer_arr[i] == 255){
          *pointer_arr[i] = 0;
        } else {
          *pointer_arr[i] = 255; // Default to toggling on if not currently at 255 (high)
        }
        break;
      }
    }

    input_ready = true;
    manual_toggle_override = true;

  // switch(gpio_called){
  //   case R_BUTTON:
  //     red_val = (red_toggle == false) ? 255: 0;
  //     red_toggle = !red_toggle;
  //   break;

  //   case G_BUTTON:
  //     green_val = (green_toggle == false) ? 255: 0;
  //     green_toggle = !green_toggle;
  //   break;

  //   case B_BUTTON:
  //     blue_val = (blue_toggle == false) ? 255: 0;
  //     blue_toggle = !blue_toggle;
  //   break;
  // }

  // input_ready = true;
  // manual_toggle_override = true;
} 

void reset_buffer(){
  memset(buffer, '\0', sizeof(buffer)); //Clears the array
  ch_index = 0;
}

void rgb_value_display(){
  char red_buffer[50];
  char blue_buffer[50];
  char green_buffer[50];
  

  snprintf(red_buffer, sizeof(red_buffer), "Red Value: %u\r\n", red_val);
  snprintf(blue_buffer, sizeof(blue_buffer), "Blue Value: %u\r\n", blue_val);
  snprintf(green_buffer, sizeof(green_buffer), "Green Value: %u\r\n", green_val);

  term_cls(); //clears screen
  term_move_to(1,1); //move cursor to the top left

  uart_puts(UART_ID, "CC2511 LAB 7");

  term_move_to(1,3);

  uart_puts(UART_ID, "-----PWM Values-----");
  
  term_move_to(1,4);

  uart_puts(UART_ID, red_buffer);
  uart_puts(UART_ID, blue_buffer);
  uart_puts(UART_ID, green_buffer);

  term_move_to(1, 8);

  uart_puts(UART_ID, "Command Line ->\r\n");
}

int main() {
    // Initialise stdio functions
    stdio_init_all();

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

    // OK, uart all set up.

    //pin initialisation
    gpio_init(R_LED);
    gpio_init(G_LED);
    gpio_init(B_LED);
    gpio_init(R_BUTTON);
    gpio_init(G_BUTTON);
    gpio_init(B_BUTTON);

    gpio_set_dir(R_LED, GPIO_OUT);
    gpio_set_dir(G_LED, GPIO_OUT);
    gpio_set_dir(B_LED, GPIO_OUT);

    gpio_set_dir(R_BUTTON, GPIO_IN);
    gpio_set_dir(G_BUTTON, GPIO_IN);
    gpio_set_dir(B_BUTTON, GPIO_IN);

    gpio_set_function(R_LED, GPIO_FUNC_PWM);
    gpio_set_function(G_LED, GPIO_FUNC_PWM);
    gpio_set_function(B_LED, GPIO_FUNC_PWM);

    pwm_set_enabled(pwm_gpio_to_slice_num(R_LED), true); // enables the pwm
    pwm_set_enabled(pwm_gpio_to_slice_num(G_LED), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(B_LED), true);

    //GPIO Pin Handlers
    gpio_set_irq_enabled_with_callback(R_BUTTON, GPIO_IRQ_EDGE_RISE, true, &button_pressed_handler);
    gpio_set_irq_enabled(G_BUTTON, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(B_BUTTON, GPIO_IRQ_EDGE_RISE, true);

    //Initialise the RGB values being displayed
    rgb_value_display();

    while (true) {
      while (!input_ready) {
          __asm ("wfi");
      }

      if(!manual_toggle_override) {
        int16_t colour_amount;
        if (sscanf(buffer, "red %hu", &colour_amount) > 0) {
            red_val = (colour_amount > COLOUR_MAX) ? COLOUR_MAX: ((colour_amount < COLOUR_MIN) ? COLOUR_MIN: colour_amount);
        } else if (sscanf(buffer, "blue %hu", &colour_amount) > 0) {
            blue_val = (colour_amount > COLOUR_MAX) ? COLOUR_MAX: ((colour_amount < COLOUR_MIN) ? COLOUR_MIN: colour_amount);
        } else if (sscanf(buffer, "green %hu", &colour_amount) > 0) {
            green_val = (colour_amount > COLOUR_MAX) ? COLOUR_MAX: ((colour_amount < COLOUR_MIN) ? COLOUR_MIN: colour_amount);
        } 
      }

      pwm_set_gpio_level(R_LED, red_val*red_val);
      pwm_set_gpio_level(G_LED, green_val*green_val);
      pwm_set_gpio_level(B_LED, blue_val*blue_val);

      rgb_value_display();
      reset_buffer();

      input_ready = false;
      manual_toggle_override = false;
    }
}