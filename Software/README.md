# Timer A0 Interrupt (1KHz)
    Configure ADC for first input
    Start ADC
    
# ADC Interrupt
    IF (Vout){
        Config ADC for V-MPPT
        Start ADC
        Adjust Dout
    } else if (V-MPPT) {
       Config ADC for I-MPPT
       Start ADC
    } else if (I-MPPT) {
        Run MPPT Algorithm
    }
    Maintain a rolling average of 3 samples for all measurements
    
# Dout Adjust
    // Monitor Vout and adjust Dout as necessary
    Dout = X * (VSet - Vout)
    
# MPPT Algorithm Using V-MPPT and I-MPPT, adjust D-MPPT
    // Sweep, P&O, Beta
    

    
    