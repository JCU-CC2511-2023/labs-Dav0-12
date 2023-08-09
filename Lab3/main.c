/**************************************************************
 * main.c
 * rev 1.0 06-Aug-2023 dghun
 * Lab3
 * ***********************************************************/

#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define RED_LED 11 // GPIO11
#define GREEN_LED 12 //GPIO12
#define BLUE_LED 13 // GPIO13


int main(void) {
  // initialise the stdio sub systems
  stdio_usb_init();
  // initialise the GPIO pins
  gpio_init(RED_LED);
  gpio_set_dir(RED_LED, GPIO_OUT); // sets as output

  gpio_init(GREEN_LED);
  gpio_set_dir(GREEN_LED, GPIO_OUT);

  gpio_init(BLUE_LED);
  gpio_set_dir(BLUE_LED, true);

  bool status;
  int32_t index;

  while (true) {
    int32_t ch = getchar_timeout_us(0);
    if(ch != PICO_ERROR_TIMEOUT) {
      // reset values
      status = false;
      index = 0;

      switch(ch) 
      {
        case 'r': // r pressed, meaning red
        index = RED_LED;
        printf("Toggle Red\r\n");
        break;

        case 'g': // g pressed, meaning green
        index = GREEN_LED;
        printf("Toggle Green\r\n");
        break;

        case 'b': // b pressed, meaning blue
        index = BLUE_LED;
        printf("Toggle Blue\r\n");
        break;

        default:
        printf("Unrecognised Character\r\n");
      }
      // check current state of light
      bool status = gpio_get(index); // returns true if light is on
      
      gpio_put(index, !status);
    }
    sleep_ms(20);
  }
}
