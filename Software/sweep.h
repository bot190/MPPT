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
// Increment duty cycle by 2/320 = 0.0625%
#define SWEEPINC 2
// Sweeping interval, calculated as Seconds / (2^16 / 12000KHz)
#define SWEEP_INTERVAL 11

// Functions
int sweep(volatile char *DCTL);
void sweep_reset(volatile char *DCTL);

// Variables
unsigned long max_power;
int max_power_duty_cycle;
char sweep_complete;

#endif /* SWEEP_H */
