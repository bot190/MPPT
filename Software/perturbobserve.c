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
    power = (long) i_mppt * v_mppt;
    // Only adjust duty cycle if the difference was "significant"
    if ((power >= (prev_power + PERTURBDEADZONE)) ||
            (power <= (prev_power - PERTURBDEADZONE) ) ) {
        // Power decreased, lets change direction
        if (power < prev_power) {
            // Swap the direction
            if (*DCTL & PERTURB_DIRECTION) {
                *DCTL &= ~(*DCTL & PERTURB_DIRECTION);
            } else {
                *DCTL |= (*DCTL & PERTURB_DIRECTION);
            }
        } else if (mppt_duty_cycle == 0) {
            // If we're at zero duty cycle we need to increase the duty cycle
            *DCTL |= (*DCTL & PERTURB_DIRECTION);
        } else if (mppt_duty_cycle == MAX_DUTY_CYCLE) {
            // If we're at 100% duty cycle we need to decrease the duty cycle
            *DCTL &= ~(*DCTL & PERTURB_DIRECTION);
        }
        // Adjust duty cycle depending on direction flag
        if (*DCTL & PERTURB_DIRECTION) {
            mppt_duty_cycle += PERTURBINC;
            // Turn on LED at P1.6 if we're increasing
            P1OUT |= (BIT6);
        } else {
            mppt_duty_cycle -= PERTURBINC;
            // Turn off LED at P1.6 if we're decreasing
            P1OUT &= (~BIT6);
        }
    }
    prev_power = power;
    return mppt_duty_cycle;
}
