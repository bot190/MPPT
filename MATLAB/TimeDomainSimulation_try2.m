%% MATLAB script to prove functionality of MPPT algorithm
% Authors: Andrew Flynn, Ben Beauregard, Johnathan Adams
%
% This script attempts to imulate the functionality of the maximum power
% point tracking algorithm built in C for use with the projects hardware.
% This is meant as a proof that the concept is sound and the algorithm will
% track theoretical and actual maximum power point data, not as an actual
% implementation to run on the hardware. As such, certain things are
% abstracted, such as duty cycle setting and tracking, ISRs, and other
% MCU-specific functionality.

%% Flynn's Code to load Experimental Data
% We're just calling another script here.  It reads a data file in a
% terribly... unintuitive way, but it does work, and if it ain't broke...
clear; 
fid = fopen('Full_Data.txt', 'r') ; % Open source file.
if fid == -1
    disp('ERROR 404: File Not Found') ;
else
    fgetl(fid) ;                                  % Read/discard line.
    fgetl(fid) ;                                  % Read/discard line.
    buffer = fread(fid, Inf) ;                    % Read rest of the file.
    fclose(fid);
    fid = fopen('_temp.txt', 'w')  ;   % Open destination file.
    fwrite(fid, buffer) ;                         % Save to file.
    fclose(fid) ;
    A = tdfread('_temp.txt') ;
    delete('_temp.txt') ;
end

%% Data-Error Check
% If we have more voltage samples than current samples something is wrong
% and should probably be fixed before the simulation is continued.
if length(A.Voltage_0x5BV0x5D) ~= length(A.x0x23_Current_0x5BA0x5D)
    error('Sample Size Mismatch. What are you doing?')
else
   IV = [A.Voltage_0x5BV0x5D.'; A.x0x23_Current_0x5BA0x5D.'];
end

%% Declaration of Variables
% These Variables are hereby independant from the monarch of Great Britan.
sim_time = 0; % tracks current elapsed time in simulation, us
Tsample = 1 / 500; % 500Hz MPPT sample rate, we want the period though
num_samp = length(IV); % Number of samples in performance data
delta_dc = 100 * (2 / 320); % Duty cycle increment value
duty_cycle = delta_dc; % Current duty cycle in simulation (range 0 - 100)
power = 0; % Holds current power point
mpp_dc = 0; % duty cycle at maximum power point
mpp = 0; % Power level at maximum power point
samp_idx = 1; % Array Index of current voltage/current sample
sim_data = []; % Array to store simulation results.
finished = 0; % Flag set when sweep is finished to prevent further sweeping

%% Data Resolution Error Check
% Confirm that we have enough experimental data to support the duty cycle
% increment value given to the simulation.  If not, abort, because
% simulation won't give accurate results.
if length(A.Voltage_0x5BV0x5D) < 100 / delta_dc
    error(['Not enough simulation data points to match given duty '...
        'cycle change rate.']);
end

%% Define Simulation Parameters
% These define the nature of the time-domain loop.
% This is not robust at all.  Simulation time must be evenly divisible by
% Tsample or simulation will derp.
disp(['Recommended Simulation Time (1 full sweep): ',...
    num2str(Tsample * 160 * 1e3),'ms.']);
sim_len = input('Please enter the time length of the simulation, in ms: ');
while sim_len < Tsample * 10e3
    tmpstr = ['Sim. Length too small.  Minimum length is ',...
        num2str(Tsample * 10e3),'. Please try again: '];
    sim_len = input(tmpstr);
end
sim_res = input('Please enter the time step for the simulation, in us: ');
while sim_res > Tsample * 1e6 / 2 || sim_res < .001
    tmpstr = ['Resolution invalid.  Maximum resolution is ',...
        num2str(Tsample * 1e6 / 2),...
        'us, minimum is 0.001us. Please try again: '];
    sim_res = input(tmpstr);
end

%% Time-Domain Simulation Loop
% While the simulation time is less than the requested time, continue
% simulating.
while sim_time < sim_len * 1e-3
    % Change Duty Cycle at Given Interval
    % mod(x,y) doesn't seem to give actual zero answers... derp...
    if mod(sim_time,Tsample) < 1e-12 && ~ finished
        % Increment Duty Cycle
        if duty_cycle >= 100 - delta_dc
            duty_cycle = mpp_dc;
            finished = 1; % Mark end of sweep
        else
            duty_cycle = duty_cycle + delta_dc;
        end
        % Check if MPP
        % This indexing code is a bit wat
        samp_idx = round(length(A.Voltage_0x5BV0x5D) * duty_cycle / 100);
        power = A.Voltage_0x5BV0x5D(samp_idx) *...
            A.x0x23_Current_0x5BA0x5D(samp_idx);
        if power > mpp
            mpp = power;
            mpp_dc = duty_cycle;
        end
    end
    % Data Logging
    sim_data = [sim_data;[sim_time,duty_cycle,power,...
        A.Voltage_0x5BV0x5D(samp_idx),...
        A.x0x23_Current_0x5BA0x5D(samp_idx)]]; % Rewrite this later
    % Increment simulaiton time and restart loop
    sim_time = sim_time + sim_res * 1e-6;
end

%% Graph the Simulation Results
[Ax,H1,H2] = plotyy(sim_data(:,1),sim_data(:,2),sim_data(:,1),...
    sim_data(:,3));
title('Power and Duty Cycle versus Time');
xlabel('Time, [s]');
ylabel(Ax(1),'Duty Cycle, [%]');
ylabel(Ax(2),'Power, [W]');
