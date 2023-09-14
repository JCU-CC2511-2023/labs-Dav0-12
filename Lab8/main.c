/**************************************************************
 * main.c
 * rev 1.0 13-Sep-2023 dghun
 * Lab8
 * ***********************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

#define LIGHT_SENSOR_PIN 0
#define VREF_LEVEL 3.3
#define LED_PIN 1

int main(void) {
  stdio_init_all();
  adc_init();
  adc_gpio_init(LIGHT_SENSOR_PIN);
  adc_select_input(0);

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
  pwm_set_enabled(pwm_gpio_to_slice_num(LED_PIN), true); // enables the pwm


  while (true) {
    uint16_t result = adc_read();
    sleep_ms(500);
    
    

  }
}
