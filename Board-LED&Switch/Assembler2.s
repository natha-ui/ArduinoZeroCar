/*
 * Assembler2.s
 *
 * Created: 17/04/2025 02:01:10
 *  Author: Natha
 */ 
 .syntax unified
.thumb
.thumb_func

.section .text
.global lab_asm_port_test

lab_asm_port_test:
    // === Configure PA20 as output (LED) ===
    ldr r0, =0x41004446     // PINCFG[20]
    movs r1, #0x0           // PMUXEN = 0, INEN = 0
    strb r1, [r0]

    // Set PA20 direction as output
    ldr r0, =0x41004400     // DIR register
    ldr r1, [r0]
    movs r2, #1
    lsls r2, r2, #6        // r2 = (1 << 20)
    orrs r1, r1, r2
    str r1, [r0]

    // === Configure PA07 as input (Switch) ===
    ldr r0, =0x41004449     // PINCFG[7]
    movs r1, #0x2           // INEN = 1, PMUXEN = 0
    strb r1, [r0]

    // PA07 does not need DIR set — default is input

    // === Read PA07 and control PA20 ===
    ldr r3, =0x41004420     // IN register
    ldr r1, [r3]            // Read input pins
    movs r3, #1
    lsls r3, r3, #9         // r3 = (1 << 7) ? mask for PA07
    ands r1, r1, r3         // r1 = r1 & (1 << 7)

    ldr r0, =0x41004410     // OUT register
    ldr r4, [r0]            // Current OUT value
    movs r5, #1
    lsls r5, r5, #6        // r5 = (1 << 20)

    cmp r1, #0              // Is switch pressed?
    beq led_off             // If not, turn off LED

led_on:
    orrs r4, r4, r5         // Set PA20 high
    b write_out

led_off:
    bics r4, r4, r5         // Clear PA20 (LED off)

write_out:
    str r4, [r0]            // Write updated OUT register
    bx lr