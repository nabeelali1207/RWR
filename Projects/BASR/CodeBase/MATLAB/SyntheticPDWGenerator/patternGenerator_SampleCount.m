function res=patternGenerator_SampleCount(valueCount,startValue,stepValue,stopValue,patternType,varargin)
%//////
%Failures Cases

%////------------------------------------------Invalid parameters check
res=[];
switch(patternType)
    case 'Linear'
        temp=max(startValue,stopValue);
        startValue=min(startValue,stopValue);
        stopValue=temp;
    case {'negprislide','negslide'}
        temp=min(startValue,stopValue);
        startValue=max(startValue,stopValue);
        stopValue=temp;
    case 'tri'
        temp=max(startValue,stopValue);
        startValue=min(startValue,stopValue);
        stopValue=temp;
    case 'sin'
        temp=max(startValue,stopValue);
        startValue=min(startValue,stopValue);
        stopValue=temp;
        periodSin=cell2mat(varargin);
        if(isempty(periodSin) || isnan(periodSin))
            periodSin=0;
        end
end
%////------------------------------------------

res=[];
currentValue=startValue;

switch(patternType)
    case 'Linear'
        while(currentValue-stepValue+1<stopValue)
            res=[res,int64(min(currentValue,stopValue))];
            currentValue=currentValue+stepValue;
        end
    case {'negprislide','negslide'}
        while(currentValue+stepValue>stopValue)
            res=[res,int64(max(currentValue,stopValue))];
            currentValue=currentValue-stepValue;
        end
    case 'tri'
        while(currentValue-stepValue<stopValue)
            res=[res,int64(min(currentValue,stopValue))];
            currentValue=currentValue+stepValue;
        end
        currentValue=stopValue-stepValue;
        while(currentValue+stepValue-1>startValue)
            res=[res,int64(max(currentValue,startValue))];
            currentValue=currentValue-stepValue;
        end
        res=res(1,1:end-1);
    case 'sin'
        mean=double(startValue+stopValue)/2;
        angle=2*pi*double(0:periodSin-1)/double(periodSin);
        value=mean+min(double(stepValue),double(stopValue-startValue)/2).*cos(angle);
        res=[res,int64(value)];
end
reps=ceil(double(valueCount)/double(length(res)));
res=repmat(res,1,reps);
res=res(1:valueCount);
end