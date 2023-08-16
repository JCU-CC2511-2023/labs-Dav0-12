/**************************************************************
 * main.c
 * rev 1.0 15-Aug-2023 dghun
 * Lab4
 * ***********************************************************/

#include <stdbool.h>

#define SIO_BASE 0xd0000000
#define LED_PIN 25

#define REG_PAD_CONTROL 0x4001c068 //Input enable and output disable registor. Write 1 to pin 7, Write 0 to pin 6
#define VAL_IE_OD 0x40 //00100 0000

#define REG_GPIO25_CTRL 0x400140cc //Register for the pad control
#define FUNCTION_5_SET 5 //Set SIO function

#define REG_GPIO_SET (SIO_BASE+0x024)

#define REG_GPIO_OE_SET (SIO_BASE+0x024)
#define REG_GPIO_OUT_SET (SIO_BASE+0x014)
#define REG_GPIO_OUT_CLR (SIO_BASE+0x018)
#define TOGGLE_GPIO25 (1 << LED_PIN)

#define CONTENT_OF(addr) (*(volatile uint_32t*)addr)

typedef unsigned long uint_32t;
//Setup wait function
void wait(uint_32t wait_length){
  for(uint_32t i = 0; i<wait_length; i++) {
    __asm volatile ("nop");
  }
}

int main(void) {
  //Initialise
  CONTENT_OF(REG_PAD_CONTROL) = VAL_IE_OD;          // SETUP PAD CONTROL. INPUT AND OUTPUT ENABLED  
  CONTENT_OF(REG_GPIO25_CTRL) = FUNCTION_5_SET;     // SETS THE SIO FUNCTION
  CONTENT_OF(REG_GPIO_OE_SET) = TOGGLE_GPIO25;      // SETS THE PIN AS OUTPUT

  
  while (true) {
    CONTENT_OF(REG_GPIO_OUT_SET) = TOGGLE_GPIO25;
    wait(8000000);
    CONTENT_OF(REG_GPIO_OUT_CLR) = TOGGLE_GPIO25;
    wait(8000000);
  }
}