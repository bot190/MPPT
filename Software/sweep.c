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
    power = i_mppt * v_mppt;
    // New power > old power, save duty cycle
    if (power > max_power) {
        max_power_duty_cycle = TA1CCR1;
        max_power = power;
    }
    // If we just hit 100%, mark sweep complete
    // else have we done the entire sweep yet?
    if (mppt_duty_cycle == 320) {
        *DCTL |= SWEEP_COMPLETE;
        mppt_duty_cycle = max_power_duty_cycle;
    }else if ((*DCTL & SWEEP_COMPLETE) == 0) {
        // Haven't completed sweep yet, increment duty cycle
        mppt_duty_cycle += SWEEPINC;
    }
    return mppt_duty_cycle;
}
