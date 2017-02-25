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
#define AVERAGE_LENGTH 8
#define AVERAGE_LENGTH_BIT 3

// Update frequency divider
#define ALG_FREQUENCY 50
#define MPPT_LIMITER ((12000 / LOW_FQ_CLOCK / AVERAGE_LENGTH) / ALG_FREQUENCY)
#define LOW_FQ_CLOCK 3

// Approximately 12V, with 750k and 75k resistors
#define V_SETPOINT 319
// Voltage safety margin to account for inefficiencies in ADC units
#define V_MARGIN 80
// Gives 50KHz duty cycle with 16MHz clock
#define MAX_DUTY_CYCLE 320
#define MIN_DUTY_CYCLE 0

#define MPPT_CONTROL 0x1
#define VOUT_CONTROL 0x2
#define INPUT_VOLTAGE_PRESENT 0x4

#define RESET_BUTTON_PRESSED 0x1
#define ALGORITHM_BUTTON_PRESSED 0x2

// Define Global Variables

// Store V-OUT from ADC
extern unsigned int v_out;
// Store V-OUT samples
extern int v_out_samples[AVERAGE_LENGTH];
// Saturation-Flag for integral computation (part of PID)
extern signed char v_out_sat;
// Value of Vout integral computation
extern long v_out_integral;
// Integration "parameter" (constant, multiplies error in integral calculation)
extern const int v_out_i;
// Store V-MPPT average from ADC
extern unsigned int v_mppt;
// Store V-MPPT samples
extern int v_mppt_samples[AVERAGE_LENGTH];
// Store I-MPPT average from ADC
extern unsigned int i_mppt;
// Store I-MPPT samples
extern int i_mppt_samples[AVERAGE_LENGTH];

// Proportional Constant = 1/2^Divisor
extern const int Divisor;
// Counts the number of samples used for any average computation
extern unsigned int sample;
// Cycle counter for handling 0V input condition
extern unsigned char zero_samples;
// MPPT Duty Cycle
extern int mppt_duty_cycle;

// Counter for slowing MPPT loop
unsigned int slow_down;
unsigned int periodic_timer_count;

// Variables used by MPPT methods
extern unsigned long power;

// Duty cycle control
// 0x1   - MPPT
// 0x2   - VOUT
// 0x4   - Input Voltage present?
// 0x8   -
// 0x16  -
// 0x32  -
// 0x64  -
// 0x128 -
extern volatile char DCTL;

// Track button status
// 0x1  - Reset Button
// 0x2  - Change Button
extern volatile char BUTTONS;

enum mppt_algorithm_type_enum {
    MPPT_SWEEP = 0,
    MPPT_PERTURBOBSERVE,
    MPPT_BETA
};

enum mppt_states_enum {
    MPPT_WAIT = 0,
    MPPT_LIMIT_SPEED,
    MPPT_LOW_VOLT_HANDLER,
    MPPT_LOW_VOLT_DETECTION,
    MPPT_LOW_VOLT_DETECTED,
    MPPT_AVERAGE,
    MPPT_ALGORITHM
};

enum vout_states_enum {
    VOUT_WAIT = 0,
    VOUT_LOW_VOLT_HANDLER,
    VOUT_AVERAGE,
    VOUT_ALGORITHM,
    VOUT_DISABLE,
};
int adjust_output_duty_cycle(int input, int setpoint, signed char *sat,
        long *x_integral, int Ki2, int n);

void button_handler();
void change_algorithm();
void reset_algorithm();
void call_algorithm();

#endif /* MAIN_H */
