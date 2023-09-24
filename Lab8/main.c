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

#define LIGHT_SENSOR_PIN 26
#define VREF_LEVEL 3.3
#define RED_PIN 11
#define GREEN_PIN 12
#define BLUE_PIN 13
#define VOLTAGE_CONVERSION (3.3f / (1 << 12))
#define VOLTAGE_TO_LIGHT_RATIO (255/3.3)

int main(void) {
  stdio_init_all();
  adc_init();
  adc_gpio_init(LIGHT_SENSOR_PIN);
  adc_select_input(0);

  gpio_init(RED_PIN);
  gpio_init(GREEN_PIN);
  gpio_init(BLUE_PIN);

  gpio_set_dir(RED_PIN, GPIO_OUT);
  gpio_set_dir(GREEN_PIN, GPIO_OUT);
  gpio_set_dir(BLUE_PIN, GPIO_OUT);

  gpio_set_function(RED_PIN, GPIO_FUNC_PWM);
  gpio_set_function(GREEN_PIN, GPIO_FUNC_PWM);
  gpio_set_function(BLUE_PIN, GPIO_FUNC_PWM);

  pwm_set_wrap(pwm_gpio_to_slice_num(RED_PIN), 255);
  pwm_set_wrap(pwm_gpio_to_slice_num(GREEN_PIN), 255);
  pwm_set_wrap(pwm_gpio_to_slice_num(BLUE_PIN), 255);

  pwm_set_enabled(pwm_gpio_to_slice_num(RED_PIN), true); // enables the pwm
  pwm_set_enabled(pwm_gpio_to_slice_num(GREEN_PIN), true); 
  pwm_set_enabled(pwm_gpio_to_slice_num(BLUE_PIN), true); 

  while (true) {
    uint16_t result = adc_read();
    uint16_t light_ratio = result * VOLTAGE_CONVERSION * VOLTAGE_TO_LIGHT_RATIO;
    // float sensor_voltage = result * VOLTAGE_CONVERSION;

    // if (light_ratio < 0) {
    //   light_ratio = 0;
    // } else if (light_ratio > 255) {
    //   light_ratio = 255;
    // }

    //printf("Raw value: %u, voltage: %f, pwm value: %u\r\n", result, sensor_voltage, light_ratio);

    // pwm_set_gpio_level(RED_PIN, light_ratio);
    // pwm_set_gpio_level(GREEN_PIN, light_ratio);
    pwm_set_gpio_level(BLUE_PIN, light_ratio);
    sleep_ms(20);
  }
}
