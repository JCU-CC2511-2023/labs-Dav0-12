/**************************************************************
 * main.c
 * rev 1.0 02-Nov-2023 dghun
 * Hunter_David_Exam
 * ***********************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hardware/uart.h"
#include "hardware/adc.h"

// Define UART macros
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// Define Light Sensor macros
#define LIGHT_SENSOR_PIN 26
#define VREF_LEVEL 3.3
#define VOLTAGE_CONVERSION (3.3f / (1 << 12))
#define VOLTAGE_TO_LIGHT_RATIO (255/3.3)

//DEFINE BUTTON PINS
#define R_BUTTON_PIN 2 //RED BUTTON
#define G_BUTTON_PIN 3 //GREEN BUTTON
#define B_BUTTON_PIN 4 //BLUE BUTTON

//DEFINE LED PINS
#define R_LED_PIN 11 //RED LED
#define G_LED_PIN 12 //GREEN LED
#define B_LED_PIN 13 //BLUE LED

// Setup for task 3
volatile bool game_start = false;
volatile int rand_number;
volatile char choice;
volatile bool waiting = false;
volatile bool is_hard_mode = false;

volatile bool is_red_pressed = false;
volatile bool is_green_pressed = false;
volatile bool is_blue_pressed = false;
uint16_t chosen_colours_number = 0;
uint16_t score = 0;

void on_uart_rx() {
  choice = uart_getc(UART_ID);
  switch(choice) {
    case 'g':
      game_start = !game_start;
      is_hard_mode = false;
      if (game_start){
        uart_puts(UART_ID, "Entering game mode\r\n");
      }
      else
      {
        uart_puts(UART_ID, "Exiting game mode\r\n"); 
      }
      
      score = 0;
    break;

    case 'm':
      if (game_start) {
        is_hard_mode = !is_hard_mode;
        if (is_hard_mode)
        {
          uart_puts(UART_ID, "Switching to 7 colour mode\r\n");
          score = 0;
        }
        else
        {
          uart_puts(UART_ID, "Switching to 3 colour mode\r\n");
          score = 0;
        }
      }
  }
}

void uart_setup(){
  // We are using pins 0 and 1, but see the GPIO function select table in the
  // datasheet for information on which other pins can be used.
  #define UART_TX_PIN 0
  #define UART_RX_PIN 1


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
}

void adc_setup() {
  adc_init();
  adc_gpio_init(LIGHT_SENSOR_PIN);
  adc_select_input(0);
}

void gpio_setup() {
  #define BUTTON_PIN_MASK (1 << R_BUTTON_PIN | 1 << G_BUTTON_PIN | 1 << B_BUTTON_PIN) //MASK WHERE THE RGB BUTTON PIN BITS ARE SET TO 1
  #define LED_PIN_MASK (1 << R_LED_PIN | 1 << G_LED_PIN | 1 << B_LED_PIN) //MASK WHERE THE RGB LED PIN BITS ARE SET TO 1

  //INITIALISE
  gpio_init_mask(BUTTON_PIN_MASK | LED_PIN_MASK); //INITIALISES THE GPIO PINS IN THE MASK 

  //SET DIRECTIONS OF GPIO PINS
  gpio_set_dir_in_masked(BUTTON_PIN_MASK); //SETS THE BUTTON GPIO PINS TO INPUT
  gpio_set_dir_out_masked(LED_PIN_MASK); //SETS THE LED GPIO PINS TO OUTPUT
}

int main(void) {
  // Setup for task 1
  stdio_init_all();
  uart_setup();
  uart_puts(UART_ID, "CC2511 Exam 2022\r\nDavid Hunter\r\n");

  // Setup for task 2
  adc_setup();
  gpio_setup();
  float values_to_average[5];
  float sum;
  float average;
  float previous_average = 0;
  uint8_t index = 0;
  int8_t score = 0;

  while (true) {
    // sleep_ms(500);

    // // Task 2 code
    // uint16_t result = adc_read();
    // float voltage = (float)result * VOLTAGE_CONVERSION;
    // printf("voltage = %f\r\n", voltage);

    // //Average calculation
    // if (index < 5){
    //   sum += voltage;
    //   values_to_average[index] = voltage;
    //   index++;
    // } else {
    //   sum -= values_to_average[0];
    //   for (int i = 0; i < 4; i++) {
    //     values_to_average[i] = values_to_average[i+1];
    //   }
    //   values_to_average[4] = voltage;
    //   sum += voltage;
    // }
    // average = sum / (float)index;

    // if (average < previous_average) {
    //   gpio_put(R_LED_PIN, true);
    // } else {
    //   gpio_put(R_LED_PIN, false);
    // }
    // previous_average = average;

    // printf("average = %f\n", average);

    //Task 3 code
    if (game_start){
      
      chosen_colours_number = 0;

      if (is_hard_mode) {
        srand(time(0));
        rand_number = (rand() % 7) + 1; // Returns a random number between 1 and 7 inclusive

        if (rand_number == 1) {
        gpio_put(R_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(R_LED_PIN, false);
        }
        else if (rand_number == 2) {
        gpio_put(G_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(G_LED_PIN, false);
        }
        else if (rand_number == 3) {
        gpio_put(R_LED_PIN, true);
        gpio_put(G_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(R_LED_PIN, false);
        gpio_put(G_LED_PIN, false);
        }
        else if (rand_number == 4) {
        gpio_put(B_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(B_LED_PIN, false);
        }
        else if (rand_number == 5) {
        gpio_put(R_LED_PIN, true);
        gpio_put(B_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(R_LED_PIN, false);
        gpio_put(B_LED_PIN, false);
        }
        else if (rand_number == 6) {
        gpio_put(B_LED_PIN, true);
        gpio_put(G_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(B_LED_PIN, false);
        gpio_put(G_LED_PIN, false);
        }
        else if (rand_number == 7) {
        gpio_put(R_LED_PIN, true);
        gpio_put(G_LED_PIN, true);
        gpio_put(B_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(R_LED_PIN, false);
        gpio_put(G_LED_PIN, false);
        gpio_put(B_LED_PIN, false);
        }
      }
      else
      {
        srand(time(0));
        rand_number = rand() % 3; // Returns a random number between 0 and 2 inclusive

        if (rand_number == 0) {
        gpio_put(R_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(R_LED_PIN, false);
        }
        else if (rand_number == 1) {
        gpio_put(G_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(G_LED_PIN, false);
        }
        else if (rand_number == 2) {
        gpio_put(B_LED_PIN, true);
        sleep_ms(1000);
        gpio_put(B_LED_PIN, false);
        }
      }


      absolute_time_t timeout_time = make_timeout_time_ms(3000);
      absolute_time_t current_time = get_absolute_time();
      int64_t time_difference = absolute_time_diff_us(current_time, timeout_time);

      while (time_difference > 0){
        if (!gpio_get(R_BUTTON_PIN)){
          is_red_pressed = true;
        }
        else if (!gpio_get(G_BUTTON_PIN)){
          is_green_pressed = true;
        }
        else if (!gpio_get(B_BUTTON_PIN)){
          is_blue_pressed = true;
        }
        current_time = get_absolute_time();
        time_difference = absolute_time_diff_us(current_time, timeout_time);
      }

      if (is_hard_mode) {
        chosen_colours_number += is_red_pressed ? 1:0;
        chosen_colours_number += is_green_pressed ? 2:0;
        chosen_colours_number += is_blue_pressed ? 4:0;
      } else {
        chosen_colours_number += is_red_pressed ? 0:0;
        chosen_colours_number += is_green_pressed ? 1:0;
        chosen_colours_number += is_blue_pressed ? 2:0;

        if (!is_red_pressed && !is_green_pressed && !is_blue_pressed){
          chosen_colours_number = 3; //There will never be a 4th colour in this mode
        }
      }

      if (chosen_colours_number == rand_number){
        score++;
        printf("Correct score = %d\r\n", score);
      }
      else
      {
        score--;
        printf("Incorrect score = %d\r\n", score);
      }

      
      if (score >= 5)
      {
        printf("Congratulations! You won!\r\n");
      }
      else if (score <= -5)
      {
        printf("Hard luck, you lost!\r\n");
      }

      if ((score >= 5) || (score <= -5)){
        score = 0;
        if (is_hard_mode) {
          printf("Starting again in 7 colour mode\r\n");
        }
        else
        {
          printf("Starting again in 3 colour mode\r\n");
        }
      } 

      is_red_pressed = false;
      is_green_pressed = false;
      is_blue_pressed = false;

    }
  }
}

