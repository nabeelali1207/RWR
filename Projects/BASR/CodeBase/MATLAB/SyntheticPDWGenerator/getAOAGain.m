function rfGain=getAOAGain(AOAVect,varargin)


if(isempty(varargin))
    
    
    file=matfile('Emitter/ACAntRadPatt2d.mat');
    antGainsVector(1,:)          = file.APValue_A;
    antGainsVector(2,:)          = file.APValue_B;
    antGainsVector(3,:)          = file.APValue_C;
    antGainsVector(4,:)          = file.APValue_D;
    AOAVect=AOAVect(:);
    ap=antGainsVector;
    rfGain=zeros(4,length(AOAVect));
    rfGain(1,:) = spline((1:length(ap))-1,ap(1,:),AOAVect);
    rfGain(2,:) = spline((1:length(ap))-1,ap(2,:),AOAVect);
    rfGain(3,:) = spline((1:length(ap))-1,ap(3,:),AOAVect);
    rfGain(4,:) = spline((1:length(ap))-1,ap(4,:),AOAVect);
    
else
    file=matfile('Emitter/ACAntRadPatt3dV1.mat');
    pattern3d= file.pattNew;
%     patternAzAngle= file.phiOut;
%     patternElAngle= file.thetaOut;
    
    azAngle=AOAVect(:);
    elAngle=varargin{1};
    
    elIndex=abs(round(elAngle))+1;
    
    beamCenters=[45,135,225,315];
    azAngleNew=double(wrapTo360(azAngle-beamCenters));
    
    azIndex=round(azAngleNew)+1;
    rfGain=zeros(4,length(AOAVect));
    for i=1:length(AOAVect)
        rfGain(:,i)=pattern3d(azIndex(i,:),elIndex(i));
    end
%     for i=1:length(AOAVect)
%         rfGain(:,i)= spline(patternAzAngle,pattern3d(:,elIndex(i)),azAngleNew(i,:));
%     end

%     rfGain(1,:) = spline((1:length(ap))-1,ap(1,:),AOAVect);
%     rfGain(2,:) = spline((1:length(ap))-1,ap(2,:),AOAVect);
%     rfGain(3,:) = spline((1:length(ap))-1,ap(3,:),AOAVect);
%     rfGain(4,:) = spline((1:length(ap))-1,ap(4,:),AOAVect);
end
end