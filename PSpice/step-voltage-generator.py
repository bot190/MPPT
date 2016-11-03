# program to generate list of times and voltages for PSpice
# output should be in form:
# + 0ms,10V 0.4999mS,10V
# + 0.5ms,7.5V 0.9999ms,7.5V
# + 1ms,5V 1.4999ms,5V 1.5ms,2.5V


# def vars
# note that time is in ms
time = 0                #start time
tstep = 5               #time in ms between voltage changes
ttrans = 0.001          #time in ms for transition from one voltage to next
voltA = 10              #starting voltage (10v = 0%, 0v = 100%)

string = "+ {0}ms,{1:.1f}V {2}ms,{3:.1f}"

while (voltA >= 0):
    print(string.format(time,voltA,time+tstep-ttrans,voltA))
    voltA-=0.1
    time+=tstep
