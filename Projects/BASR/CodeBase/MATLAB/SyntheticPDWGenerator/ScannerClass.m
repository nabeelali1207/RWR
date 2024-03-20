%Simulates a rotating antenna according to loaded antenna pattern.
classdef ScannerClass < handle
    
    properties
        RPM=60;
        scanType;
        radiationPattern;
        relativeTimeVector;
        
        myAzimuthVector;
        myElevationVector;
        BWAZ=0.05;
        BWEL=0.5;

%         BWAZ=.5;
%         BWEL=0.5;
    end
    
    methods
        function obj=ScannerClass(scanType,RPM,tIndexes)
            obj.scanType = scanType;
            obj.RPM     =RPM;
%             obj.RPM     =2;
            obj.relativeTimeVector=cumsum([0,diff(tIndexes)]);
            s=load('Emitter/rad_patt_look_up_table.mat');
            a=s.look_up_table(1:181)';
            obj.radiationPattern=a;
            
        end
        function [scanGain,dist]=getScannerGain(obj,myKinematicsObj,otherKinematicsObj)
            [az,el,dist]=myKinematicsObj.calculateRelativeSphericalPosition(otherKinematicsObj);
            
            switch (obj.scanType)
                case 0      %tracking
                    obj.myAzimuthVector=az;
                    obj.myElevationVector=el;
                case 1      %circular
                    obj.myElevationVector=el;
                    angleStepPerSecond = 360*obj.RPM/60;
                    obj.myAzimuthVector=wrapTo180(obj.relativeTimeVector.*angleStepPerSecond);
                case 2
                    elevationStep = 4;      % how much elevation is changed with one circle
                    angleStepPerSecond = 360*obj.RPM/60;
                    obj.myAzimuthVector=wrapTo180(obj.relativeTimeVector.*angleStepPerSecond);
                    obj.myElevationVector=wrapTo180(obj.relativeTimeVector.*elevationStep.*obj.RPM/60);
            end
%             az=wrapTo180(az+2*2*(0.5-rand(size(az))));
            lookupAngle=sqrt((wrapTo180(az-obj.myAzimuthVector)/obj.BWAZ).^2+...
                ((el-obj.myElevationVector)/obj.BWEL).^2);
            scanGain=spline(0:180,obj.radiationPattern,max(0,min(180,lookupAngle)));
        end
    end
    
end