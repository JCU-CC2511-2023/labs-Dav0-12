.syntax unified
.text
.align 2
.global asm_average

/* C Declaration                                             */
/* extern uint32_t asm_average(uint32_t Q, uint32_t* input); */

asm_average:
    /*
        On entry:
        Q       r0 (1-16) // ln2(count)
        input   r1        // address of array containing count 32-bit unsigned integers
    */
    push    {r4-r7, lr}

    movs r2, #4
    lsls r2, r0

    movs r5, #0
    movs r4, #0

    sum_loop:
        subs r2, #4
        ldr r6, [r1, r2]
        adds r5, r6
        bcs carry_loop
        check:
        cmp r2, #0
        bne sum_loop
        b division

    carry_loop:
    adds r4, #1
    b check

    division:
    /* TODO - put the result into r0 */
    movs r6, #32
    subs r6, r0

    lsls r4, r6
    lsrs r5, r0
    adds r0, r4, r5
    //ldr r0, r4
    pop {r4-r7, pc}