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
#define SWEEPINC 2

// Functions
int sweep(volatile char *DCTL);

// Variables
unsigned long max_power;
int max_power_duty_cycle;
int count;

#endif /* SWEEP_H */
