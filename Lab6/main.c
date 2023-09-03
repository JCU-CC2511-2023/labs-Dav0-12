/**************************************************************
 * main.c
 * rev 1.0 31-Aug-2023 dghun
 * Lab6
 * ***********************************************************/

#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define STEP 0
#define DIR 1
#define DELAY 2

int main(void) {
  
  stdio_init_all();

  printf("Hello World\r\n");

  gpio_init(STEP);
  gpio_init(DIR);

  gpio_set_dir(STEP, GPIO_OUT);
  gpio_set_dir(DIR, GPIO_OUT);

  gpio_put(DIR, true);

  while (true) {

    int32_t ch = getchar_timeout_us(0);
    if(ch != PICO_ERROR_TIMEOUT) {

      switch(ch) 
      {
        case 'r': // r pressed, meaning right
        gpio_put(DIR, true);
        printf("Direction set to Right\r\n");
        break;

        case 'l': // l pressed, meaning left
        gpio_put(DIR, false);
        printf("Direction set to Left\r\n");
        break;

        default:
        printf("Unrecognised Character, please enter either r or l\r\n");
      }
    }
    gpio_put(STEP, true);
    sleep_ms(DELAY);
    gpio_put(STEP, false);
    sleep_ms(DELAY);
  }
}
