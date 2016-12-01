# program to generate list of times and voltages for PSpice
# output should be in form:
# + 0ms,10V 0.4999mS,10V
# + 0.5ms,7.5V 0.9999ms,7.5V
# + 1ms,5V 1.4999ms,5V 1.5ms,2.5V


# def vars
# note that time is in ms
frequency = 50000       #freqency (Hertz)
period = 1000/frequency    #length of period (m)
pstart = 1              #starting duty cycle percentage
pend = 99               #ending duty cycle percentage
pcurrent = 1            #place holder for current duty cycle value
pstep = 0.3125          #percentage to go up for each tstep 
tstart = 0              #start time (ms)
tend = 0                #end time (ms) just to inform user
tstep = 5               #time in ms between voltage changes
telap1 = 0              #elapsed time in ms
telap2 = 0              #elapsed time in ms
ttrans = 0.0001         #time in ms for transition from one voltage to next
vhigh = 3.3             #upper voltage (output high)
vlow = 0                #lower voltage (output low)
DT = (pcurrent/100)*period  #pulse width

string = "+ {0:.4f}ms,{1:.1f}V {2:.4f}ms,{3:.1f}"

f=open('\\\\storage.wpi.edu\\home\\My_Documents\\MPPT_GIT\\MPPT\\PSpice\\BRUTE_FORCE_VOLTAGE_SOURCE_320.LIB', 'w')

print("*BRUTE_FORCE_VOLTAGE_SOURCE_320", file=f)
print(".SUBCKT PWM VOUT GND", file=f)
print("VTHR VOUT GND PWL(", file=f)

while (pcurrent < pend):
    while (telap2-telap1 < tstep):
        print(string.format(telap2,vhigh,telap2+DT-ttrans,vhigh), file=f)
        print(string.format(telap2+DT,vlow,telap2+period-ttrans,vlow), file=f)
        telap2 += period
    pcurrent += pstep
    DT = (pcurrent/100)*period
    telap1=telap2

            
##    print(string.format(time,voltA,time+tstep-ttrans,voltA))
##    voltA-=0.1
##    time+=tstep

print(")", file=f)
print(".ENDS", file=f)
print("*END PWM", file=f)
f.close()
