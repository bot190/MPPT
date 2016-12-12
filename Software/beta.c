/******************************************************************************
 *                    Maximum Power Point Tracker (MPPT)
 *
 * This file implements the beta method of finding and staying at the MPPT.
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

#include "math.h"
#include "main.h"
#include "beta.h"
#include "perturbobserve.h"

int beta(volatile char *DCTL) {
    // Store previous value
    beta_calculated_prev = beta_calculated;
    // Calculate current Beta value
    beta_calculated = logf(i_mppt/v_mppt) - CCONST * v_mppt;
    if ( (beta_calculated > BMAX) || (beta_calculated < BMIN) ){
        // If beta value far away from optimal value, calculate new duty cycle
        if ( (beta_calculated_prev < BMAX) && (beta_calculated_prev > BMIN) ){
            // Step size is adjusted based on conditions found at maximum power point
            beta_step_const = MAX_DUTY_CYCLE / (beta_calculated - BETADESIRED);
        }
        // Calculate the new duty cycle based on the previous duty cycle and error
        mppt_duty_cycle = mppt_duty_cycle + ((BETADESIRED - beta_calculated) * beta_step_const);
        // Limit duty cycle to correct range
        if (mppt_duty_cycle > MAX_DUTY_CYCLE) {
            mppt_duty_cycle = MAX_DUTY_CYCLE;
        } else if (mppt_duty_cycle < 0) {
            mppt_duty_cycle = 0;
        }
    } else {
        // Otherwise use another algorithm to reach the top
        perturb_and_observe(DCTL);
        beta_step_const = 0;
    }
	return mppt_duty_cycle;
}




