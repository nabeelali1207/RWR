classdef KinematicsClass < handle
    properties
        startPosition;
        stopPosition;
        position;t;
        totalTime;
    end
    
    methods
        function obj=KinematicsClass(startPosition,simTime)
            obj.totalTime=simTime;obj.startPosition=startPosition;
        end
        
        function calculatePositionFromSpeed(obj,timeVector,speedVector)
            displacement=[obj.startPosition,diff(timeVector).*speedVector];
            positionSummary = cumsum(displacement,2);
            obj.position=positionSummary;
            obj.t = timeVector;
            obj.stopPosition=positionSummary(:,end);
        end
        function calculatePositionFromSpeed_circularMovement(obj,timeVector,speedVector)
            maxSpeed=speedVector(1);
            r=abs(obj.startPosition(1));
            perSecondAngleChange=360*(maxSpeed/(2*pi*r));
            angle=cumsum([0,(diff(timeVector)*perSecondAngleChange)]);
            xPos=r*cosd(angle);
            yPos=r*sind(angle);
            positionSummary = [xPos;yPos;timeVector.*speedVector(3)];
            obj.position=positionSummary;
            obj.t = timeVector;
            obj.stopPosition=positionSummary(:,end);
        end
        function pos=getPositionAtTime(obj,queryTime)
            pos=interp1(obj.t,obj.position',queryTime,'linear')';
            pos(:,queryTime<=obj.t(1,1))=repmat(obj.startPosition,1,sum(queryTime<=obj.t(1,1)));
            pos(:,queryTime>=obj.t(1,end))=repmat(obj.stopPosition,1,sum(queryTime>=obj.t(1,end)));
        end
        function [az,el,distance]=calculateRelativeSphericalPosition(obj,otherObj,varargin)
           if(isempty(varargin))
               a=otherObj.getPositionAtTime(obj.t)-obj.position;
           else
               a=otherObj.position-obj.getPositionAtTime(otherObj.t);
           end
           %calculates distance,azimuth and elevation angle between me and otherObj
           [az,el,distance]=cart2sph(a(1,:),a(2,:),a(3,:));
           az=rad2deg(az);
           el=rad2deg(el);
           %res=[az;el;rn];
        end
    end
end