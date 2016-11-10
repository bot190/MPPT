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

// DEFINES
#define DOUT_P_CONST .1
#define V_SETPOINT 200	// Approximately 12V, with 750k and 45.3k resistors

// Define Global Variables
int v_out;	// Store V-OUT from ADC
int v_mppt;	// Store V-MPPT from ADC
int i_mppt;	// Store I-MPPT from ADC



inline void adjust_output_duty_cycle(void);

void main(void) {

	//	Configuration & Initialization
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    /*
     * Configure Clocks
     */
    // 16MHz calibrated clock
    DCOCTL = CALDCO_16MHZ;
    // Set ACLK to /2
    BCSCTL1 = (CALBC1_16MHZ);// | DIVA_1);
    // MCLK & SMCLK from DCO, both divided by 1, DCO resistor internal
    BCSCTL2 = 0b00000000;
    // Use VLOCLK for ACLK
    BCSCTL3 = LFXT1S_2;

    /*  PIN MAP
     * P1.0 - I - I-MPPT - A0
     * P1.1 - O - UNUSED
     * P1.2 - O - UNUSED
     * P1.3 - O - UNUSED
     * P1.4 - I - V-OUT - A4
     * P1.5 - I - V-MPPT - A5
     * P1.6 - O - UNUSED
     * P1.7 - O - UNUSED
     * P2.0 - O - UNUSED
     * P2.1 - O - Gate Driver MPPT MOSFET - TA1.1
     * P2.2 - O - UNUSED
     * P2.3 - O - UNUSED
     * P2.4 - O - Gate Drive V-OUT MOSFET - TA1.2
     * P2.5 - O - UNUSED
     * P2.6 - O - UNUSED
     * P2.7 - O - UNUSED
     */

    /*
     * Configure GPIO
     */
    // Set P2.1 and P2.4 to output direction
    P2DIR = (BIT1 | BIT4);
    // Set P2.1 and P2.4 to primary function
    P2SEL = (BIT1 | BIT4);
    /*
     * Configure unused pins
     */
    // Set unused P1 pins to output
    P1DIR = (BIT1 | BIT2| BIT3 | BIT6 | BIT7);
    P1OUT = (BIT1 | BIT2| BIT3 | BIT6 | BIT7); //todo Initialize correctly
    // Set unused P2 pins to output
    P2DIR |= (BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7);
    // Set all P3 pins to output
    P3DIR = 0xF;
    P3OUT = 0x0;
    P3REN = 0xF;

    /*
     * Configure ADC - Run on MCLK - 16MHz/4 = 4MHzx64 = 16uS
     */
    // Use Vcc and Vss, S&H Time: 64*ADC10CLKs, turn on ADC, enable interrupt, Enable ADC
    ADC10CTL0 = (SREF_0 | ADC10SHT_3 | ADC10ON | ADC10SR | ADC10IE);
    // Use MCLK/4
    ADC10CTL1 = (ADC10SSEL_2 | ADC10DIV_3);
    // Using A0, A4, and A5
    ADC10AE0 = (BIT0 | BIT4 | BIT5);
    // Not using the DTC

    /*
     * Configure Timer1 - SMCLK
     * PWM at 50KHz with 16MHz Clock
     * Count to 320-1
     */
    // Use SMCLK, /1 divider, Up mode
    TA1CTL = (TASSEL_2 | ID_0 | MC_1);
    // Compare 1 should reset/set TA1.1
    TA1CCTL1 = OUTMOD_7;
    // Compare 2 should reset/set TA1.2
    TA1CCTL2 = OUTMOD_7;
    // PWM frequency should be 50KHz
    TA1CCR0 = (320-1);

    /*
     * Configure Timer0 - ACLK
     */
    // Use ACLK, /8 divider, Up mode
    TA0CTL = (TASSEL_1 | ID_3 | MC_1 | TAIE);
    // 1.5KHz clock, into 1500 gives 1 second
    TA0CCR0 = (1500);

    //Global Interrupt Enable... I think.  Not tested.  Do all config before this line.
    _BIS_SR(GIE);

    //	Code Body
//    while(1) {
//    	//This is where the call to one of the MPPT algorithms goes... maybe?
//    }
}

/** \brief Timer A0 ISR that starts ADC measurements
 *
 * Function starts ADC CH2 (V-OUT) measurement when TA0 rolls over.
 * The period is controlled by TA0CCR0
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void timerA0_ISR(void)
{
	if (TA0IV == 0xA) {
		/*
		 * Configure ADC to measure V-OUT
		 */
		// Read	A4 / V-OUT
		ADC10CTL0 &= (~ENC);
		ADC10CTL1 &= 0xFFF;
		ADC10CTL1 |= INCH_4;
		// Start ADC conversion
		ADC10CTL0 |= (ENC | ADC10SC);
	}
}

/** \brief ADC10 ISR stores measurement, and kicks-off next one if necessary.
 *
 * Function copies measurement value into the respective global variable.
 * Kicks off V-MPPT measurement, then I-MPPT measurement. Also calls
 * adjust_out_duty_cycle to control D-OUT.
 */
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	switch (ADC10CTL1 >> 12) {
	// I-MPPT
	case (0x0):
		i_mppt = ADC10MEM;
		// We're done at this point
		break;
	// V-MPPT
	case (0x5):
		v_mppt = ADC10MEM;
		// Read	A0 / I-MPPT
		ADC10CTL0 &= (~ENC);
		ADC10CTL1 &= 0xFFF;
		ADC10CTL1 |= INCH_0;
		// Start ADC conversion
		ADC10CTL0 |= (ENC | ADC10SC);
		break;
	// V-OUT
	case (0x4):
		v_out = ADC10MEM;
		// Read	A5 / V-MPPT
		ADC10CTL0 &= (~ENC);
		ADC10CTL1 &= 0xFFF;
		ADC10CTL1 |= INCH_5;
		// Start ADC conversion
		ADC10CTL0 |= (ENC | ADC10SC);
		// Call function to manage D-OUT
		adjust_output_duty_cycle();
		break;
	}
}

/** \brief Adjusts the Duty Cycle of output buck converter
 *
 * Function adjusts the Duty Cycle of the output buck converter based on the
 * measured voltage and the set-point. Uses a proportional algorithm to adjust
 * duty cycle.
 */
inline void adjust_output_duty_cycle(void)
{
	TA1CCR2 += (V_SETPOINT - v_out);
	if (TA1CCR2 >= TA1CCR0) {
		TA1CCR2 = TA1CCR0-1;
	}
}
