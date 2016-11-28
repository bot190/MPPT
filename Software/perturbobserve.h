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

// Defines
// Perturb duty cycle by 4/320 = 1.25%
#define PERTURBINC 4
#define PERTURBDEADZONE 5

// Functions
int perturb_and_observe(long power, char *DCTL);

// Global Variables
long prev_power;
int perturb_observe_duty_cycle;
