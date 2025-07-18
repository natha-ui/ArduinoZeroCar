// nmku 

#pragma GCC target ("thumb")

#include "sam.h"
#include "uart_print.h"

void GCLK_setup();
void USART_setup();
void PORT_setup();
void TC3_setup();
void TC4_setup();


int main()
 {
 	unsigned char dashline[]     = {"--------------------------------"};
	unsigned char motor_intro[]  = {"   Motor Control Instructions"};
	unsigned char motor_inst_1[] = {" - '1' Forward"};
	unsigned char motor_inst_2[] = {" - '2' Backward"};
	unsigned char motor_inst_3[] = {" - '3' to increase speed by 50"};
	unsigned char motor_inst_4[] = {" - '4' to decrease speed by 50"};
	unsigned char motor_inst_5[] = {" - '5' for left turn"};
	unsigned char motor_inst_6[] = {" - '6' for right turn"};
	unsigned char motor_inst_7[] = {" - '7' to stop"};
	
	unsigned char rx_data;
	_Bool	led, rxc_flag;	
	
	/* Initialize the SAM system */
    SystemInit();
	
	GCLK_setup();
	
	USART_setup();

	PORT_setup();
	
	TC3_setup(); 

	TC4_setup(); 
	
// 

	led = 0;

//

	println_string(dashline,     sizeof(dashline));
	println_string(motor_intro,  sizeof(motor_intro));
	println_string(motor_inst_1, sizeof(motor_inst_1));
	println_string(motor_inst_2, sizeof(motor_inst_2));		
	println_string(motor_inst_3, sizeof(motor_inst_3));	
	println_string(motor_inst_4, sizeof(motor_inst_4));		
	println_string(motor_inst_5, sizeof(motor_inst_5));
	println_string(motor_inst_6, sizeof(motor_inst_6));	
	println_string(motor_inst_7, sizeof(motor_inst_7));		
	println_string(dashline,     sizeof(dashline));	
//

	while(1) {
		rxc_flag = SERCOM5->USART.INTFLAG.bit.RXC; // check out RXC (Receive Complete) flag
		if (rxc_flag == 1)	{
			rx_data	= SERCOM5->USART.DATA.reg; // Read the received data
			SERCOM5->USART.DATA.reg	= rx_data; // Write the received data (echo back to PC)
			
			if (rx_data == '7') {
				PORT->Group[0].OUT.reg = led << 17; // Turn on Built-in LED: Output register
				led = led ^ 1; // toggle
				
				TC3->COUNT16.CC[1].reg = 0;  // stop by generating no pulse
				TC4->COUNT16.CC[1].reg = 0;  // stop by generating no pulse
			}
			else if (rx_data == '1') { //set forward
				PORT->Group[0].OUTCLR.reg = (1 << 10); //IN1 = 0
				PORT->Group[0].OUTSET.reg = (1 << 11); //IN2 = 1

				PORT->Group[0].OUTSET.reg = (1 << 8); //IN3 = 1
				PORT->Group[0].OUTCLR.reg = (1 << 9); //IN4 = 0
				
			}
			else if (rx_data == '2') {
				PORT->Group[0].OUTSET.reg = (1 << 10); //IN1 = 0
				PORT->Group[0].OUTCLR.reg = (1 << 11); //IN2 = 1
				
				PORT->Group[0].OUTCLR.reg = (1 << 8); //IN3 = 0
				PORT->Group[0].OUTSET.reg = (1 << 9); //IN4 = 1
			}
			else if (rx_data == '3') {
				TC3->COUNT16.CC[1].reg = TC3->COUNT16.CC[1].reg + 20;  // Increase speed L
				TC4->COUNT16.CC[1].reg = TC4->COUNT16.CC[1].reg + 20;  // Increase speed R
			}
			else if (rx_data == '4') {
				TC3->COUNT16.CC[1].reg = TC3->COUNT16.CC[1].reg - 20;  // Decrease speed L
				TC4->COUNT16.CC[1].reg = TC4->COUNT16.CC[1].reg - 20;  // Decrease speed R
			}
			else if (rx_data == '5') { // Turn Left
				PORT->Group[0].OUTCLR.reg = (1 << 10); //IN1 = 0
				PORT->Group[0].OUTSET.reg = (1 << 11); //IN2 = 1 so right motor forward
				
				PORT->Group[0].OUTCLR.reg = (1 << 8); //IN3 = 0
				PORT->Group[0].OUTSET.reg = (1 << 9); //IN4 = 1 so left motor back
			}
			else if (rx_data == '6') { // Turn Right
				PORT->Group[0].OUTSET.reg = (1 << 10); //IN1 = 1
				PORT->Group[0].OUTCLR.reg = (1 << 11); //IN2 = 0 so right motor back
				
				PORT->Group[0].OUTSET.reg = (1 << 8); //IN3 = 1
				PORT->Group[0].OUTCLR.reg = (1 << 9); //IN4 = 0 so left motor forward
			}
			/*Alternative turning method - increasing pwm on outside
			Listed as instructions 8 and 9
			//LEFT TURN
			else if (rx_data == '8') {
				TC4->COUNT16.CC[1].reg = TC4->COUNT16.CC[1].reg + 20; //INCREASE R SPEED
			}
			//RIGHT TURN
			else if (rx_data == '9') {
				TC3->COUNT16.CC[1].reg = TC3->COUNT16.CC[1].reg + 20; //INCREASE L SPEED
			}*/
		}
		
	};

	return (0);
}



