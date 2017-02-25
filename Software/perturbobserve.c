/******************************************************************************
 *                    Maximum Power Point Tracker (MPPT)
 *
 * This file implements the perturb and observe method of finding and staying
 * at the MPPT.
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
#include "perturbobserve.h"
#include "main.h"
#include "msp430.h"

int perturb_and_observe(volatile char *DCTL) {
    power = (long)i_mppt * v_mppt;
    // Only adjust duty cycle if the difference was "significant"
    if ( (power >= (prev_power + PERTURBDEADZONE) ) ||
            (power <= (prev_power - PERTURBDEADZONE) ) ) {
        // Power decreased, lets change direction
        if (power < prev_power) {
            // Swap the direction
            direction = !direction;
        }
        if (mppt_duty_cycle == 0) {
            // If we're at zero duty cycle we need to increase the duty cycle
            direction = 1;
        } else if (mppt_duty_cycle == MAX_DUTY_CYCLE) {
            // If we're at 100% duty cycle we need to decrease the duty cycle
            direction = 0;
        }
        // Adjust duty cycle depending on direction flag
        switch (direction) {
            case 1:
                mppt_duty_cycle += PERTURBINC;
                // Turn on LED at P1.6 if we're increasing
                P1OUT |= BIT6;
                break;
            default:
                mppt_duty_cycle -= PERTURBINC;
                // Turn off LED at P1.6 if we're decreasing
                P1OUT &= ~BIT6;
                break;
        }
    }
    prev_power = power;
    return mppt_duty_cycle;
}

void perturb_and_observe_reset(void) {
    // Reset direction to up
    direction = 1;
    // Reset previous power to 0 to ensure we move upwards
    prev_power = 0;
    // Start at 25% duty cycle
    mppt_duty_cycle=80;
}
