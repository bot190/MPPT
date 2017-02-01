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

// Includes
#include <msp430.h>
#include <limits.h>
#include "main.h"
#include "sweep.h"
#include "beta.h"
#include "perturbobserve.h"

// Define Global Variables

// Store V-OUT from ADC
int v_out;
// Store V-OUT samples
int v_out_samples[AVERAGELENGTH];
// Saturation-Flag for integral computation (part of PID)
signed char v_out_sat;
// Value of Vout integral computation
long v_out_integral;
// Integration "parameter" (constant, multiplies error in integral calculation)
const int v_out_i = 1024;
// Store V-MPPT average from ADC
int v_mppt;
// Store V-MPPT samples
int v_mppt_samples[AVERAGELENGTH];
// Store I-MPPT average from ADC
int i_mppt;
// Store I-MPPT samples
int i_mppt_samples[AVERAGELENGTH];


// Proportional Constant = 1/2^Divisor
const int Divisor = 3;
// Counts the number of samples used for any average computation
unsigned int sample;
// Cycle counter for handling 0V input condition
unsigned char zero_samples;
// MPPT Duty Cycle - Start at 25%
int mppt_duty_cycle=80;

//Bitmask.  Moonorails.  (see header).
volatile char DCTL;

// Calculated power draw from MPPT Buck Converter
unsigned long power;

// Define algorithm variable to choose MPPT algorithm
enum mppt_algorithm_type algorithm = MPPT_SWEEP;

void main(void) {

    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    /*
     * Configure Clocks
     */
    // 16MHz calibrated clock
    DCOCTL = CALDCO_16MHZ;
    // Set ACLK to /2
    BCSCTL1 = (CALBC1_16MHZ);
    // MCLK & SMCLK from DCO, both divided by 1, DCO resistor internal
    BCSCTL2 = 0b00000000;
    // Use VLOCLK for ACLK
    BCSCTL3 = LFXT1S_2;

    /*  PIN MAP
     * P1.0 - I - I-MPPT - A0
     * P1.1 - O - Algorithm Reset Button
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
    P1DIR = (BIT2 | BIT3 | BIT6 | BIT7);
    // Enable pull-up resistor for P1.1
    P1REN = (BIT1);
    // Setting BIT1 uses the Pull-Up resistor instead of the pull down resistor
    P1OUT = (BIT1 | BIT2 | BIT3 | BIT6 | BIT7);
    // Set unused P2 pins to output
    P2DIR |= (BIT0 | BIT2 | BIT3 | BIT5 | BIT6 | BIT7);
    // Set all P3 pins to output
    P3DIR = 0xF;
    P3OUT = 0x0;
    P3REN = 0xF;

    /*
     * Configure ADC - Run on MCLK - 16MHz/4 = 4MHzx64 = 16uS
     */
    // Use Vcc and Vss, S&H Time: 64*ADC10CLKs, turn on ADC, enable interrupt
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
    TA1CCR0 = (MAX_DUTY_CYCLE - 1);

    /*
     * Configure Timer0 - ACLK
     */
    // Use ACLK, /1 divider, Up mode
    TA0CTL = (TASSEL_1 | ID_0 | MC_1 | TAIE);
    // 12KHz clock, into 3 gives 4KHz
    TA0CCR0 = (3);

    //Global Interrupt Enable
    _BIS_SR(GIE);

    // Iterator variable because this isn't c99 apparently
    unsigned int i;

    // Code Body
    while (1) {
        // Should we adjust the MPPT duty cycle this loop?
        if (DCTL & (MPPT_CONTROL)) {
            // Mark that this is complete
            DCTL = DCTL & (~MPPT_CONTROL);
            if (DCTL & INPUT_VOLTAGE_PRESENT) {
                // Get average current and voltage
                i_mppt = v_mppt = 0;
                for (i = AVERAGELENGTH; i > 0; i--) {
                    // Calculate average I-MPPT
                    i_mppt += i_mppt_samples[i - 1];
                    // Calculate average V-MPPT
                    v_mppt += v_mppt_samples[i - 1];
                }
                i_mppt = i_mppt >> AVERAGELENGTHBIT;
                v_mppt = v_mppt >> AVERAGELENGTHBIT;
                /* HANDLE ZERO INPUT VOLTAGE */
                if (v_mppt < 15) {
                    if (zero_samples >= 20) {
                        zero_samples = 0;
                        // Not detecting a voltage
                        DCTL = DCTL & (~INPUT_VOLTAGE_PRESENT);
                        // Shut off VOUT buck converter for now
                        TA1CCR2 = 0;
                    } else {
                        zero_samples++;
                    }
                }
                /* HANDLE ZERO INPUT VOLTAGE */
                // Run MPPT algorithm (Set duty cycle - TA1CCR1)
                switch (algorithm) {
                    case MPPT_SWEEP:
                        TA1CCR1 = sweep(&DCTL);
                        break;
                    case MPPT_PERTURBOBSERVE:
                        TA1CCR1 = perturb_and_observe(&DCTL);
                        break;
                    case MPPT_BETA:
                        TA1CCR1 = beta(&DCTL);
                        break;
                    case DEFAULT:
                        break;
                }
            }
            /* HANDLE ZERO INPUT VOLTAGE */
            else {
                zero_samples++;
                if (zero_samples >= 100) {
                    DCTL |= INPUT_VOLTAGE_PRESENT;
                    // Zero integrals to avoid unnecessarily integrated error
                    v_out_integral = 0;
                    zero_samples = 0;
                }
            }
            /* HANDLE ZERO INPUT VOLTAGE */
        }
        // Should we adjust the output duty cycle this loop?
        if (DCTL & VOUT_CONTROL) {
            // Mark that this is complete
            DCTL = DCTL & (~VOUT_CONTROL);
            if (DCTL & INPUT_VOLTAGE_PRESENT) {
                v_out = 0;
                for (i = AVERAGELENGTH; i > 0; i--) {
                    v_out += v_out_samples[i - 1];
                }
                v_out = v_out >> AVERAGELENGTHBIT;

                // Run Vout Control algorithm
                TA1CCR2 += adjust_output_duty_cycle(v_out, V_SETPOINT,
                        &v_out_sat, &v_out_integral, v_out_i, Divisor);
                if (TA1CCR2 >= MAX_DUTY_CYCLE) {
                    TA1CCR2 = MAX_DUTY_CYCLE;
                }
            }
        }
    }
}

