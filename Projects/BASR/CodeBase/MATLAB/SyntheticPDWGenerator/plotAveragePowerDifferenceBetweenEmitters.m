function plotAveragePowerDifferenceBetweenEmitters(varargin)
PLOT_TYPE=2;
if(nargin==2)
    EmitterParameters=varargin{1};emitterStartPoints=varargin{2};
    
else
    file=matfile('Data\Pulse_Info.mat');
    EmitterParameters=file.PulseParameters;emitterStartPoints=file.EmitterStartPoints;
end

emitterStartPoints=[1,cumsum(emitterStartPoints)];
eparameters={};

for ii=1:length(emitterStartPoints)-1
    currParameters=EmitterParameters(emitterStartPoints(ii):emitterStartPoints(ii+1),:);
    eparameters{ii}=currParameters;
end

powers={};
duration=double(max(currParameters.TOA))/1.6e9;
duration=0:0.01:duration;
for ii=1:length(emitterStartPoints)-1
    tim=double(eparameters{ii}.TOA)/1.6e9;
    pow=eparameters{ii}.FinalPower'+getAOAGain(eparameters{ii}.AOA);
    pow=spline(tim,pow,duration);
    powers{ii}=pow;
end

figure;
for ii=1:length(emitterStartPoints)-1
   for jj=ii+1:length(emitterStartPoints)-1
       subplot(length(emitterStartPoints)-1-ii,1,jj-ii);
       
       if(PLOT_TYPE==1)
           input=powers{ii};
           [v1,indexes]=max(input);
           index1=indexes+4*(0:1:length(duration)-1);
           input(index1)=-70;
           [v2,indexes]=max(input);
           index2=indexes+4*(0:1:length(duration)-1);
           diff=[abs(v1-powers{jj}(index1));abs(v2-powers{jj}(index2))];
           ylim([-2 10]);
           
       elseif(PLOT_TYPE==2)
           input=powers{ii};
           [v1,indexes]=max(input);
           index1=indexes+4*(0:1:length(duration)-1);
           input(index1)=-70;
           [v2,indexes]=max(input);
           index2=indexes+4*(0:1:length(duration)-1);
           d1=abs(v1-powers{jj}(index1));
           d2=abs(v2-powers{jj}(index2));
           diff=d1+d2;
           
       else
           diff=abs(powers{ii}-powers{jj});
           diff(powers{ii}<-70)=60;
           diff(powers{jj}<-70)=60;
           ylim([-2 10]);
       end
       plot(diff');ylim([-2 20]);
       title(sprintf('Emitter %d and %d',int32(ii),int32(jj)));
   end
   if(ii~=length(emitterStartPoints)-1)
       pause();
   end
end
close();
end