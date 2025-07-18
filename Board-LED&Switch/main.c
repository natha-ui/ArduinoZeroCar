/*
 * GccApplication_lab_prep.c
 *
 * Created: 7/27/2023 10:26:32 AM
 * Author : nmku
 */ 

#pragma GCC target ("thumb")

#include "sam.h"

int foo(int, int);

extern int lab_asm_port_test();
extern int lab_asm_func_test();

int main()
{
	int c;
	/* Initialize the SAM system */
	SystemInit();
	
	//lab_asm_port_test();
	
	// Pin Configuration: PA07 (Switch input)
	PORT->Group[0].PINCFG[09].reg = 0x2; // PA19: INEN = 1, PMUXEN = 0
	
	// Pin Configuration: PA17 (LED output)
	PORT->Group[0].PINCFG[6].reg = 0x0; // PA17: INEN = 0, PMUXEN = 0
	
	// Direction for PA19 and PA17
	PORT->Group[0].DIR.reg = 0x1 << 6; //  Direction: PA19 (Input), PA17 (Output)
	
	while (1) {
		PORT->Group[0].OUT.reg = (PORT->Group[0].IN.reg >> 9) << 6;
	}
	
	c = lab_asm_func_test();
	c = foo(c, 2);
	
	return (c);
}

int foo(int a, int b) {
	return (a + b);
}
