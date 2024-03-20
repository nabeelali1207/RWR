function pdwsRes=generatePDWS(inputParameters,RSS)
pdwsCount=length(inputParameters.TOA);
pdwsRes=struct();
pdwsRes.TOA=int64(inputParameters.TOA/16);
pdwsRes.PW =int16(inputParameters.PW/16);
pdwsRes.AZ =inputParameters.AOA;
pdwsRes.EL =round(inputParameters.AOA_El);

pulseGains=inputParameters.FinalPower'+getAOAGain(inputParameters.AOA,inputParameters.AOA_El);
ignoreIndexes=(pulseGains<-62);
t=sum(ignoreIndexes,1)>1;
pulseGains=2048*(3.704+(0.0539*pulseGains))*0.37;
pulseGains(:,t)=250;
pulseGains(pulseGains<250)=250;
pulseGains=int16(pulseGains);
pdwsRes.PA             = pulseGains(1,:)';
pdwsRes.PB             = pulseGains(2,:)';
pdwsRes.PC             = pulseGains(3,:)';
pdwsRes.PD             = pulseGains(4,:)';

pdwsRes.QUAD           =bi2de([~(pdwsRes.PA> pdwsRes.PC),~(pdwsRes.PB>pdwsRes.PD)]);
[~,primaryChID]=max([pdwsRes.PA,pdwsRes.PB,pdwsRes.PC,pdwsRes.PD],[],2);
pdwsRes.primaryChID    =int8(primaryChID);

pdwsRes.wow1           =zeros(pdwsCount,1,'int8');
pdwsRes.wow2           =zeros(pdwsCount,1,'int8');
pdwsRes.wow3           =zeros(pdwsCount,1,'int8');
pdwsRes.wow4           =zeros(pdwsCount,1,'int8');

bandsInfoObject = BandScaningClass();
bandsInfoObject.readDataFromFile();
[activeBands,activeAntenna]=bandsInfoObject.getActiveBandAndAntenna(inputParameters.TOA);
pdwsRes.DIFM_BAND      =activeBands;
pdwsRes.DIFM_ANT       =activeAntenna;

if(RSS==0)
    pdwsRes.RF             =inputParameters.RF;
    pdwsRes.RF_V           =ones(pdwsCount,1,'int8');
else
    
    pdwsRes.RF             =inputParameters.RF .* (activeBands==inputParameters.Band);
    pdwsRes.RF_V           =(activeBands==inputParameters.Band);
end

pdwsRes.PULSE_POP_ID   =zeros(pdwsCount,1,'int8');
pdwsRes.SEP_ERR        =zeros(pdwsCount,1,'int8');
pdwsRes.SRISE_ERR      =zeros(pdwsCount,1,'int8');
pdwsRes.SFALL_ERR      =zeros(pdwsCount,1,'int8');
pdwsRes.PW_ERR         =zeros(pdwsCount,1,'int8');
pdwsRes.TOA_ERR        =zeros(pdwsCount,1,'int8');
pdwsRes.PERCENT_ERR    =zeros(pdwsCount,1,'int8');
pdwsRes.Test_Powers    =zeros(pdwsCount,1,'int8');

pdwsRes=struct2table(pdwsRes);
[~,sortIndexes]=sort(pdwsRes.TOA);
pdwsRes=pdwsRes(sortIndexes,:);
pdwsRes=pdwsRes(~(pdwsRes.PA==250 &pdwsRes.PB==250 & pdwsRes.PC==250 & pdwsRes.PD==250),:);

pdwsTemp=pdwsRes(2:end,:);
POPIndex=((pdwsRes.TOA(1:end-1)+int64(pdwsRes.PW(1:end-1)))>=pdwsTemp.TOA);
POPIndex=[POPIndex;1==0];
POPIndex(2:end)=POPIndex(1:end-1)|POPIndex(2:end);
pdwsRes.POP_ERR        =POPIndex;
pdwsRes.RF_V           =pdwsRes.RF_V & (~POPIndex);
SRISE_ERROR=[pdwsRes.TOA(1:end-1)==pdwsRes.TOA(2:end);1==0];
pdwsRes.SRISE_ERR=SRISE_ERROR;
ignorePulses=[0==1;SRISE_ERROR(1:end-1)];

pdwsRes=pdwsRes(~ignorePulses,:);

pdwsRes=pdwsRes(pdwsRes.PW>2,:);
pdwsRes=insertMissingPulses(pdwsRes,0); % by default missingpulse count was 10, Tehseen on 4.01.23 changed it to zero
end