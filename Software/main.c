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

    //Configure Clocks
    DCOCTL = CALDCO_16MHZ;	// 16MHz calibrated clock
    BCSCTL1 = CALBC1_16MHZ;
    BCSCTL2 = 0b00000000;	// MCLK & SMCLK from DCO, both divided by 1, DCO resistor internal

    // PIN MAP
    // P1.0 - I - Current Sense
    // P1.1 - O - Gate Driver MPPT MOSFET
    // P1.2 - O - Gate Drive 2nd MOSFET
    // P1.3 - I - MPPT Voltage
    // P1.4 - I - Output Voltage
    // P1.5 - O - UNUSED
    // P1.6 - O - UNUSED
    // P1.7 - O - UNUSED
    // P2.6 - O - UNUSED
    // P2.7 - O - UNUSED

    //Configure GPIO
    P1DIR = (BIT1 | BIT2);					// Set P1.1 and P1.2 to output direction
    P1SEL = (BIT1 | BIT2);					// Set P1.1 and P1.2 to primary function

    // Configure unused pins
    P1DIR |= (BIT5 | BIT6 | BIT7);
    P1OUT |= (BIT5 | BIT6 | BIT7);
    P2DIR = (BIT6 | BIT7);
    P2OUT = 0x00;

    //Configure ADC - Run on MCLK - 16MHz/4 = 4MHzx64 = 16uS
    ADC10CTL0 = (SREF_0 | ADC10SHT_3 | ADC10ON | ADC10IE);		// Use Vcc and Vss, S&H Time: 64*ADC10CLKs, turn on ADC, enable interrupt
    ADC10CTL1 = (ADC10SSEL_2 | ADC10DIV_3);						// Use MCLK/4
    // Not using the DTC

    //Configure Timers - SMCLK

    //Enable ADC's

    //Enable Individual Interrupts

    _BIS_SR(GIE);	//Global Interrupt Enable... I think.  Not tested.  Do all config before this line.

    //	Code Body
//    while(1){
    	//This is where the call to one of the MPPT algorithms goes... maybe?
//    }



	return 0;
}
