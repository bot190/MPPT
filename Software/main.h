#ifndef MAIN_H
#define MAIN_H
/******************************************************************************
 *                    Maximum Power Point Tracker (MPPT)
 *
 * This code implements multiple algorithms for finding and tracking the
 * maximum power point of a solar panel.
 *
 * This project is in fulfillment of the Major Qualifying Project at
 * Worcester Polytechnic Institute.
 *
 * Authored by:
 * 	Johnathan Adams
 * 	Ben Beauregard
 * 	Andrew Flynn
 *
 *****************************************************************************/

// DEFINES
#define AVERAGELENGTH 8
#define AVERAGELENGTHBIT 3

#define V_SETPOINT 200	// Approximately 12V, with 750k and 45.3k resistors
#define MAX_DUTY_CYCLE 320
#define MIN_DUTY_CYCLE 0

#define MPPT_CONTROL 0x1
#define VOUT_CONTROL 0x2
#define INPUT_VOLTAGE_PRESENT 0x4
#define SWEEP_COMPLETE 0x8
#define PERTURB_DIRECTION 0x16

// Define Global Variables

extern int v_out;							// Store V-OUT from ADC
extern int v_out_samples[AVERAGELENGTH]; 	// Store V-OUT samples
extern signed char v_out_sat;				// Saturation-Flag for integral computation (part of PID)
extern long v_out_integral;				    // Value of Vout integral computation
extern const int v_out_i;			        // Integration "parameter" (constant, multiplies error in integral calculation)
extern int v_mppt;							// Store V-MPPT average from ADC
extern int v_mppt_samples[AVERAGELENGTH]; 	// Store V-MPPT samples
extern int i_mppt;							// Store I-MPPT average from ADC
extern int i_mppt_samples[AVERAGELENGTH];	// Store I-MPPT samples
extern signed char mppt_sat;				// Flag for integral computation (part of PID)
extern long mppt_integral;					// Value of MPPT integral

extern const int Divisor;	    			// Proportional Constant = 1/2^Divisor
extern unsigned int sample;				    // Counts the number of samples used for any average computation
extern unsigned char zero_samples;			// Cycle counter for handling 0V input condition
extern int mppt_duty_cycle;                 // MPPT Duty Cycle

// Variables used by MPPT methods
extern long power;

// Duty cycle control
// 0x1   - MPPT
// 0x2   - VOUT
// 0x4   - Input Voltage present?
// 0x8   - Sweep Complete (0 no, 1 yes)
// 0x16  - Perturb Direction (0 down, 1 up)
// 0x32  -
// 0x64  -
// 0x128 -
extern volatile char DCTL;

enum mppt_algorithm_type {
    MPPT_SWEEP,
    MPPT_PERTURBOBSERVE,
    MPPT_BETA
} algorithm;

int adjust_output_duty_cycle(int input, int setpoint, signed char *sat,
        long *x_integral, int Ki2, int n);

#endif /* MAIN_H */
