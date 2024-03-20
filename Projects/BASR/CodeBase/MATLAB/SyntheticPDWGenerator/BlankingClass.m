%This Class generates the logical indexes for samples that will be set to
%zero due to blanking
classdef BlankingClass < handle
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
            object=BlankingClass;
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
        function [blankingPulse,validFlag]=getBlanking(obj,startPoint,endPoint)           
            blankingPulse=[];
            validFlag=1;
            if(obj.blankingFlag==0)
               validFlag=0;
               return;
            end
            if(endPoint<=obj.blankingStartPoint)
               validFlag=0;
               return;
            elseif(startPoint>=obj.blankingPulseArrivalPoints(end))
               validFlag=0;
               return;
            end
            tempLength=endPoint-startPoint+1;
            blankingPulse=zeros(1,tempLength,'logical');


            startPulseIndex=find(obj.blankingPulseArrivalPoints>startPoint, 1, 'first' );
            startPulseIndex=max(1,startPulseIndex-1);
            endPulseIndex=find(obj.blankingPulseArrivalPoints<endPoint, 1, 'last' );
            endPulseIndex=min(length(obj.blankingPulseArrivalPoints),endPulseIndex+1);

            pulseStartPoint= obj.blankingPulseArrivalPoints(startPulseIndex);
            pulseEndPoint=   obj.blankingPulseArrivalPoints(endPulseIndex);

            blankingSig=zeros(1,pulseEndPoint-pulseStartPoint,'logical');
            for i=startPulseIndex:endPulseIndex-1
               blankingSig(1,obj.blankingPulseArrivalPoints(i)-pulseStartPoint+1:...
                   obj.blankingPulseArrivalPoints(i)-pulseStartPoint+1+obj.blankingPulseWidth)=1;
            end
            [outOffset,sigOffset,len]=doTheMagic(pulseStartPoint,pulseEndPoint,startPoint,endPoint);
            blankingPulse(1,outOffset+1:outOffset+len)=blankingSig(1,sigOffset+1:sigOffset+len);
       end
    end
end