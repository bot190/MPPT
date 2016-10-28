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
 * 	Johnathan Adams
 * 	Ben Beauregard
 * 	Andrew Flynn
 *
 *****************************************************************************/

#include <msp430.h>

int main(void) {

	//	Configuration & Initialization
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //Set Clock/Timer
    DCOCTL = CALDCO_16MHZ;	// 16MHz calibrated clock
    BCSCTL1 = CALBC1_16MHZ;
    BCSCTL2 = 0b00000000;	// MCLK & SMCLK from DCO, both divided by 1, DCO resistor internal

    //Set ADC
    //Set GPIO

    //Start Timers
    //Enable ADC's

    //Enable Individual Interrupts

    //_BIS_SR(GIE);	//Global Interrupt Enable... I think.  Not tested.  Do all config before this line.

    //	Code Body
//    while(1){
    	//This is where the call to one of the MPPT algorithms goes... maybe?
//    }

	P1DIR |= BIT4;					// Set P1.0 and P1.4 to output direction
	P1SEL |= BIT4;					// Set P1.4


	return 0;
}
