function [Complete]=Conv2Csv(pdws,varargin)
disp('PDWs Formation Started');
%% Initialiazation
len =length(pdws.PA);
% pa = []; pb = []; pc = []; pd = [];
% nd = []; N = [];PDW = []; px = [];
% py = []; rfv = []; pop = [];
% ant = [];band = [];
% lpi =[]; lpiv=[];
AMP=[];
File_Names=matfile('Data\OutputName.mat','Writable',true);

if(length(varargin)==1)
    Filename=varargin{1};
else
    Filename=File_Names.filename;
end

%% Obtaining Data from PDWs
pa= ceil([pdws.PA]);    %Antenna A Information
pb= ceil([pdws.PB]);    %Antenna B Information
pc= ceil([pdws.PC]);    %Antenna C Information
pd= ceil([pdws.PD]);    %Antenna D Information
px=max(pa,pc);
py=max(pb,pd);
AMP=max(px,py);
toa=([pdws.TOA]);       %toa information
ov=zeros(len,1,'single');                  %toa overflow flag information
rf=ceil(([pdws.RF]) .*10e-7);         %RF information
rfv=([pdws.RF_V]);      %RF valid flag information
pw=([pdws.PW]);         %pw information
band = ([pdws.DIFM_BAND]-1);   %band code information by difm
N=([pdws.QUAD]);          %quadrant number used to calculate AOA
lpi=zeros(len,1,'single');            %lpi Information
lpiv=zeros(len,1,'single');          %lpi valid flag Information
ant=([pdws.DIFM_ANT]);  %infromation for difm antenna
pop = ([pdws.POP_ERR]); %pulse on pulse 
pulse_pop_id = ([pdws.PULSE_POP_ID]);
sep_err=([pdws.SEP_ERR]);
Srise_err=([pdws.SRISE_ERR]);
Sfall_err=([pdws.SFALL_ERR]);
pw_err=([pdws.PW_ERR]);
toa_err=([pdws.TOA_ERR]);
pop_err=([pdws.POP_ERR]);
percent_err=([pdws.PERCENT_ERR]);

%% Decimal To Hex Conversion
% PA = dec2hex (pa,4);
% PB = dec2hex (pb,4);
% PC = dec2hex (pc,4);
% PD = dec2hex (pd,4);
% TOA = dec2hex(toa,16);
% OV=dec2hex(ov,2);
% RF = dec2hex(rf,4);
% RFV = dec2hex (rfv,2);
% PW= dec2hex (pw,4);
% BCODE= dec2hex (band,2);
% NQuad= dec2hex (N,2);
% ANT= dec2hex (ant,2);
% LPI= dec2hex (lpi,2);
% LPIV=dec2hex (lpiv,2);
% POP=dec2hex (pop,2);
% PULSE_POP_ID=dec2hex(pulse_pop_id,1);
% SEP_ERR=dec2hex(sep_err,1);
% SRISE_ERR=dec2hex(Srise_err,1);
% SFALL_ERR=dec2hex(Sfall_err,1);
% PW_ERR=dec2hex(pw_err,1);
% TOA_ERR=dec2hex(toa_err,1);
% POP_ERR=dec2hex(pop_err,1);
% PERCENT_ERR=dec2hex(percent_err,1);

%% String Conversion For .csv File 
% PA = num2str (PA);
% PB = num2str (PB);
% PC = num2str (PC);
% PD = num2str (PD);
% TOA = num2str(TOA);
% OV=num2str(OV);
% RF = num2str (RF);
% RFV = num2str (RFV);
% PW= num2str (PW);
% BCODE= num2str (BCODE);
% NQuad= num2str (NQuad);
% ANT= num2str (ANT);
% LPI= num2str (LPI);
% LPIV=num2str(LPIV);
% % RES= num2str(RES);
% POP=num2str(POP);
% PULSE_POP_ID=num2str(PULSE_POP_ID);
% SEP_ERR=num2str(SEP_ERR);
% SRISE_ERR=num2str(SRISE_ERR);
% SFALL_ERR=num2str(SFALL_ERR);
% PW_ERR=num2str(PW_ERR);
% TOA_ERR=num2str(TOA_ERR);
% POP_ERR=num2str(POP_ERR);
% PERCENT_ERR=num2str(PERCENT_ERR);

%% PDWs Storage
% CSVFilename=strcat('Output/',Filename,'.csv');
MFilename=strcat('Output/',Filename,'.mat');
PDWSFilename=strcat('Output\',Filename,'_sPDW.mat');
% file                = matfile(PDWSFilename,'Writable',true);
% file.pdws= pdws;
% outputFile = fopen(CSVFilename, 'w+');
% M = [PERCENT_ERR POP_ERR TOA_ERR PW_ERR SFALL_ERR SRISE_ERR SEP_ERR PULSE_POP_ID,LPIV,LPI,POP,ANT,NQuad,BCODE,OV,RFV,PW,RF,TOA,PD,PC,PB,PA];
% for k = 1:len
%       switch N(k)
% 	  case 0
% 	  px(k)= pa(k); %#ok<*AGROW>
% 	  py(k)= pb(k);
% 	  case 1
% 	  px(k)= pc(k);
% 	  py(k)= pd(k);
% 	  case 2
% 	  px(k)= pa(k);
% 	  py(k)= pd(k);
% 	  case 3
% 	  px(k)= pc(k);
% 	  py(k)= pd(k);
%       end
% AMP(k,:)=px(k)+py(k);
% fprintf(outputFile,'%s\n', M(k,:));
% end
% 
% 
% disp('PDWs CSV Formation Completed')
% fclose(outputFile);
% Srise_err = Srise_err'; Sfall_err=Sfall_err'; sep_err=sep_err';toa_err=toa_err';percent_err=percent_err';
% pop_err=pop_err';pop=pop';pulse_pop_id=pulse_pop_id';
AOA = zeros(len,1,'single');
PDW = [double(AOA) double(AMP) double(pw) double(toa)];
% px=px'; py=py';   %#ok<*NASGU>
rf = double(rf);
rfv = double(rfv);
pa = double(pa);
pb = double(pb);
pc = double(pc);
pd = double(pd);
px = double(px);
py = double(py);
ant = double(ant);
band = double(band);
lpi = double(lpi);
lpiv = double(lpiv);
N = double(N);
percent_err = double(percent_err);
pop = double(pop);
pop_err = double(pop_err);
pulse_pop_id = double(pulse_pop_id);
pw_err = double(pw_err);
sep_err = double(sep_err);

Sfall_err = double(Sfall_err);
Srise_err = double(Srise_err);
toa_err = double(toa_err);
% ant = ant';
% band = band';
% N =N';
% pa = pa';
% pb= pb';
% pc = pc';
% pd = pd';
% percent_err = percent_err';
% pop = pop';
% pop_err =pop_err';
% pulse_pop_id = pulse_pop_id';
% pw_err = pw_err';
% rf = rf';
% rfv = rfv';
save(MFilename, 'ant','PDW','band','px','py','pa','pb','pc','pd','N','rfv','rf','pop','lpi','lpiv', 'pulse_pop_id', 'sep_err', 'Srise_err', 'Sfall_err', 'pw_err', 'toa_err', 'pop_err', 'percent_err');
end