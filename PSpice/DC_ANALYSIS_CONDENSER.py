inFile = open("\\\\storage.wpi.edu\\home\\My_Documents\\MPPT_GIT\\MPPT\\PSpice\\SOLAR_SIM_TEST.out")
outFile = open("\\\\storage.wpi.edu\\home\\My_Documents\\MPPT_GIT\\MPPT\\PSpice\\SOLAR_SIM_TEST_CONDENSED.txt", "w")

i=0
lnstart=195
lnstart2=206
for line in inFile:
    if ((i-lnstart)%20==0) and (i>=lnstart):
        print(line[:-1],end="",file=outFile)
    if ((i-lnstart2)%20==0) and (i>=lnstart2):
        print(line[:-1],file=outFile)
    i+=1

inFile.close()
outFile.close()
