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
fid = fopen('SolarSweepData.txt', 'r') ; % Open source file.
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

%% Ben's MPPT Algorithm
% The "Sweep" Algorithm runs continuously in a loop using ISR's that call
% eachother.

%% Simulation Parameters
% User-Modifiable to change how the simulation is run, and how fast the
% algorithm itself runs. 

delta_dc = 0.625;           % Percentage change in duty cycle
req_samp = 100 / delta_dc;  % Required samples for accurate sim.
sample_rate = 500;          % Algorithm sample rate, Hz.
sim_time = 100;             % Simultion time, in milliseconds (ms)
sim_res = 100;              % Simulation resolution in microseconds (us)

%% Simulation Parameter Error Check
% If you can customize it, someone will break it.  Let them know how they
% did so, so they can fix it.

% Warn user if duty cycle change won't always use a new sample
if length(IV) < req_samp
    disp('Simultion Resolution Low. Results Not Accurate.');
end
% Warn user if simulation time is too short to show full sweep
if sim_time < req_samp
    disp('Sim time too short to show full sweep.');
end
% ERROR if time step is larger than sweep time step
if 1E6 / sample_rate < sim_res
    error('Simulation Resolution too Large for Sample Rate.');
end

%% These variables shouldn't be modified by the user
duty_cycle = delta_dc;      % Holds the current duty cycle
time_count = 0;             % Holds the current time during simulation
max_power = 0;              % Holds Max Power Value
max_power_dc = 0;           % Emulates Duty-Cycle tracking
power = 0;                  % Holds power value for 
ind = 1;                    % Matrix index for logging
current_sample = 0;         % Current Sample Variable for readibility
voltage_sample = 0;         % Voltage Sample Variable for readibility
% Matrix to hold time-domain result of simulation
pts = sim_time * 1E-3 / sim_res * 1E-6;
sim_result = zeros(6,pts);  % [Time;DC;Power;Voltage;Current;MaxPower]

%% Algorithm Body
% This is the sweep algorithm.  It just sweeps.  It's the definition of
% terrible, but basically everything is based on it.
for i = 0:sim_res * 1E-6:sim_time * 1E-3
    % Change duty cycle at the designated sample rate, otherwise wait
    if mod(i,1 / sample_rate) == 0
        % Avoiding short and open circuit conditions via duty cycle limits
        if duty_cycle < 100 - delta_dc;
            duty_cycle = duty_cycle + delta_dc;
        else
            duty_cycle = delta_dc;
            max_power = 0;
            max_power_dc = 0;
        end
    end
    
    % Use duty cycle to compute current, voltage and power
    % Calculate sample index from duty cycle because we can't just take an
    % ADC reading in a simulation...
    ind = round(duty_cycle * length(IV) / 100);
    
    % Ensure ind stays within its bounds
    if ind > length(IV)
        ind = length(IV);
    elseif ind < 1
        ind = 1;
    end
    
    % Read in samples from ADC
    current_sample = IV(2,ind);
    voltage_sample = IV(1,ind);
    power = current_sample * voltage_sample;
    
    % If the current sample is higher than the previous maximum power
    % point, save it.
    if power > max_power
        max_power = power;
        max_power_dc = duty_cycle;
    end
    
    % Log the simulation results and increment the index
    sim_result(1,ind) = i;  % time
    sim_result(2,ind) = duty_cycle;
    sim_result(3,ind) = power;
    sim_result(4,ind) = voltage_sample;
    sim_result(5,ind) = current_sample;
    sim_result(6,ind) = max_power;
    ind = ind + 1;
end


%% Graph data
% This is a wall of nonsense that I promise you works just fine and you
% don't have to waste your time reading.

% Here, we display consice numerical results.  The wierdness comes from
% trying to stay under 80 characters per line.
disp(['The calculated maximum power was ',num2str(max_power)]);
str = 'The theoretical duty cycle is ';
st = ' percent';
disp([str,num2str(max_power_dc),st]);

plot(sim_result(1),sim_result(2));

% Too complex half-shared axis multiple y-scale plotting
% Mostly because of MATLAB 2015... (As opposed to 2016)
% x = A.Voltage_0x5BV0x5D;
% y1 = A.x0x23_Current_0x5BA0x5D;
% y2 = A.Voltage_0x5BV0x5D.*A.x0x23_Current_0x5BA0x5D;
% [hAx,hLine1,hLine2] = plotyy(x,y1,x,y2);
% title('Power, Current and Voltage Characteristics')
% xlabel('Voltage, [V]')
% ylabel(hAx(1),'Current, [A]') % left y-axis
% ylabel(hAx(2),'Power, [W]') % right y-axis
% hLine1.LineStyle = '-';
% hLine2.LineStyle = '-';
% hold on;
% v = IV(1,max_power_dc);
% plot([v v], [0 max_power],'Color',[0 1 0]);
% grid on;
% grid minor;

