/** 
 * CC2511 Lab 10
 */

/* Assembler directives */

.syntax unified /* Use modern syntax                                                    */
.text           /* The "text" section of the executable is where code is placed.        */
.thumb_func     /* Specifies that main is a thumb mode function and will be offset by 1 */ 
.global main    /* Declare "main" as a global symbol (callable from other files).       */
.align 4        /* Make sure the starting address is a multiple of 4.                   */

/* Definitions */
/* These are equivalent to C macros. However, note that they use a comma. */

.EQU LED_PIN,                 25
.EQU WAIT_LOOPS_ON,           10000000
.EQU WAIT_LOOPS_OFF,          10000000

/* TODO - Add definitions of register addresses used in the program */

.EQU REG_PAD_CONTROL,      0x4001c068 
.EQU VAL_INPUT_ENABLE,     01000000
.EQU VAL_OUTPUT_DISABLE,   10000000

.EQU REG_GPIO25_FUNC_CTRL, 0x400140cc // REGISTER FOR THE GPIO25 CONTROL 
.EQU REG_GPIO_OE_SET,      0xd0000024
.EQU REG_GPIO_OUT_SET,     0xd0000014
.EQU REG_GPIO_OUT_CLR,     0xd0000018

.EQU TOGGLE_GPIO25,        0x2000000

/* Main function */
main:
  push {lr}

  /* Initialise 3 registers, as in Lab 3 */ 

  /* 
  CONTENT_OF(REG_PAD_CONTROL) = CONTENT_OF(REG_PAD_CONTROL) | VAL_INPUT_ENABLE;
  CONTENT_OF(REG_PAD_CONTROL) = CONTENT_OF(REG_PAD_CONTROL) &~ VAL_OUTPUT_DISABLE;
  */
  ldr r0, =REG_PAD_CONTROL
  ldr r1, [r0]
  ldr r3, =VAL_INPUT_ENABLE
  ldr r4, =VAL_OUTPUT_DISABLE
  orrs r1, r3
  mvns r4, r4
  ands r1, r4
  str r1, [r0]

  /*
  CONTENT_OF(REG_GPIO25_FUNC_CTRL) = CONTENT_OF(REG_GPIO25_FUNC_CTRL) &~ 0x1F; //SETS BITS 0-4 OF THE FUNCTION REGISTER TO 0
  CONTENT_OF(REG_GPIO25_FUNC_CTRL) = CONTENT_OF(REG_GPIO25_FUNC_CTRL) | FUNCTION_5_SET; // SETS THE SIO FUNCTION
  */
  ldr r0, =REG_GPIO25_FUNC_CTRL
  ldr r1, [r0]
  movs r3, #5
  movs r4, #0x1F
  mvns r4, r4
  ands r1, r4 
  orrs r1, r3
  str r1, [r0]

  /*
  CONTENT_OF(REG_GPIO_OE_SET) = TOGGLE_GPIO25;  // SETS THE PIN AS OUTPUT
  */
  ldr r0, =REG_GPIO_OE_SET
  ldr r2, =TOGGLE_GPIO25
  str r2, [r0]


  /*
   *  Run in an endless loop that:
   *    (i)    Turns on the LED
   *    (ii)   Waits, by using an inner loop
   *    (iii)  Turns off the LED
   *    (iv)   Waits, by using an inner loop
   *    (v)    Returns to the top of the loop 
   */

  ldr r0, =REG_GPIO_OUT_SET
  ldr r1, =REG_GPIO_OUT_CLR

  loop:

    ldr r4, =WAIT_LOOPS_ON
    ldr r5, =WAIT_LOOPS_OFF

    /*
    CONTENT_OF(REG_GPIO_OUT_SET) = TOGGLE_GPIO25; // SETS THE GPIO25 PIN TO ON
    wait(wait_length);
    CONTENT_OF(REG_GPIO_OUT_CLR) = TOGGLE_GPIO25; // SETS THE GPIO25 PIN TO OFF
    wait(wait_length);
    */

    str r2, [r0]
    wait_on:
      subs r4, #1
      cmp r4, #0
      bne wait_on
    str r2, [r1]
    wait_off:
      subs r5, #1
      cmp r5, #0
      bne wait_off
    b loop

	pop {pc}  /* Return from this function, by popping the return address into the program counter (pc) */
