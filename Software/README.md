# Timer A0 Interrupt (4KHz)
    Configure ADC for first input
    Start ADC
    
# ADC Interrupt
    IF (Vout){
    	Add measurement to sample array
        Config ADC for V-MPPT
		Start ADC
        Enable flag to Adjust Dout
    } else if (V-MPPT) {
    	Add measurement to sample array
    	Config ADC for I-MPPT
       	Start ADC
    } else if (I-MPPT) {
    	Add measurement to sample array
       	Enable flag to Run MPPT Algorithm
       	Increment sample counter
       		Roll over at Sample Size
    }
    
If MPPT Buck doesn't see voltage when duty cycle is increased, then probably no input.
Shut off Output buck AND shut off MPPT buck. Periodically test this, turning
MPPT buck on, and leaving out buck off 
    
# Dout Adjust
    // Monitor Vout and adjust Dout as necessary
    Calulate error (setpoint - value)
    Integrate error (integrator += Ki * error)
    
    
# MPPT Algorithms

## Sweep
1. Start at 0% Duty Cycle - Ramp to 100% Duty cycle
 Calculate Power at each point update max_power_duty_cycle
2. After entire range is swept, go to max_pwer_duty_cycle

## Peturb and Observe
1. Adjust duty cycle based on perturb_dir
 Set prev_power to current_power
 Calculate current_power from current & voltage readings
2. If current_power < prev_power, reverse perturb_dir
3. Repeat forever

## Beta Method
1. Measure Voltage & Current, calculate B = ln (i/v) - C * v
2. If B is within Bmin & Bmax
 Switch to Perturb & Observe
3. Else adjust duty cycle based on B
 (Duty Cycle = D-Old + K * Bg-B)

    

    