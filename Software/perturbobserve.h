#ifndef PERTURBOBSERVE_H
#define PERTURBOBSERVE_H
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
// Perturb duty cycle by 2/320 = 0.0625%
#define PERTURBINC 4
#define PERTURBDEADZONE 0

// Functions
int perturb_and_observe(volatile char *DCTL);
void perturb_and_observe_reset(void);

// Global Variables
unsigned long prev_power;
unsigned char direction;

#endif /* PERTURBOBSERVE_H */
