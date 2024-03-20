%This class handles the antenna directivity, power recieved at antenna will
%be according to the angle of arrival.
%This class outputs the scaling vectors for the generated raw signal and
%video signal.
classdef AntennaAssemblyClass < handle
    
    properties
        bandScaningObj;
        antGainsVector;
    end
    
    methods
        function object=copy(obj)
            object                  = AntennaAssemblyClass();
            object.bandScaningObj   = obj.bandScaningObj.copy();
            object.antGainsVector   = obj.antGainsVector;
        end
        function Initialize(obj,aircraftBandScanningObject)
            
            if(class(aircraftBandScanningObject)~='BandScaningClass')
                disp('Invalid object sent instead of BandScanning Object');
                assert(false);
            elseif(isempty(aircraftBandScanningObject.switchTime))
                disp('BandScanning Object Empty');
                assert(false);
            end
            obj.bandScaningObj  = aircraftBandScanningObject.copy();
            file=matfile('Emitter/ACAntRadPatt2d.mat');
            obj.antGainsVector               = zeros(4,361);
            obj.antGainsVector(1,:)          = file.APValue_A;
            obj.antGainsVector(2,:)          = file.APValue_B;
            obj.antGainsVector(3,:)          = file.APValue_C;
            obj.antGainsVector(4,:)          = file.APValue_D;
        end
        function [rfGain,vidGain]=getPowerLevelMultipliers(obj,receivedPowerdB,AOAVect)
            rfGain      = zeros(4,length(AOAVect));
            ap          = obj.antGainsVector;
            %Cubic splines are used to approximate in between values as
            %raditation pattern table is defined only for integer values
            %from 0-360 and not in between. eg. what will be gain when AOA
            %is 1.5degrees solution is to interpolate.
            rfGain(1,:) = spline(1:length(ap),ap(1,:),AOAVect);
            rfGain(2,:) = spline(1:length(ap),ap(2,:),AOAVect);
            rfGain(3,:) = spline(1:length(ap),ap(3,:),AOAVect);
            rfGain(4,:) = spline(1:length(ap),ap(4,:),AOAVect);
            rfGain      = rfGain + receivedPowerdB;
            zeroPoints  = (rfGain<-60);
            vidGain     = 10.^(rfGain/10);
            rfGain      = sqrt(1e-3*50)*10.^(rfGain/20)*sqrt(2);
            
            %pulse at dominent antenna must give some value at other antennas
            isDomAntennaPres = (sum(zeroPoints,1)==4);
            isDomAntennaPres =  repmat(isDomAntennaPres,4,1);
            vidGain(isDomAntennaPres) =0;
            
            
        end
        %This function turns the power levels received for a pulse to power
        %levels received for a single sample catering for antenna switching
        %taking place at the aircraft. Video signal is not effected by the
        %switching time as only ADC for DIFM is time shared, each antenna
        %has its own dedicated dlva. so video signal is uneffected by
        %switching of bands of difm.
        function [sigMultiplier,vidMultiplier]=getRFScalingVector(obj,emitterPowerdB...
                ,arrivalIndex,bandVector,AOAVector)
            startPoint=arrivalIndex(1);     offset=arrivalIndex(1);
            endPoint=arrivalIndex(end);
            
            [pulseRfGain,pulseVidGain] = obj.getPowerLevelMultipliers(emitterPowerdB,AOAVector);
            sigMultiplier   = zeros(1,endPoint-startPoint,'single');
            vidMultiplier   = zeros(4,endPoint-startPoint,'single');
            
            for ii=1:length(arrivalIndex)-1
                vidMultiplier(1,arrivalIndex(ii)+1-offset:arrivalIndex(ii+1)-offset)=pulseVidGain(1,ii);
                vidMultiplier(2,arrivalIndex(ii)+1-offset:arrivalIndex(ii+1)-offset)=pulseVidGain(2,ii);
                vidMultiplier(3,arrivalIndex(ii)+1-offset:arrivalIndex(ii+1)-offset)=pulseVidGain(3,ii);
                vidMultiplier(4,arrivalIndex(ii)+1-offset:arrivalIndex(ii+1)-offset)=pulseVidGain(4,ii);
                startAntIndex   = obj.bandScaningObj.getActiveAntBandIndex(arrivalIndex(ii));
                endAntIndex     = obj.bandScaningObj.getActiveAntBandIndex(arrivalIndex(ii+1));
                startingBand    = obj.bandScaningObj.getActiveBand(arrivalIndex(ii));
                endingBand      = obj.bandScaningObj.getActiveBand(arrivalIndex(ii+1));
                if(startAntIndex==endAntIndex)
                    if(startingBand==bandVector(ii))
                        currAntenna=obj.bandScaningObj.getActiveAntenna(arrivalIndex(ii));
                        sigMultiplier(1,arrivalIndex(ii)+1-offset:arrivalIndex(ii+1)-offset)=pulseRfGain(currAntenna,ii);
                    end
                else
                    %this code section written in else section handles the
                    %logic when a pulse was going on but antenna switched.
                    %Only two things can happen
                    if(startingBand==bandVector(ii))
                        currAntenna=obj.bandScaningObj.getActiveAntenna(arrivalIndex(ii));
                        sigMultiplier(1,arrivalIndex(ii)+1-offset:...
                            obj.bandScaningObj.getNextAntennaOrBandSwitchTime...
                            (arrivalIndex(ii)-1)-offset)=pulseRfGain(currAntenna,ii);
                    end
                    if(endingBand==bandVector(ii))
                        currAntenna=obj.bandScaningObj.getActiveAntenna(arrivalIndex(ii+1));
                        sigMultiplier(1,obj.bandScaningObj.getNextAntennaOrBandSwitchTime...
                            (arrivalIndex(ii)-1)-offset+1:arrivalIndex(ii+1)-offset)...
                            =pulseRfGain(currAntenna,ii);
                    end
                end                    
            end
        end
    end
end