void GCLK_setup() {
	
	// OSC8M
	//SYSCTRL->OSC8M.bit.ENABLE = 0; // Disable
	SYSCTRL->OSC8M.bit.PRESC = 0;  // prescalar to 1
	SYSCTRL->OSC8M.bit.ONDEMAND = 0;
	//SYSCTRL->OSC8M.bit.ENABLE = 1; // Enable

	//
	// Generic Clock Controller setup for TC3
	// * TC3 ID: #0x1B 
	// * Generator #0 is feeding TC3
	// * Generator #0 is taking the clock source #6 (OSC8M: 8MHz clock input) as an input
	//
	GCLK->GENCTRL.bit.ID = 0; // Generator #0
	GCLK->GENCTRL.bit.SRC = 6; // OSC8M
	GCLK->GENCTRL.bit.OE = 1 ;  // Output Enable: GCLK_IO
	GCLK->GENCTRL.bit.GENEN = 1; // Generator Enable
	
	GCLK->CLKCTRL.bit.ID = 0x1B; // ID #ID (TCC2, TC3)
	GCLK->CLKCTRL.bit.GEN = 0; // Generator #0 selected for TCC2, TC3
	GCLK->CLKCTRL.bit.CLKEN = 1; // Now, clock is supplied to TCC2, TC3
	
	GCLK->CLKCTRL.bit.ID = 0x1C; // ID #ID (TC4, TC5)
	GCLK->CLKCTRL.bit.GEN = 0; // Generator #0 selected for TC4, TC5
	GCLK->CLKCTRL.bit.CLKEN = 1; // Now, clock is supplied to TC4, TC5
	

}

void PORT_setup() {

	//
	// PORT setup for Built-in LED output (PA17)
	//
	PORT->Group[0].PINCFG[17].reg = 0x0; // peripheral mux enable = 0
	PORT->Group[0].DIR.reg = 0x1 << 17; // Direction: Output
	
	//
	// PORT setup for PA14 (GCLK_IO[0]) to check out clock output using logic analyzer
	//
	PORT->Group[0].PINCFG[14].reg = 0x41;		// peripheral mux: DRVSTR=1, PMUXEN = 1
	PORT->Group[0].PMUX[7].bit.PMUXE = 0x07;	// peripheral function H selected
	
	//Motor direction control
	PORT->Group[0].DIRSET.reg = (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11); // setting each IN as an output
	PORT->Group[0].OUTSET.reg = (1 << 9) | (1 << 10); // setting forward movement as intiial
	PORT->Group[0].OUTCLR.reg = (1 << 8) | (1 << 11); // 
}


void TC3_setup() {

	//
	// PORT setup for PA18 ( TC3's WO[0] )
	//
	PORT->Group[0].PINCFG[18].reg = 0x41;		// peripheral mux: DRVSTR=1, PMUXEN = 1
	PORT->Group[0].PMUX[9].bit.PMUXE = 0x04;	// peripheral function E selected

	//
	// PORT setup for PA19 ( TC3's WO[1] )
	//
	PORT->Group[0].PINCFG[19].reg = 0x41;		// peripheral mux: DRVSTR=1, PMUXEN = 1
	PORT->Group[0].PMUX[9].bit.PMUXO = 0x04;	// peripheral function E selected

	// Power Manager
	PM->APBCMASK.bit.TC3_ = 1 ; // Clock Enable (APBC clock) for TC3

	//
	// TC3 setup: 16-bit Mode
	//

	TC3->COUNT16.CTRLA.bit.MODE = 0;  // Count16 mode
	TC3->COUNT16.CTRLA.bit.WAVEGEN = 3 ; // Match PWM (MPWM)
	TC3->COUNT16.CTRLA.bit.PRESCALER = 6; // Timer Counter clock 31,250 Hz = 8MHz / 256
	//TC3->COUNT16.CC[0].reg = 30000;  // CC0 defines the period
	//TC3->COUNT16.CC[1].reg = 10000;  // CC1 match pulls down WO[1]
	TC3->COUNT16.CC[0].reg = 1000;  // CC0 defines the period
	TC3->COUNT16.CC[1].reg = 200;  // CC1 match pulls down WO[1]	
	TC3->COUNT16.CTRLA.bit.ENABLE = 1 ;
}


