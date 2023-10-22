/**************************************************************
 * main.c
 * rev 1.0 20-Sep-2023 dghun
 * Lab9
 * ***********************************************************/

#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

//PIN DEFINITIONS
#define LIGHT_SENSOR_PIN 26
#define RED_LED 11
#define GREEN_LED 12
#define BLUE_LED 13
#define LED_MASK (1 << RED_LED | 1 << GREEN_LED | 1 << BLUE_LED)

//TRANSMISSION DEFINITIONS
#define IDLE_TIME 50
#define TIME_0 400
#define TIME_1 600

//ADC DEFINITIONS
#define VOLTAGE_CONVERSION (3.3f / (1 << 12))
#define EDGE_TOLERANCE 150

int main(void) {
  stdio_init_all();
  adc_init();

  //initialise LED pins
  gpio_init_mask(LED_MASK);
  gpio_set_dir_out_masked(LED_MASK);

  //initialise light sensor pins
  adc_gpio_init(LIGHT_SENSOR_PIN);
  adc_select_input(0);

  //VARIABLE DECLERATIONS
  absolute_time_t rising_edge_time;
  absolute_time_t falling_edge_time;
  volatile int pulse_width_time_ms;
  int16_t previous_sensor_reading = adc_read();
  int16_t current_sensor_reading;
  volatile int pulse_width_timer;
  bool timing;
  uint8_t mode; 

  while(true) {
    mode = 1; //0 is flashing code, 1 is light detection code
    if (mode == 0) {

      //flashing code
      light_flash(IDLE_TIME);

    } else {

      //light detection code
      current_sensor_reading = adc_read();
      if((current_sensor_reading - previous_sensor_reading) >= EDGE_TOLERANCE){
        rising_edge_time = get_absolute_time();
      } else if((previous_sensor_reading - current_sensor_reading) >= EDGE_TOLERANCE) {
        falling_edge_time = get_absolute_time();
        pulse_width_time_ms = ((absolute_time_diff_us(rising_edge_time, falling_edge_time))/1000);
        printf("Pulse Width: %d ms\r\n", pulse_width_time_ms);
      }
      previous_sensor_reading = current_sensor_reading;

    }
  }
}

void light_flash(uint8_t pulse_width) {
  gpio_put_masked(LED_MASK, LED_MASK);
  sleep_ms(pulse_width);
  gpio_put_masked(LED_MASK, 0);
  sleep_ms(pulse_width);
}