function [finalNormSig,finalDeldSig,finalVideSig]=pulseSignalGenerator(sinAmp,pulseAmp,PRIVector,PWVector,RFVector,LOVector,FS)
%-----------------------video filter
N     = 2;         % Order
Fpass = 300000000;  % Passband Frequency
FStop = 400000000;  % Stopband Frequency
Wpass = 1;          % Passband Weight
Wstop = 1;          % Stopband Weight
vidFilter  = firls(N, [0,Fpass,FStop,double(FS)/2]./(double(FS)/2), [1 1 0 0], [Wpass Wstop]);
Hd = dsp.FIRFilter('Numerator', vidFilter);
[vFilterB,vFilterA]=tf(Hd);
%----------------------------------

finalSigLen=sum(PRIVector);
finalNormSig=zeros(1,finalSigLen,'single');
finalDeldSig=zeros(1,finalSigLen,'single');
finalVideSig=zeros(1,finalSigLen,'single');

if(sum(abs(diff(RFVector)))==0)
    [normalSig,delaydSig]=generateSinglePulse(sinAmp,max(PWVector),RFVector(1),LOVector(1),FS);
    currIndex=0;
    for ii=1:length(PRIVector)
        finalNormSig(1,currIndex+1:currIndex+PWVector(1,ii))=normalSig(1:PWVector(1,ii));
        finalDeldSig(1,currIndex+1:currIndex+PWVector(1,ii))=delaydSig(1:PWVector(1,ii));
        finalVideSig(1,currIndex+1:currIndex+PWVector(1,ii))=pulseAmp;
        currIndex=currIndex+PRIVector(ii);
    end
else
    currIndex=0;
    for ii=1:length(PRIVector)
        [normalSig,delaydSig]=generateSinglePulse(sinAmp,PWVector(ii),RFVector(ii),LOVector(ii),FS);
        finalNormSig(1,currIndex+1:currIndex+PWVector(1,ii))=normalSig;
        finalDeldSig(1,currIndex+1:currIndex+PWVector(1,ii))=delaydSig;
        finalVideSig(1,currIndex+1:currIndex+PWVector(1,ii))=pulseAmp;
        currIndex=currIndex+PRIVector(ii);
    end
end
finalVideSig=filter(vFilterB,vFilterA,finalVideSig);

end