void TC4_setup() {

	//
	// PORT setup for PA22 ( TC4's WO[0] )
	//
	PORT->Group[0].PINCFG[22].reg = 0x41;		// peripheral mux: DRVSTR=1, PMUXEN = 1
	PORT->Group[0].PMUX[11].bit.PMUXE = 0x04;	// peripheral function E selected

	//
	// PORT setup for PA23 ( TC4's WO[1] )
	//
	PORT->Group[0].PINCFG[23].reg = 0x41;		// peripheral mux: DRVSTR=1, PMUXEN = 1
	PORT->Group[0].PMUX[11].bit.PMUXO = 0x04;	// peripheral function E selected

	// Power Manager
	PM->APBCMASK.bit.TC4_ = 1 ; // Clock Enable (APBC clock) for TC3

	//
	// TC4 setup: 16-bit Mode
	//

	TC4->COUNT16.CTRLA.bit.MODE = 0;  // Count16 mode
	TC4->COUNT16.CTRLA.bit.WAVEGEN = 3 ; // Match PWM (MPWM)
	TC4->COUNT16.CTRLA.bit.PRESCALER = 6; // Timer Counter clock 31,250 Hz = 8MHz / 256
	//TC3->COUNT16.CC[0].reg = 30000;  // CC0 defines the period
	//TC3->COUNT16.CC[1].reg = 10000;  // CC1 match pulls down WO[1]
	TC4->COUNT16.CC[0].reg = 1000;  // CC0 defines the period
	TC4->COUNT16.CC[1].reg = 200;  // CC1 match pulls down WO[1]
	TC4->COUNT16.CTRLA.bit.ENABLE = 1 ;
}



void USART_setup() {
	
	//
	// PORT setup for PB22 and PB23 (USART)
	//
	PORT->Group[1].PINCFG[22].reg = 0x41; // peripheral mux: DRVSTR=1, PMUXEN = 1
	PORT->Group[1].PINCFG[23].reg = 0x41; // peripheral mux: DRVSTR=1, PMUXEN = 1

	PORT->Group[1].PMUX[11].bit.PMUXE = 0x03; // peripheral function D selected
	PORT->Group[1].PMUX[11].bit.PMUXO = 0x03; // peripheral function D selected

	// Power Manager
	PM->APBCMASK.bit.SERCOM5_ = 1 ; // Clock Enable (APBC clock) for USART
	
	//
	// * SERCOM5: USART
	// * Generator #0 is feeding USART as well
	//
	GCLK->CLKCTRL.bit.ID = 0x19; // ID #0x19 (SERCOM5: USART): GCLK_SERCOM3_CORE
	GCLK->CLKCTRL.bit.GEN = 0; // Generator #0 selected for USART
	GCLK->CLKCTRL.bit.CLKEN = 1; // Now, clock is supplied to USART!

	GCLK->CLKCTRL.bit.ID = 0x13; // ID #0x13 (SERCOM5: USART): GCLK_SERCOM_SLOW
	GCLK->CLKCTRL.bit.GEN = 0; // Generator #0 selected for USART
	GCLK->CLKCTRL.bit.CLKEN = 1; // Now, clock is supplied to USART!
	
	//
	// USART setup
	//
	SERCOM5->USART.CTRLA.bit.MODE = 1 ; // Internal Clock
	SERCOM5->USART.CTRLA.bit.CMODE = 0 ; // Asynchronous UART
	SERCOM5->USART.CTRLA.bit.RXPO = 3 ; // PAD3
	SERCOM5->USART.CTRLA.bit.TXPO = 1 ; // PAD2
	SERCOM5->USART.CTRLB.bit.CHSIZE = 0 ; // 8-bit data
	SERCOM5->USART.CTRLA.bit.DORD = 1 ; // LSB first
	//SERCOM5->USART.CTRLA.bit.SAMPR = 1 ; //

	SERCOM5->USART.BAUD.reg = 0Xc504 ; // 115,200 bps (baud rate) with 8MHz input clock
	//SERCOM5->USART.BAUD.reg = 0Xe282 ; // 115,200 bps (baud rate) with 16MHz input clock
	//SERCOM5->USART.BAUD.reg = 0Xec57 ; // 115,200 bps (baud rate) with 24MHz input clock
	//SERCOM5->USART.BAUD.reg = 0Xf62b ; // 115,200 bps (baud rate) with 48MHz input clock

	SERCOM5->USART.CTRLB.bit.RXEN = 1 ;
	SERCOM5->USART.CTRLB.bit.TXEN = 1 ;

	SERCOM5->USART.CTRLA.bit.ENABLE = 1;
	
}
