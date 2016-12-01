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

// Defines
#define BETADESIRED 1024
#define BMIN 1000
#define BMAX 1048
// #define C (q/n*K*T*Ns)
#define CCONST 1024

// Functions
int beta(volatile char *DCTL);

// Variables
int beta_step_const;
float beta_calculated;
float beta_calculated_prev;
