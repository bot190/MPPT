clear; 
fid = fopen('SolarSimulator_30V_CurrentRamp.txt', 'r') ; % Open source file.
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

%Plotting part that we don't need but is useful as reference
%plot(A.Voltage_0x5BV0x5D, A.x0x23_Current_0x5BA0x5D,'-o')
%figure
%plot(A.Voltage_0x5BV0x5D, A.Voltage_0x5BV0x5D.*A.x0x23_Current_0x5BA0x5D,'-o')