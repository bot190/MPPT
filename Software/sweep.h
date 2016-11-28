#ifndef SWEEP_H
#define SWEEP_H
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

// Defines
// Increment duty cycle by 4/320 = 1.25%
#define SWEEPINC 4

// Functions
int sweep(long power, char *DCTL);

// Variables
long max_power;
int max_power_duty_cycle;
int sweep_duty_cycle;


#endif /* SWEEP_H */
