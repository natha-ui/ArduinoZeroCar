/*
 * GccApplication3.c
 *
 * Created: 16/04/2025 18:38:02
 * Author : nmku
 */ 


#include "sam.h"

int foo(int,int);

extern int lab_asm_port();

int main()
{
	
	SystemInit();
	
	//Pin Config: PA09 (Switch 1 input)
	PORT->Group[0].PINCFG[9].reg = 0x2;
	
	//Pin Config: PA08 (Switch 2 input)
	PORT->Group[0].PINCFG[8].reg = 0x2;
	
	// Pin Config: PA06 (LED 1 output)
	PORT->Group[0].PINCFG[6].reg = 0x0; // PA06: INEN = 0, PMUXEN = 0
	
	// Pin Config: PA07 (LED 2 output)
	PORT->Group[0].PINCFG[7].reg = 0x0; // PA07: INEN = 0, PMUXEN = 0
	
	// Direction for PA09 and PA06, PA08 and PA07
	PORT->Group[0].DIR.reg = (0x1<<6)|(0x1<<7);
	
	while (1) {
			PORT->Group[0].OUT.reg = (PORT->Group[0].IN.reg>>9) << 6 | (PORT->Group[0].IN.reg>>8) << 7;
	}
}

int foo(int a, int b) {
	return (a + b);
}
	
