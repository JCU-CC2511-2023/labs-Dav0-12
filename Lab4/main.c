/**************************************************************
 * main.c
 * rev 1.0 15-Aug-2023 dghun
 * Lab4
 * ***********************************************************/

// INCLUDE BOOLEAN FUNCTIONALITY
#include <stdbool.h>

// DEFINE REGISTERS AND FUNCTIONS
#define SIO_BASE 0xd0000000
#define LED_PIN 25 // GPIO PIN 25

#define REG_PAD_CONTROL 0x4001c068 // INPUT ENABLE AND OUTPUT DISABLE REGISTER. WRITE 1 TO PIN 7, WRITE 0 TO PIN 6.
#define VAL_IE_OD (1 << 6)         // 00100 0000

#define REG_GPIO25_CTRL 0x400140cc // REGISTER FOR THE PAD CONTROL
#define FUNCTION_5_SET 5           // SIO FUNCTION INDENTIFIER

#define REG_GPIO_OE_SET (SIO_BASE + 0x024)  // GPIO PIN DIRECTION REGISTER
#define REG_GPIO_OUT_SET (SIO_BASE + 0x014) // OUTPUT SET REGISTER. TURNING ON PIN.
#define REG_GPIO_OUT_CLR (SIO_BASE + 0x018) // OUTPUT CLEAR REGISTER. TURNING OFF/CLEAR PIN.
#define TOGGLE_GPIO25 (1 << LED_PIN)

// CREATE MACRO FUNCTIONS
#define CONTENT_OF(addr) (*(volatile uint_32t *)addr) // ACCESSES THE CONTENTS OF A REGISTER ADDRESS

// CREATE VARIABLE TYPES
typedef unsigned long uint_32t;

// SETUP WAIT FUNCTION
void wait(uint_32t wait_length)
{
  for (uint_32t i = 0; i < wait_length; i++) // ARBITRARY DELAY VALUE
  { 
    __asm volatile("nop");
  }
}

int main(void)
{
  uint_32t wait_length = 8000000;

  // INITIALISE
  CONTENT_OF(REG_PAD_CONTROL) = CONTENT_OF(REG_PAD_CONTROL) | VAL_IE_OD; // SETUP PAD CONTROL. INPUT AND OUTPUT ENABLED
  CONTENT_OF(REG_GPIO25_CTRL) = FUNCTION_5_SET; // SETS THE SIO FUNCTION
  CONTENT_OF(REG_GPIO_OE_SET) = TOGGLE_GPIO25;  // SETS THE PIN AS OUTPUT

  while (true)
  {
    CONTENT_OF(REG_GPIO_OUT_SET) = TOGGLE_GPIO25; // SETS THE GPIO25 PIN TO ON
    wait(wait_length);
    CONTENT_OF(REG_GPIO_OUT_CLR) = TOGGLE_GPIO25; // SETS THE GPIO25 PIN TO OFF
    wait(wait_length);
  }
}