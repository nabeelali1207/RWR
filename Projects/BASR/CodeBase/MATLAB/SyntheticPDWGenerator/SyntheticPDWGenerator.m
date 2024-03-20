Rows2structuresfunction();
pfile=matfile('Data/EmitterArrays.mat');
simLen          = pfile.totalTime;

SIM_STRT        = 1;
FS              = 1.6e9;
SIM_END         = FS    * simLen;
ANT_SWTIME      = 25e-3;
FILE_STEP       = 150000000;
SIM_TIMESTEP    = 3e-3;
ITERATION_STEP  = int64(SIM_TIMESTEP*FS);

%BEGIN-- Antenna And Band Switch Logic
bandSwitchingObj= BandScaningClass();
bandSwitchingObj.generate_AntVsBand_Vector(SIM_STRT,SIM_END,FS,ANT_SWTIME);
bandSwitchingObj.writeDataToFile();
%END----
EmittersArray         = pfile.EmittersArray;
ACParameters          = pfile.AirCraftParameters;
ThreatCount           = pfile.totalEmitters;
airCraftKinematics    = KinematicsClass(ACParameters.AirCraftPosition,simLen);
airCraftKinematics.calculatePositionFromSpeed(0:0.01:simLen,ACParameters.AirCraftVelocity);
threatObjects         = EmitterClass(FS,bandSwitchingObj,airCraftKinematics,EmittersArray);

inputParametersFile  = matfile('Data/Pulse_Info.mat');
PulseParameters=inputParametersFile.PulseParameters;
RSS=0;
pdws                = generatePDWS(PulseParameters,RSS);
outFileNames        = matfile('Data/OutputName.mat');
FileName            = "Case_3.91AllRF";FileName=string(outFileNames.filename);
file                = matfile("Output/"+FileName+"PulseInfo.mat",'Writable',true);
%PulseParameters.Band = PulseParameters.Band-1;

file.PulseParameters= PulseParameters;
file                = matfile("Output/"+FileName+"_sPDWs.mat",'Writable',true);
pdws.DIFM_BAND =  pdws.DIFM_BAND - 1;
file.pdws= pdws;
Conv2Csv(pdws,FileName);

close all;figure;
subplot(3,1,1);plot(double(PulseParameters.TOA)/1.6e9,PulseParameters.FinalPower,'.');    title("Received Power");
subplot(3,1,2);plot(double(PulseParameters.TOA)/1.6e9,PulseParameters.AOA,'.');           title("AOA");
subplot(3,1,3);plot(double(PulseParameters.TOA)/1.6e9,PulseParameters.Distance,'.');      title("Distance");

figure;
subplot(4,1,1);plot(double(pdws.TOA)/1e8,pdws.PA,'.');      title("Power At antenna A");
subplot(4,1,2);plot(double(pdws.TOA)/1e8,pdws.PB,'.');      title("Power At antenna B");
subplot(4,1,3);plot(double(pdws.TOA)/1e8,pdws.PC,'.');      title("Power At antenna C");
subplot(4,1,4);plot(double(pdws.TOA)/1e8,pdws.PD,'.');      title("Power At antenna D");

figure;
pow=PulseParameters.FinalPower'+getAOAGain(PulseParameters.AOA);
subplot(4,1,1);plot(double(PulseParameters.TOA)/1.6e9,pow(1,:),'.');ylim([-65 5]);      title("A");
subplot(4,1,2);plot(double(PulseParameters.TOA)/1.6e9,pow(2,:),'.');ylim([-65 5]);      title("B");
subplot(4,1,3);plot(double(PulseParameters.TOA)/1.6e9,pow(3,:),'.');ylim([-65 5]);      title("C");
subplot(4,1,4);plot(double(PulseParameters.TOA)/1.6e9,pow(4,:),'.');ylim([-65 5]);      title("D");
