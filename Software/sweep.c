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
    // Only need to do something if sweep is not complete
    if ((*DCTL & SWEEP_COMPLETE) == 0 ) {
        power = (long) i_mppt * v_mppt;
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
    }
    return mppt_duty_cycle;
}

void sweep_reset(volatile char *DCTL) {
    max_power = 0;
    mppt_duty_cycle=80;
    *DCTL &= ~SWEEP_COMPLETE;
}
