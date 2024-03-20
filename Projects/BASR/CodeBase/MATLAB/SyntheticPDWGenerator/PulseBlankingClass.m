%This Class generates the logical indexes for samples that will be set to
%zero due to blanking
classdef PulseBlankingClass < handle
    properties
      blankingFlag;
      blankingPRI;
      blankingPulseArrivalPoints;
      blankingStartPoint;
      blankingPulseWidth;
      blankingPulseCount;
      blankingType;
      
    end
    
    methods
        function object=copy(obj)
            object=PulseBlankingClass;
            object.blankingFlag         =obj.blankingFlag;
            object.blankingPRI          =obj.blankingPRI;
            object.blankingPulseArrivalPoints=obj.blankingPulseArrivalPoints;
            object.blankingStartPoint   =obj.blankingStartPoint;
            object.blankingPulseWidth   =obj.blankingPulseWidth;
            object.blankingPulseCount   =obj.blankingPulseCount;
            object.blankingType         =obj.blankingType;
        end
        function Initialize(obj,blankingFlag,FS,struct)
            obj.blankingFlag=blankingFlag;
            obj.blankingType=string(struct.Type);
            if(obj.blankingFlag==0)
                return
            end
            if(obj.blankingType=='Emitter' || obj.blankingType=='Periodic')
               obj.blankingStartPoint   = int64(FS*struct.startPoint);
               obj.blankingPulseWidth   = int64(FS*struct.pulseWidth);
               obj.blankingPulseCount   = int64(   struct.pulseCount);
               obj.blankingPRI          = int64(FS*struct.PRI);

               obj.blankingPulseArrivalPoints=zeros(1,obj.blankingPulseCount+1,'int64');
               obj.blankingPulseArrivalPoints(1)=obj.blankingStartPoint;
               obj.blankingPulseArrivalPoints(2:end)=obj.blankingPRI;
               obj.blankingPulseArrivalPoints=cumsum(obj.blankingPulseArrivalPoints);
            end
           
        end
        function [blankingMultiplier]=getBlankingGain(obj,queryTimeIndexes)
            blankingMultiplier=zeros(1,length(queryTimeIndexes));
            if(obj.blankingFlag==0)
                return
            end
            for i =1:length(blankingMultiplier)
                
                [~,index]=min(abs(queryTimeIndexes(1,i)-obj.blankingPulseArrivalPoints(1,1:end-1)));
                if(abs(queryTimeIndexes(1,i)-obj.blankingPulseArrivalPoints(1,index))<obj.blankingPulseWidth)
                    blankingMultiplier(1,i)=1;
                end
            end
            blankingMultiplier(blankingMultiplier==0)=-200;
            blankingMultiplier(blankingMultiplier==1)=0;
       end
    end
end