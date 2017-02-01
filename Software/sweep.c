/******************************************************************************
 *                    Maximum Power Point Tracker (MPPT)
 *
 * This file implements the sweep method of finding and staying at the MPPT.
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

// Includes
#include "main.h"
#include "sweep.h"
#include "msp430.h"


int sweep(volatile char *DCTL) {
    if ((P1IN & BIT1) == 0) {
        *DCTL |= BUTTON_PRESSED;
        P1OUT |= (BIT6);
        max_power = 0;
        mppt_duty_cycle=80;
        count = 0;
    }
    else if (*DCTL & BUTTON_PRESSED) {
        if (count >= 10) {
            P1OUT &= (~BIT6);
            // Button is no longer pressed, so lets restart the sweep
            *DCTL &= ~BUTTON_PRESSED;
            *DCTL &= ~SWEEP_COMPLETE;
            count = 0;
//            mppt_duty_cycle += SWEEPINC;
        } else {
           count++;
        }
    }

    // Only need to do something if sweep is not complete
    if ((*DCTL & SWEEP_COMPLETE) == 0 ) {
        // Reduce update rate to ~20Hz
        if (SlowDown >= 25) {
            SlowDown = 0;

            power = i_mppt * v_mppt;
            // New power > old power, save duty cycle
            if (power > max_power) {
                max_power_duty_cycle = mppt_duty_cycle;
                max_power = power;
            }
            // We've reached 100% duty cycle, mark as complete,
            // Set duty cycle to maximum power point
            if (mppt_duty_cycle == 320) {
                *DCTL |= SWEEP_COMPLETE;
                mppt_duty_cycle = max_power_duty_cycle;
            } else {
                // Haven't completed sweep yet, increment duty cycle
                mppt_duty_cycle += SWEEPINC;
                if (mppt_duty_cycle % 16 == 0) {
                    // Turn on LED at P1.6 when sweeping
                    P1OUT |= (BIT6);
                } else {
                    // Turn off LED at P1.6 when done sweeping
                    P1OUT &= (~BIT6);
                }
            }
        } else {
            SlowDown++;
        }
    }
    return mppt_duty_cycle;
}
