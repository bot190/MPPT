# MPPT
Maximum Power Point Tracker (MPPT)

Oh look an MQP.

Software: Use CCS: [Download TI CCS](https://www.ti.com/licreg/docs/swlicexportcontrol.tsp?form_type=2&prod_no=ccs_setup_win32.exe&ref_url=http://software-dl.ti.com/ccs/esd/CCSv6/latest/) or on Hutt, or any lab computer.

This is a copy of the git repository used during the course of the project.  Contents are organized by folder and should be fairly self-explanatory.  

Board_Design contains all of the schematics and layout diagrams used for the design of the PCB. These will require NI Multisim 13 and/or NI Ultiboard 13 or later to open.

MATLAB contains matlab code used for the project, mostly for the simulation of software algorithms.  MATLAB 2015b or later should be sufficient.

PSpice contains the SPICE simulations of the circuits. These simulations were run with OrCad PSpice, but can in theory run in any SPICE netlist simulator.

Software is a TI Code Composer Studio workspace.  It includes the "Sweep" algorithm, the "Perturb and Observe" algorithm, and the "Beta" algorithm.

Any Python code was made in Python 3.
