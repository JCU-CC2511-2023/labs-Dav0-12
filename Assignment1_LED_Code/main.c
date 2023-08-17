/**************************************************************
 * main.c
 * rev 1.0 16-Aug-2023 dghun
 * Assignment1_Code
 * ***********************************************************/

#include "pico/stdlib.h"
#include <stdbool.h>

//DEFINE BUTTON PINS
#define R_BUTTON_PIN 2 //RED BUTTON
#define G_BUTTON_PIN 3 //GREEN BUTTON
#define B_BUTTON_PIN 4 //BLUE BUTTON

//DEFINE LED PINS
#define R_LED_PIN 11 //RED LED
#define G_LED_PIN 12 //GREEN LED
#define B_LED_PIN 13 //BLUE LED

//EXTRA DEFINITIONS
#define BUTTON_PIN_MASK (1 << R_BUTTON_PIN | 1 << G_BUTTON_PIN | 1 << B_BUTTON_PIN) //MASK WHERE THE RGB BUTTON PIN BITS ARE SET TO 1
#define LED_PIN_MASK (1 << R_LED_PIN | 1 << G_LED_PIN | 1 << B_LED_PIN) //MASK WHERE THE RGB LED PIN BITS ARE SET TO 1

int main(void) {
  //INITIALISE
  gpio_init_mask(BUTTON_PIN_MASK | LED_PIN_MASK); //INITIALISES THE GPIO PINS IN THE MASK 

  //SET DIRECTIONS OF GPIO PINS
  gpio_set_dir_in_masked(BUTTON_PIN_MASK); //SETS THE BUTTON GPIO PINS TO INPUT
  gpio_set_dir_out_masked(LED_PIN_MASK); //SETS THE LED GPIO PINS TO OUTPUT

  while (true) {
    sleep_ms(10); //WAITS A SMALL AMOUNT OF TIME TO ALLOW BACKGROUND PROCESSES TO RUN

    //UPDATE RED LED STATUS
    gpio_put(R_LED_PIN, !gpio_get(R_BUTTON_PIN)); //CHECKS THE STATUS OF THE RED BUTTON. WILL RETURN 0 IF PRESSED. THEN UPDATES THE LIGHT.

    //UPDATE GREEN LED STATUS
    gpio_put(G_LED_PIN, !gpio_get(G_BUTTON_PIN));

    //UPDATE BLUE LED STATUS
    gpio_put(B_LED_PIN, !gpio_get(B_BUTTON_PIN));
  }
}

/*This is a simple programme which checks whether a button is pressed, then turns on the corresponding LED. 
  Multiple buttons can be pressed at the same time to create different colour combinations. If no buttons
  are pressed then the RGB LED will be off */