/** \brief Timer A0 ISR that starts ADC measurements
 *
 * Function starts ADC CH2 (V-OUT) measurement when TA0 rolls over.
 * The period is controlled by TA0CCR0
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void timerA0_ISR(void) {
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
__interrupt void ADC10_ISR(void) {
    switch (ADC10CTL1 >> 12) {
        // I-MPPT
        case (0x0):
            i_mppt_samples[sample] = ADC10MEM;
            // Only update Duty cycle at 500Hz
            if (sample == 0) {
                DCTL |= MPPT_CONTROL;
            }
            // Increment sample count, roll over at 3
            sample++;
            if (sample == AVERAGELENGTH) {
                sample = 0;
            }
            break;
        // V-MPPT
        case (0x5):
            v_mppt_samples[sample] = ADC10MEM;
            // Read	A0 / I-MPPT
            ADC10CTL0 &= (~ENC);
            ADC10CTL1 &= 0xFFF;
            ADC10CTL1 |= INCH_0;
            // Start ADC conversion
            ADC10CTL0 |= (ENC | ADC10SC);
            break;
        // V-OUT
        case (0x4):
            v_out_samples[sample] = ADC10MEM;
            // Read	A5 / V-MPPT
            ADC10CTL0 &= (~ENC);
            ADC10CTL1 &= 0xFFF;
            ADC10CTL1 |= INCH_5;
            // Start ADC conversion
            ADC10CTL0 |= (ENC | ADC10SC);
            // Enable D-OUT algorithm at 500Hz
            if (sample == 0) {
                DCTL |= VOUT_CONTROL;
            }
            break;
    }
}

/** \brief Adjusts the Duty Cycle of output buck converter
 *
 * Function adjusts the Duty Cycle of the output buck converter based on the
 * measured voltage and the set-point. Uses a proportional algorithm to adjust
 * duty cycle.
 */
int adjust_output_duty_cycle(int input, int setpoint, signed char *sat,
        long *x_integral, int Ki2, int n) {
    int e = setpoint - input;
    int x;

    /* If there isn't saturation, or there is,
     * but saturation is the opposite direction from the error
     */
    if (! ((*sat < 0 && e < 0) || (*sat > 0 && e > 0))) {
        *x_integral = *x_integral + (long) Ki2 * e;
        // Keep integral within range
        if (*x_integral > LONG_MAX) {
            *x_integral = LONG_MAX;
            *sat = 1;
        } else if (*x_integral < LONG_MIN) {
            *x_integral = LONG_MIN;
            *sat = -1;
        } else {
            *sat = 0;
        }
        x = (e >> n) + (int) (*x_integral >> 16);
    }
    return x;
}
