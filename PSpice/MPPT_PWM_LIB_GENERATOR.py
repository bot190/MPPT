############################################################
# SOLAR MAX. POWER POINT TRACKER MQP
# JOHNATHAN ADAMS, BENJAMIN BEAUREGARD, ANDREW FLYNN
# ADVISOR: PROFESSOR ALEXANDER EMANUEL
# A, B, C TERM 2016-2017
# WORCESTER POLYTECHNIC INSTITUTE
#########################################################################
# DOCUMENT: MPPT_PWM_LIB_GENERATOR.py
# DATE CREATED: 11/28/16
# program to generate list of times and voltages for PSpice to define
# the entirety of a stepped pwm waveform. Defines 4 points per period.
# Also outputs a list of times and voltages corresponding to the duty
# cycle as a percentage between 0 and 100

# def vars
# note that time is in ms
frequency = 50000       	#freqency (Hertz)
period = 1000/frequency    	#length of period (ms)
pstart = 1              	#starting duty cycle percentage
pend = 99               	#ending duty cycle percentage
pcurrent = 1            	#place holder for current duty cycle value
pstep = 0.3125          	#percentage to go up for each tstep 
tstart = 0              	#start time (ms)
tstep = 5               	#time in ms between voltage changes
telap1 = 0              	#elapsed time in ms
telap2 = 0              	#elapsed time in ms
ttrans = 0.0001         	#time in ms for transition from one voltage to next
vhigh = 3.3             	#upper voltage (output high)
vlow = 0                	#lower voltage (output low)
DT = (pcurrent/100)*period  #pulse width

# string (for PWM) uses format:
# + 0ms,10V 0.4999mS,10V
# + 0.5ms,7.5V 0.9999ms,7.5V
string = "+ {0:.4f}ms,{1:.1f}V {2:.4f}ms,{3:.1f}"

# string2 (for PWM percent) uses format:
# + 0.0000ms, 1.0000V
# + 5.0200ms, 1.3125V
string2 = "+ {0:.4f}ms, {1:.4f}V"

#open files for printing to
f=open('\\\\storage.wpi.edu\\home\\My_Documents\\MPPT_GIT\\MPPT\\PSpice\\PWM_STEP_SOURCE_320.LIB', 'w')
f2=open('\\\\storage.wpi.edu\\home\\My_Documents\\MPPT_GIT\\MPPT\\PSpice\\PWM_STEP_SOURCE_320_PERCENT.LIB', 'w')

#print init lines for pwm lib
print("*PWM_STEP_SOURCE_320", file=f)
print(".SUBCKT PWM VOUT GND", file=f)
print("VTHR VOUT GND PWL(", file=f)

#print init lines for pwm percent lib
print("*PWM_STEP_SOURCE_320_PERCENT", file=f2)
print(".SUBCKT PERCENT VOUT GND", file=f2)
print("VTHR VOUT GND PWL(", file=f2)

#loop through from starting duty cycle to ending duty cycle
while (pcurrent < pend):
    print(string2.format(telap2,pcurrent),file=f2)
    while (telap2-telap1 < tstep):
        print(string.format(telap2,vhigh,telap2+DT-ttrans,vhigh), file=f)
        print(string.format(telap2+DT,vlow,telap2+period-ttrans,vlow), file=f)
        telap2 += period
    pcurrent += pstep
    DT = (pcurrent/100)*period
    telap1=telap2

#finalize and close up pwm lib file
print(")", file=f)
print(".ENDS PWM", file=f)
f.close()

#finalize and close up pwm percent file
print(")", file=f2)
print(".ENDS PERCENT", file=f2)
f2.close()