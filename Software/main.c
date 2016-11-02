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
    DCOCTL = CALDCO_16MHZ;				// 16MHz calibrated clock
    BCSCTL1 = (CALBC1_16MHZ);// | DIVA_1);	// Set ACLK to /2
    BCSCTL2 = 0b00000000;				// MCLK & SMCLK from DCO, both divided by 1, DCO resistor internal
    BCSCTL3 = LFXT1S_2;					// Use VLOCLK for ACLK

    // PIN MAP
    // P1.0 - I - Current Sense - A0
    // P1.1 - I - MPPT Voltage - A1
    // P1.2 - I - Vout- A2
    // P1.3 - O - UNUSED
    // P1.4 - O - UNUSED
    // P1.5 - O - UNUSED
    // P1.6 - O - UNUSED
    // P1.7 - O - UNUSED
    // P2.0 - O - UNUSED
    // P2.1 - O - Gate Driver MPPT MOSFET - TA1.1
    // P2.2 - O - UNUSED
    // P2.3 - O - UNUSED
    // P2.4 - O - Gate Drive 2nd MOSFET - TA1.2
    // P2.5 - O - UNUSED
    // P2.6 - O - UNUSED
    // P2.7 - O - UNUSED

    //Configure GPIO
    P2DIR = (BIT1 | BIT4);		// Set P2.1 and P2.4 to output direction
    P2SEL = (BIT1 | BIT4);		// Set P2.1 and P2.4 to primary function

    // Configure unused pins
    P1DIR = (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
//    P1OUT = (BIT3 | BIT4 | BIT5 | BIT6 | BIT7); //todo Initialize correctly
    P2DIR |= (BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7);
    P3DIR = 0xF;
    P3OUT = 0x0;
    P3REN = 0xF;

    //Configure ADC - Run on MCLK - 16MHz/4 = 4MHzx64 = 16uS
    ADC10CTL0 = (SREF_0 | ADC10SHT_3 | ADC10ON | ADC10IE);		// Use Vcc and Vss, S&H Time: 64*ADC10CLKs, turn on ADC, enable interrupt
    ADC10CTL1 = (ADC10SSEL_2 | ADC10DIV_3);						// Use MCLK/4
    ADC10AE0 = (BIT0 | BIT1 | BIT2);							// Using A0, A1, and A2
    // Not using the DTC

    // Configure Timer0 - SMCLK
    // PWM at 50KHz with 16MHz Clock
    // Count to 320-1
    TA1CTL = (TASSEL_2 | ID_0 | MC_1); 							// Use SMCLK, /1 divider, Up mode
    TA1CCTL1 = OUTMOD_7;											// Compare 1 should reset/set TA1.1
    TA1CCTL2 = OUTMOD_7;											// Compare 2 should reset/set TA1.2
    TA1CCR0 = (320-1);											// PWM frequency should be 50KHz

    // Configure Timer1 - ACLK
    TA0CTL = (TASSEL_1 | ID_3 | MC_1 | TAIE);					// Use ACLK, /8 divider, Up mode
    TA0CCR0 = (1500);											// 1.5KHz clock, into 1500 gives 1 second

    _BIS_SR(GIE);	//Global Interrupt Enable... I think.  Not tested.  Do all config before this line.

    //	Code Body
//    while(1){
    	//This is where the call to one of the MPPT algorithms goes... maybe?
//    }
	return 0;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void timerA0Interrupt(void)
{
	if (TA0IV == 0xA) {
		// Configure ADC to measure Vout
		ADC10CTL1 |= INCH_2;			// Read	A2
		ADC10CTL0 |= (ENC | ADC10SC);	// Enable ADC and start conversion
	}
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10ISR(void)
{
	switch (ADC10CTL1 >> 12) {
	case (0x0):

		break;
	case (0x1):

		break;
	case (0x2):

		break;
	}
}
