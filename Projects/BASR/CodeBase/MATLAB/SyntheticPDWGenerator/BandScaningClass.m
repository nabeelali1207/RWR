%This class simulates the DIFM hardware switching between different
%antennas and bands. Scan time for each antenna and each band is
%pre-defined as of now but it maybe changed to handle on demand switching.
classdef BandScaningClass < handle
    properties
        switchTime          =[];
        activeAntennaIndex  =[];
        
        numOfAntennas   = 4;
        numOfBands      = 4;
    end
    methods
        function object=copy(obj)
            object                      = BandScaningClass;
            object.switchTime           = obj.switchTime;
            object.activeAntennaIndex   = obj.activeAntennaIndex;
            object.numOfAntennas        = obj.numOfAntennas;
            object.numOfBands           = obj.numOfBands;
        end
        function readDataFromFile(obj)
            file=matfile('Data\bandsInfo.mat');
            obj.switchTime          = file.switchTime;
            obj.activeAntennaIndex  = file.activeAntennaIndex;
            obj.numOfAntennas       = file.numOfAntennas;
            obj.numOfBands          = file.numOfBands;
        end
        function writeDataToFile(obj)
            if(isempty(obj.switchTime))
                disp('Error in Saving Bands Info File.');
                return;
            end
            file=matfile('Data\bandsInfo.mat','writable',true);
            file.switchTime          = obj.switchTime;
            file.activeAntennaIndex  = obj.activeAntennaIndex;
            file.numOfAntennas       = obj.numOfAntennas;
            file.numOfBands          = obj.numOfBands;
        end
        function generate_AntVsBand_Vector(obj,startTime,endTime,FS,antSwitchTime)
            antSwitchTime   = int64(double(FS)*double(antSwitchTime));
            
            %--begining-- A variable that contains time duration each band
            %at each antenna gets activated
            bandsDuration   = zeros(1,obj.numOfAntennas * obj.numOfBands,'int64');
            bandsDuration(:)= antSwitchTime;        % equal time for each band
            
                %---added for specified band dwell time
                bandsDuration   = double(FS)* 1e-3 * [25, 25, 25, 25, 125, 125, 125, 125, 50, 50, 50, 50, 37.5, 37.5, 37.5, 37.5];
                bandsDuration   = int64(bandsDuration);
                %---added for specified band dwell time
            %--ending--
            
            singleCycleTime = sum(bandsDuration);
            numOfCycles     = ceil(double((endTime-startTime+1)/singleCycleTime));
            obj.switchTime  = startTime+[0,cumsum(repmat(bandsDuration,1,numOfCycles))];
            obj.activeAntennaIndex = int32(mod(0:1:length(obj.switchTime),obj.numOfAntennas* obj.numOfBands));
        end
        function antennaIndex   = getActiveAntBandIndex(obj,queryTimeIndex)
            antennaIndex    = obj.activeAntennaIndex(1,find( queryTimeIndex >=obj.switchTime,1,'last'));
        end
        function band           = getActiveBand(obj,queryTimeIndex)
            antennaIndex    = obj.getActiveAntBandIndex(queryTimeIndex);
            band            = floor(double(antennaIndex)/double(obj.numOfBands))+1;
        end
        function ant            = getActiveAntenna(obj,queryTimeIndex)
            antennaIndex    = obj.getActiveAntBandIndex(queryTimeIndex);
            ant             = mod(antennaIndex,obj.numOfAntennas)+1;
        end
        function switchTime     = getNextAntennaOrBandSwitchTime(obj,queryTimeIndex)
            switchTime      = obj.switchTime(1,find( queryTimeIndex <obj.switchTime,1,'first'));
        end
        function [activeBands,activeAntts]    = getActiveBandAndAntenna(obj,queryTimeIndexes)
            queryTimeIndexes=queryTimeIndexes(:);
            [~,sortI]=sort(queryTimeIndexes);
            queryTimeIndexes=queryTimeIndexes(sortI);
            activeIndex=zeros(length(queryTimeIndexes),1);
            
            kk=1;            
            for ii=1:length(obj.switchTime)-1
                while(queryTimeIndexes(kk,1)>=obj.switchTime(1,ii) && queryTimeIndexes(kk,1)<obj.switchTime(1,ii+1))
                    activeIndex(kk,1)=obj.activeAntennaIndex(1,ii);
                    kk=kk+1;
                    if(kk>length(queryTimeIndexes))
                        break;
                    end
                end
                if(kk>length(queryTimeIndexes))
                        break;
                end
            end
            %reverse sort
            sortI(sortI)=1:length(sortI);
            activeIndex=activeIndex(sortI);
            
            activeBands=floor(double(activeIndex)/double(obj.numOfBands))+1;
            activeAntts=mod(activeIndex,obj.numOfAntennas)+1;
            
        end
    end
end
