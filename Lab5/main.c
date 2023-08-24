/**************************************************************
 * main.c
 * rev 1.0 21-Aug-2023 dghun
 * Lab5
 * ***********************************************************/

#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "hardware/pwm.h"

// pin definitions
#define FLOATING_PIN 15
#define RED_LED 11
#define GREEN_LED 12
#define BLUE_LED 13

// macro definitions
#define CONTENT_OF(addr) (*(volatile uint32_t*)addr)
#define PAD_CONTROL_BASE 0x4001c000
#define REG_PAD_CONTROL_GPIO15 (PAD_CONTROL_BASE+0x40)
#define CTRL_PDE 0x4
#define CTRL_PUE 0x8

int main(void) {
  // initialise the stdio sub systems
  stdio_usb_init();

  //initialisation
  CONTENT_OF(REG_PAD_CONTROL_GPIO15) = CONTENT_OF(REG_PAD_CONTROL_GPIO15) &~ (CTRL_PDE | CTRL_PUE); //clear pull up/pull down resistor registers

  gpio_init(FLOATING_PIN);
  gpio_set_dir(FLOATING_PIN, GPIO_IN);

  gpio_init(RED_LED);
  gpio_init(GREEN_LED);
  gpio_init(BLUE_LED);

  gpio_set_dir(RED_LED, GPIO_OUT);
  gpio_set_dir(GREEN_LED, GPIO_OUT);
  gpio_set_dir(BLUE_LED, GPIO_OUT);

  // pwm setup
  gpio_set_function(RED_LED, GPIO_FUNC_PWM); //sets the function to pwm
  gpio_set_function(GREEN_LED, GPIO_FUNC_PWM);

  uint8_t red_level = 0;
  uint8_t green_level = 0;

  uint8_t upper_limit = 20; //upper_limit plus one is the number of steps from 0-100% duty rate
  uint8_t lower_limit = 0;

  uint32_t red_led_slice_num = pwm_gpio_to_slice_num(RED_LED); // gets the slice number associated with the gpio pin
  uint32_t green_led_slice_num = pwm_gpio_to_slice_num(GREEN_LED);

  pwm_set_wrap(red_led_slice_num, upper_limit); // sets the period of the slice to the upper limit
  pwm_set_wrap(green_led_slice_num, upper_limit);

  pwm_set_enabled(red_led_slice_num, true); // enables the pwm
  pwm_set_enabled(green_led_slice_num, true);

  while (true) {

    int32_t ch = getchar_timeout_us(0);
    if(ch != PICO_ERROR_TIMEOUT) {

      switch(ch) 
      {
        case 'u':
          CONTENT_OF(REG_PAD_CONTROL_GPIO15) = CONTENT_OF(REG_PAD_CONTROL_GPIO15) | CTRL_PUE;  // turn on pull up resistor
          CONTENT_OF(REG_PAD_CONTROL_GPIO15) = CONTENT_OF(REG_PAD_CONTROL_GPIO15) &~ CTRL_PDE; // turn off pull down resistor
          printf("Pull up enabled\r\n");
        break;

        case 'd':
          CONTENT_OF(REG_PAD_CONTROL_GPIO15) = CONTENT_OF(REG_PAD_CONTROL_GPIO15) | CTRL_PDE;  // turn on pull down resistor
          CONTENT_OF(REG_PAD_CONTROL_GPIO15) = CONTENT_OF(REG_PAD_CONTROL_GPIO15) &~ CTRL_PUE; // turn off pull up resistor
          printf("Pull down enabled\r\n");
        break;

        case 'o':
          CONTENT_OF(REG_PAD_CONTROL_GPIO15) = CONTENT_OF(REG_PAD_CONTROL_GPIO15) &~ (CTRL_PDE | CTRL_PUE); //clear pull up/pull down registers
          printf("Pull up and Pull down disabled\r\n");
        break;

        case 'r': // up red brightness
          if(red_level < upper_limit){
            red_level++;
          }
        break;

        case 'R': // down red brightness
          if(red_level > lower_limit){
            red_level--;
          }
        break;

        case 'g': // up green brightness
          if(green_level < upper_limit){
            green_level++;
          }
        break;

        case 'G': // down green brightness
          if(green_level > lower_limit) {
            green_level--;
          }
        break;

        default:
          printf("Unrecognised Character\r\n");
        break;
      }
    }

    gpio_put(BLUE_LED, gpio_get(FLOATING_PIN)); //If low, led is off. If high, led is in
    sleep_ms(20); //small delay

    // set pwm pin levels
    pwm_set_gpio_level(RED_LED, red_level);
    pwm_set_gpio_level(GREEN_LED, green_level);

  }
}
