/******************************************************************************
 *                    Maximum Power Point Tracker (MPPT)
 *
 * This code implements multiple algorithms for finding and tracking the
 * maximum power point of a solar panel.
 *
 * This project is in fulfillment of the Major Qualifying Project at
 * Worcester Polytechnic Institute.
 *
 * Authored by:
 * 	Jonathan Adams
 * 	Ben Beauregard
 * 	Andrew Flynn
 *
 *****************************************************************************/

#include <msp430.h>

int main(void) {

	//	Configuration & Initialization
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //Set Clock/Timer
    DCOCTL = 0b11100000;	//DCOx = 7, MODx = 0
    BCSCTL1 |= 0b1111;		//RSELx = 15
    BCSCTL1 &= 0b001111;	//DIVx = 0, clock divider = 1 (default)
    BCSCTL2 = 0b00000000;	// MCLK & SMCLK from DCO, both divided by 1, DCO resistor internal

    //Set ADC
    //Set GPIO

    //Start Timers
    //Enable ADC's

    //Enable Individual Interrupts

    BIS_SR(GIE);	//Global Interrupt Enable... I think.  Not tested.  Do all config before this line.

    //	Code Body
    while(1){
    	//This is where the call to one of the MPPT algorithms goes... maybe?
    }

	return 0;
}
