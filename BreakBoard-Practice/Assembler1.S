
/*
 * Assembler1.s
 *
 * Created: 16/04/2025 19:17:14
 *  Author: nmku
 */ 

 
 .syntax unified
  
 .section .text
 .globl lab_asm_port


 .thumb
 .thumb_func

 lab_asm_port:

lab_asm_port:

	//PAC1 Protect
	ldr r0, =0x41000000  // PAC1 Write Protect Clear
	ldr r1, [r0]         // check

	ldr r0, =0x41000004  // PAC1 Write Protect Set
	ldr r2, [r0]         // check

    //LED1 (PA06) Configuration
    ldr r0, =0x41004446     //Pin Configuration Register: PA06
    ldrb r1, [r0]			//check if 0
	ldr r2, =0x0			//bit = 0
	strb r2, [r0]			//store
	ldrb r3, [r0]			//check if 0

    // Set PA06 direction as output
    ldr r0, =0x41004400     //DIR register add
    ldr r1, = 0x40			//bit 06 = 1
    str r1, [r0]			//set as output

    //Switch1 (PA09) Configuration
    ldr r0, =0x41004449     //Pin Configuration Register: PA09
    ldrb r1, [r0]			//check if 0
	ldr r2, =0x2			//bit = 2
	strb r2, [r0]			//store
	ldrb r3, [r0]			//check if 0
	
	//LED2 (PA07) Configuration
    ldr r0, =0x41004447		//Pin Configuration Register: PA07
    ldrb r1, [r0]			//check if 0
	ldr r2, =0x0			//bit = 0
	strb r2, [r0]			//store
	ldrb r3, [r0]			//check if 0

    // Set PA07 direction as output
    ldr r0, =0x41004400     //DIR register add
    ldr r1, = 0x80			//bit = 7
    str r1, [r0]			//store as output


    //Switch2 Configuration
    ldr r0, =0x41004448     //Pin Configuration Register: PA08
    ldrb r1, [r0]			//check if 0
	ldr r2, =0x2			//bit = 2
    strb r2, [r0]			//store	
	ldrb r3, [r0]			//check if 0

    //IN register - System 1
    ldr r3, =0x41004418     //IN register
    ldr r1, [r3]            //load input val
    movs r3, #1				//setup for comparison 
    lsls r3, r3, #9         //(1 << 9) shifting A06 to 9position
    ands r1, r1, r3         //check if not pressed

	//OUT register - System 1
    ldr r0, =0x41004410     //OUT register address
    ldr r4, [r0]            //Current OUT value
    movs r5, #1				//setup for comparison
    lsls r5, r5, #06        //(1 << 06) shifting PA06 to 6position

    cmp r1, #0              //check if switch pressed
    beq led1_off            //if not, turn off

led1_on:
    orrs r4, r4, r5         //Set PA06 as ON
    b exit

led1_off:
    bics r4, r4, r5			//clear PA06 so LED off
	b exit

	 //IN register - System 2
    ldr r3, =0x41004418     //IN register address
    ldr r1, [r3]            //Read input value
    movs r3, #1				//setup for shift
    lsls r3, r3, #8         // (1 << 8) shifting PA07 to 8position
    ands r1, r1, r3         // check if not pressed

	//OUT register - System 2
    ldr r0, =0x41004410     //OUT register address
    ldr r4, [r0]            //OUT value
    movs r5, #1				//setup for shift
    lsls r5, r5, #07        //shifting back

    cmp r1, #0              //check if switch pressed
    beq led2_off            //if not, turn off

led2_on:
    orrs r4, r4, r5         //set PA07 as ON
    b exit					//exit

led2_off:
    bics r4, r4, r5         //clear PA07 so LED off

exit:
    str r4, [r0]            // Write updated OUT register
    bx lr
