%Provides functionality to pre-generate all the parameters of a single emitter
%according to specified parameters in a structure (PRI,RF,type,etc.).
%Complete signal generation for a single emitter without Composite signal
%conditioning.
%Also writes the parameters of an emitter to TotalInfoTable file
classdef EmitterClass < handle
   properties
      ERP;
      freqType,pwType,priType;
      startTime;
      emitterDuration;
      PRIVector;
      pulseArrivalTime;
      
      EmBlanking;
      
      RFVector;
      RFModifiedVector;
      
      PWVector;
      AOA;AOA_El;Distance; %AOA and distance from air craft for each pulse
      struct_parameters;
      RF_amplitude =1;
      pulse_amplitude=1;
      LOVector;
      FS;
      
      antennaArray;
      scanGaindB;
      blankingGaindB;
      receivedPower;
      bandVector;
      kinemeticsObj;
      scannerObj;
   end
   methods
       function obj=EmitterClass(varargin)
           narginchk(0,4);
           switch (nargin)
               case 0
                   return;
               case 4
                   
               otherwise
                   assert(0);
           end
           FS                    = varargin{1};
           bandSwitchingObj      = varargin{2};
           aircraftKinematicsObj = varargin{3};
           eInfo_Struct          = varargin{4};
           obj(length(eInfo_Struct),1)=EmitterClass();
           PulseParameters=[];
           sizeArr         =zeros(1,length(eInfo_Struct),'int64');
           for ii=1:length(eInfo_Struct)
                obj(ii,1).Initialize(FS,bandSwitchingObj,aircraftKinematicsObj,eInfo_Struct(ii));
                t                = obj(ii,1).getInfoTable();
                sizeArr(1,ii)=length(t.AOA);
                if(ii==1)
                    PulseParameters=t;
                    continue;
                end
                names=fieldnames(t);
                for jj=1:length(names)
                    PulseParameters=setfield(PulseParameters,names{jj},[getfield(PulseParameters,names{jj});getfield(t,names{jj})]);
                end
           end
           FileName             = "Pulse_Info.mat";
           file                 = matfile("Data/"+FileName,'Writable',true);
           file.PulseParameters = struct2table(PulseParameters);
           file.EmitterStartPoints = sizeArr;
           
       end
       function Initialize(obj,FS,bandSwitchingObj,aircraftKinematicsObj,eInfo_Struct)
           obj.FS                   = int64(FS);
           obj.startTime            = max(1,int64(FS*eInfo_Struct.EmitterStartTime));
           obj.emitterDuration      = int64(FS*eInfo_Struct.Time);
           tempTypes                = string(strsplit(eInfo_Struct.Type_Name,'.'));
           
           obj.priType              = tempTypes(1,contains(tempTypes,'pri'));
           obj.freqType             = tempTypes(1,contains(tempTypes,'freq'));
           obj.pwType               = tempTypes(1,contains(tempTypes,'pw'));
           if(isempty(obj.priType))
              obj.priType = 'pri_fix'; 
           end
           if(isempty(obj.freqType))
              obj.freqType = 'freq_fix'; 
           end
           if(isempty(obj.pwType))
              obj.pwType = 'pw_fix'; 
           end
           
           obj.struct_parameters    = eInfo_Struct;
           obj.ERP                  = eInfo_Struct.Power;
           
           obj.RFVector             = eInfo_Struct.RF;
           obj.antennaArray         = AntennaAssemblyClass();
           obj.antennaArray.Initialize(bandSwitchingObj);
           
           blankingFlag = eInfo_Struct.EmitterBlankingPresent;
           obj.EmBlanking = PulseBlankingClass;
           blankingStruct.Type       = 'Emitter';
           blankingStruct.startPoint = eInfo_Struct.EmitterStartTime;
           blankingStruct.pulseWidth = eInfo_Struct.EmitterBlankingpw;
           blankingStruct.pulseCount = eInfo_Struct.EmitterBlankingpulses;
           blankingStruct.PRI        = eInfo_Struct.EmitterBlankingpri;
           obj.EmBlanking.Initialize(blankingFlag,FS,blankingStruct);
           
           obj.generatePRIVector();
           obj.generateRFVector();
           obj.generatePWVector();
           
           emitterStartPosition = eInfo_Struct.EmitterPosition;
           emitterSpeed         = eInfo_Struct.EmitterVelocity;
           obj.kinemeticsObj    = KinematicsClass(emitterStartPosition,eInfo_Struct.TotalTime);
           obj.kinemeticsObj.calculatePositionFromSpeed(double(obj.pulseArrivalTime(1,1:end-1))/double(FS),emitterSpeed);
           %obj.kinemeticsObj.calculatePositionFromSpeed_circularMovement(double(obj.pulseArrivalTime(1,1:end-1))/double(FS),emitterSpeed);
           
           scanPattern          = eInfo_Struct.EmitterScanType;
           obj.scannerObj       = ScannerClass(scanPattern,eInfo_Struct.EmitterRPM,double(obj.pulseArrivalTime(1,1:end-1))/double(FS));
           [scanGain,distance]  = obj.scannerObj.getScannerGain(obj.kinemeticsObj,aircraftKinematicsObj);
           fspl                 = calculatefspl(distance,obj.RFVector);
           obj.Distance         = distance;
           recPower             = obj.ERP-fspl;
           recPower(recPower>0) = 0;
           obj.receivedPower    = recPower;
           obj.scanGaindB       = scanGain;
           obj.blankingGaindB   = obj.EmBlanking.getBlankingGain(obj.pulseArrivalTime(1,1:end-1));
           [az,el,~]             = aircraftKinematicsObj.calculateRelativeSphericalPosition(obj.kinemeticsObj,0);
           obj.AOA              = wrapTo360(az); % -90 because our antenna A is at angle 135 not at 45
           obj.AOA_El           = el; % range is -90 to 90
       end
       function parameterTable=getInfoTable(obj)  
           parameterTable      = [];
            
           parameterTable.TOA   = obj.pulseArrivalTime(1,1:end-1)';
           parameterTable.AOA   = obj.AOA';
           parameterTable.AOA_El   = obj.AOA_El';
           parameterTable.BlankingGain=obj.blankingGaindB';
           parameterTable.RawRecPower= obj.receivedPower';
           parameterTable.FinalPower = obj.receivedPower'+obj.scanGaindB'+obj.blankingGaindB';
           parameterTable.RF    = obj.RFVector';
           parameterTable.PW    = obj.PWVector';
           parameterTable.PRI   = obj.PRIVector';
           parameterTable.LO    = obj.LOVector';
           parameterTable.Band  = int8(obj.bandVector');
           parameterTable.Distance=obj.Distance';
       end
       function generatePRIVector(obj)
           e=obj.struct_parameters;
           switch(obj.priType)
               case {'pri_fix','pri_agile','pri_complex'}
                   PRI              = e.PRI;
                   DeltaT           = e.t / 100;
                   DeltaPRI         = PRI * (DeltaT);
                   maxPRI           = int64(obj.FS*(PRI + (DeltaPRI)));
                   minPRI           = int64(obj.FS*(PRI - (DeltaPRI)));
                   TotalPulses      = int32(min(1e8,100+ceil(double(obj.emitterDuration) / double(PRI * obj.FS))));
                   obj.PRIVector    = int64(randi([minPRI maxPRI],1,TotalPulses,'int32'));
                   
               case 'pri_slide'
                   startValue   = int64(obj.FS* obj.struct_parameters.PRI_Start);
                   stepValue    = int64(obj.FS* obj.struct_parameters.PRI_Step);
                   stopValue    = int64(obj.FS* obj.struct_parameters.PRI_Stop);
                   pattern      =               obj.struct_parameters.Modulation;
                   sinPeriod    =               obj.struct_parameters.Cycle_Sin;
                   sampleCount  = int64(min(1e8,ceil(double(obj.emitterDuration)/double(mean([startValue,stopValue])-stepValue/2))));
                   obj.PRIVector= patternGenerator_SampleCount(sampleCount,startValue,stepValue,stopValue,pattern,sinPeriod);
                   %obj.PRIVector= patternGenerator_intervalLength(...
                   %    obj.emitterDuration,startValue,stepValue,stopValue,pattern,sinPeriod);
                   
               case 'pri_stagger'
                   validCount = e.StaggerN;
                   priVector        =[e.Stag_PRI1          ,e.Stag_PRI2          ,e.Stag_PRI3          ,e.Stag_PRI4      ,...
                                      e.Stag_PRI5          ,e.Stag_PRI6          ,e.Stag_PRI7          ,e.Stag_PRI8      ,...
                                      e.Stag_PRI9          ,e.Stag_PRI10         ,e.Stag_PRI11         ,e.Stag_PRI12     ,...
                                      e.Stag_PRI13         ,e.Stag_PRI14         ,e.Stag_PRI15         ,e.Stag_PRI16     ,...
                                      e.Stag_PRI17         ,e.Stag_PRI18         ,e.Stag_PRI19         ,e.Stag_PRI20     ,...
                                      e.Stag_PRI21         ,e.Stag_PRI22         ,e.Stag_PRI23         ,e.Stag_PRI24     ,...
                                      e.Stag_PRI25         ,e.Stag_PRI26         ,e.Stag_PRI27         ,e.Stag_PRI28     ,...
                                      e.Stag_PRI29         ,e.Stag_PRI30         ,e.Stag_PRI31         ,e.Stag_PRI32     ,...
                                      e.Stag_PRI33         ,e.Stag_PRI34         ,e.Stag_PRI35];
                   Deviation_Vector =[e.staggerDeltat1     ,e.staggerDeltat2     ,e.staggerDeltat3     ,e.staggerDeltat4 ,...
                                      e.staggerDeltat5     ,e.staggerDeltat6     ,e.staggerDeltat7     ,e.staggerDeltat8 ,...
                                      e.staggerDeltat9     ,e.staggerDeltat10    ,e.staggerDeltat11    ,e.staggerDeltat12,...
                                      e.staggerDeltat13    ,e.staggerDeltat14    ,e.staggerDeltat15    ,e.staggerDeltat16,...
                                      e.staggerDeltat17    ,e.staggerDeltat18    ,e.staggerDeltat19    ,e.staggerDeltat20,...
                                      e.staggerDeltat21    ,e.staggerDeltat22    ,e.staggerDeltat23    ,e.staggerDeltat24,...
                                      e.staggerDeltat25    ,e.staggerDeltat26    ,e.staggerDeltat27    ,e.staggerDeltat28,...
                                      e.staggerDeltat29    ,e.staggerDeltat30    ,e.staggerDeltat31    ,e.staggerDeltat32,...
                                      e.staggerDeltat33    ,e.staggerDeltat34    ,e.staggerDeltat35];
                   tempPRIs     = int64(double(obj.FS)*priVector);
                   tempPRIs     = tempPRIs(:,1:validCount);
                   tempDevs     = Deviation_Vector(:,1:validCount)/100;
                   resetCounts  = int32(min(1e7,ceil(double(obj.emitterDuration)/double(sum(tempPRIs)))));
                   tempPRIs     = repmat(tempPRIs,1,resetCounts);
                   tempDevs     = repmat(tempDevs,1,resetCounts);
                   rangeLimitedRandomNumber=2.*(rand(1,length(tempPRIs))-0.5);
                   tempPRIs     = tempPRIs+int64(double(tempPRIs).*tempDevs.*rangeLimitedRandomNumber);
                   obj.PRIVector= int64(tempPRIs);
                   
                   case {'pri_staggerComplex','pri_staggercomplex'}
                   validCount = e.StaggerN;
                   priVector        =[e.Stag_PRI1          ,e.Stag_PRI2          ,e.Stag_PRI3          ,e.Stag_PRI4      ,...
                                      e.Stag_PRI5          ,e.Stag_PRI6          ,e.Stag_PRI7          ,e.Stag_PRI8      ,...
                                      e.Spritag_PRI9          ,e.Stag_PRI10         ,e.Stag_PRI11         ,e.Stag_PRI12     ,...
                                      e.Stag_PRI13         ,e.Stag_PRI14         ,e.Stag_PRI15         ,e.Stag_PRI16     ,...
                                      e.Stag_PRI17         ,e.Stag_PRI18         ,e.Stag_PRI19         ,e.Stag_PRI20     ,...
                                      e.Stag_PRI21         ,e.Stag_PRI22         ,e.Stag_PRI23         ,e.Stag_PRI24     ,...
                                      e.Stag_PRI25         ,e.Stag_PRI26         ,e.Stag_PRI27         ,e.Stag_PRI28     ,...
                                      e.Stag_PRI29         ,e.Stag_PRI30         ,e.Stag_PRI31         ,e.Stag_PRI32     ,...
                                      e.Stag_PRI33         ,e.Stag_PRI34         ,e.Stag_PRI35];
                   Deviation_Vector =[e.staggerDeltat1     ,e.staggerDeltat2     ,e.staggerDeltat3     ,e.staggerDeltat4 ,...
                                      e.staggerDeltat5     ,e.staggerDeltat6     ,e.staggerDeltat7     ,e.staggerDeltat8 ,...
                                      e.staggerDeltat9     ,e.staggerDeltat10    ,e.staggerDeltat11    ,e.staggerDeltat12,...
                                      e.staggerDeltat13    ,e.staggerDeltat14    ,e.staggerDeltat15    ,e.staggerDeltat16,...
                                      e.staggerDeltat17    ,e.staggerDeltat18    ,e.staggerDeltat19    ,e.staggerDeltat20,...
                                      e.staggerDeltat21    ,e.staggerDeltat22    ,e.staggerDeltat23    ,e.staggerDeltat24,...
                                      e.staggerDeltat25    ,e.staggerDeltat26    ,e.staggerDeltat27    ,e.staggerDeltat28,...
                                      e.staggerDeltat29    ,e.staggerDeltat30    ,e.staggerDeltat31    ,e.staggerDeltat32,...
                                      e.staggerDeltat33    ,e.staggerDeltat34    ,e.staggerDeltat35];
                   tempPRIs     = int64(double(obj.FS)*priVector);
                   tempPRIs     = tempPRIs(:,1:validCount);
                   tempDevs     = Deviation_Vector(:,1:validCount)/100;
                   resetCounts  = int32(min(1e7,ceil(double(obj.emitterDuration)/double(sum(tempPRIs)))));
                   randPerms = repmat(tempPRIs,1,resetCounts);
                   for ii=1:resetCounts
                       randPerms((ii-1)*validCount+1:ii*validCount)=randperm(validCount);
                   end
                   tempPRIs     = tempPRIs(randPerms);
                   tempDevs     = tempDevs(randPerms);
                   rangeLimitedRandomNumber=2.*(rand(1,length(tempPRIs))-0.5);
                   tempPRIs     = tempPRIs+int64(double(tempPRIs).*tempDevs.*rangeLimitedRandomNumber);
                   obj.PRIVector= int64(tempPRIs);
                   
                                  
               case 'pri_switch'
                   validCount = e.N;
                   priVector        =[e.pri1          ,e.pri2          ,e.pri3          ,e.pri4      ,...
                                      e.pri5          ,e.pri6          ,e.pri7          ,e.pri8      ,...
                                      e.pri9          ,e.pri10         ,e.pri11         ,e.pri12     ,...
                                      e.pri13         ,e.pri14         ,e.pri15         ,e.pri16     ,...
                                      e.pri17         ,e.pri18         ,e.pri19         ,e.pri20     ,...
                                      e.pri21         ,e.pri22         ,e.pri23         ,e.pri24     ,...
                                      e.pri25         ,e.pri26         ,e.pri27         ,e.pri28     ,...
                                      e.pri29         ,e.pri30         ,e.pri31         ,e.pri32     ,...
                                      e.pri33         ,e.pri34         ,e.pri35         ];
                   priHoldtime      =[e.d_time1       ,e.d_time2       ,e.d_time3       ,e.d_time4   ,...
                                      e.d_time5       ,e.d_time6       ,e.d_time7       ,e.d_time8   ,...
                                      e.d_time9       ,e.d_time10      ,e.d_time11      ,e.d_time12  ,...
                                      e.d_time13      ,e.d_time14      ,e.d_time15      ,e.d_time16  ,...
                                      e.d_time17      ,e.d_time18      ,e.d_time19      ,e.d_time20  ,...
                                      e.d_time21      ,e.d_time22      ,e.d_time23      ,e.d_time24  ,...
                                      e.d_time25      ,e.d_time26      ,e.d_time27      ,e.d_time28  ,...
                                      e.d_time29      ,e.d_time30      ,e.d_time31      ,e.d_time32  ,...
                                      e.d_time33      ,e.d_time34      ,e.d_time35];
                   Deviation_Vector =[e.switchDeltat1 ,e.switchDeltat2 ,e.switchDeltat3 ,e.switchDeltat4 ,...
                                      e.switchDeltat5 ,e.switchDeltat6 ,e.switchDeltat7 ,e.switchDeltat8 ,...
                                      e.switchDeltat9 ,e.switchDeltat10,e.switchDeltat11,e.switchDeltat12,...
                                      e.switchDeltat13,e.switchDeltat14,e.switchDeltat15,e.switchDeltat16,...
                                      e.switchDeltat17,e.switchDeltat18,e.switchDeltat19,e.switchDeltat20,...
                                      e.switchDeltat21,e.switchDeltat22,e.switchDeltat23,e.switchDeltat24,...
                                      e.switchDeltat25,e.switchDeltat26,e.switchDeltat27,e.switchDeltat28,...
                                      e.switchDeltat29,e.switchDeltat30,e.switchDeltat31,e.switchDeltat32,...
                                      e.switchDeltat33,e.switchDeltat34,e.switchDeltat35];
                   priHoldtime  = int64(double(obj.FS)*priHoldtime(1:validCount));
                   priVector    = int64(double(obj.FS)*priVector);
                   tempPRIs     = priVector(:,1:validCount);
                   tempDevs     = Deviation_Vector(:,1:validCount)/100;
                   
                   resetCounts  = int32(ceil(double(obj.emitterDuration)/double(sum(priHoldtime))));
                   maxValues    = int32(min(1e8,ceil(resetCounts*sum(priHoldtime./tempPRIs))))+1;
                   tempPRIs     = repmat(tempPRIs,1,resetCounts);
                   tempDevs     = repmat(tempDevs,1,resetCounts);
                   priHoldtimes = cumsum(repmat(priHoldtime,1,resetCounts));
                   
                   priVectorfinal = zeros(1,maxValues,'int64');
                   timeProcessed  = 0; tempIndex = 1;
                   for ii=1:maxValues
                       rangeLimitedRandomNumber=2.*(rand(1,1)-0.5);
                       value=tempPRIs(1,mod(tempIndex-1,validCount)+1);
                       deviation=tempDevs(1,mod(tempIndex-1,validCount)+1);
                       priValue=value+int64(double(value.*deviation).*rangeLimitedRandomNumber);
                       priVectorfinal(1,ii)=priValue;
                       timeProcessed=timeProcessed+priValue;
                       if(timeProcessed>priHoldtimes(tempIndex))
                           tempIndex=tempIndex+1;
                       end
                       if(timeProcessed>=obj.emitterDuration)
                           break;
                       end
                   end
                   obj.PRIVector=int64(priVectorfinal(1:ii));
                   
               otherwise
                   PRI= int64(obj.FS*obj.struct_parameters.PRI);
                   if(PRI==0)
                       disp('Error in PRI Generation vector');
                       PRI=16000;
                   end
                   count=int64(ceil(double(obj.emitterDuration)/double(PRI)));
                   obj.PRIVector=PRI.*ones(1,count,'int64');
           end
           obj.checkLimits_PRIVector();
           tempPRI=[obj.startTime,obj.PRIVector];
           obj.pulseArrivalTime=cumsum(tempPRI);
       end
       function checkLimits_PRIVector(obj)
           %find the pulse that arrives just before emitter stops generating pulses
           stopTime = obj.emitterDuration;
           temp     = find(cumsum(obj.PRIVector)<stopTime,1,'last');
           if(~isempty(temp) && temp<length(obj.PRIVector))
               temp=temp+1;
               obj.PRIVector         = obj.PRIVector(1:temp);
               obj.PRIVector(end)    = obj.PRIVector(end)-(sum(obj.PRIVector)-stopTime);
           end
           if(sum(obj.PRIVector)~=obj.emitterDuration)
               disp('error in creating pri vector');
           end
           
       end
       
       function generatePWVector(obj)
           e        = obj.struct_parameters;
           PW       = double(e.PW) * double(obj.FS);
           pwCounts = length(obj.PRIVector);
           switch(obj.pwType)
               case 'pw_fix'
                   DeltaPW = (e.pw/100)*PW;
                   obj.PWVector = int64(PW + (DeltaPW.*2.*(rand(1,pwCounts)-0.5)));
               otherwise
                   if(PW<16)
                       disp('Invalid PW values Exists')
                       PW=96;
                   end
                   obj.PWVector = ones(1,length(obj.PRIVector),'int64')*int64(PW);
           end
           
           obj.PWVector=min(obj.PRIVector,obj.PWVector);
           
           blankPulses =obj.struct_parameters.EmitterBlankPulses;
           if(blankPulses~=0 && ~isnan(blankPulses))
               blankPulsesIndexes=sort(randi([1,length(obj.PWVector)],1,int64(blankPulses)));
               obj.PWVector(1,blankPulsesIndexes)=0;
           end
       end
       
           
       function generateRFVector(obj)
           rfCounts=length(obj.PRIVector); 
           e=obj.struct_parameters;
           switch(obj.freqType)
               case {'freq_fix','freq_agile','freq_complex'}
                   RFtemp       = e.RF;
                   DeltaPRI     = RFtemp * (e.f/100);
                   obj.RFVector = RFtemp + (DeltaPRI.*2.*(rand(1,rfCounts)-0.5));
               case 'freq_sweep'
                   startValue   = int64(e.Sweep_RFStart);
                   stepValue    = int64(e.Sweep_RFStep);
                   stopValue    = int64(e.Sweep_RFStop);
                   pattern      = obj.struct_parameters.Freq_Sweep_Mod;
                   sinPeriod    = obj.struct_parameters.Cycle_Sin;
                   obj.RFVector = patternGenerator_SampleCount(rfCounts,startValue,stepValue,stopValue,pattern,sinPeriod);
                   
               case 'freq_jump'
                   validCount=int32(max(1,min(18,e.Freq_JumpN)));
                   RF_Vector        =[ e.Freq_JumpRF1 ,e.Freq_JumpRF2 ,e.Freq_JumpRF3 ,e.Freq_JumpRF4 ,...
                                       e.Freq_JumpRF5 ,e.Freq_JumpRF6 ,e.Freq_JumpRF7 ,e.Freq_JumpRF8 ,...
                                       e.Freq_JumpRF9 ,e.Freq_JumpRF10,e.Freq_JumpRF11,e.Freq_JumpRF12,...
                                       e.Freq_JumpRF13,e.Freq_JumpRF14,e.Freq_JumpRF15,e.Freq_JumpRF16,...
                                       e.Freq_JumpRF17,e.Freq_JumpRF18];
                   Deviation_Vector =[ e.JumpDeltaRF1 ,e.JumpDeltaRF2 ,e.JumpDeltaRF3 ,e.JumpDeltaRF4 ,...  
                                       e.JumpDeltaRF5 ,e.JumpDeltaRF6 ,e.JumpDeltaRF7 ,e.JumpDeltaRF8 ,...
                                       e.JumpDeltaRF9 ,e.JumpDeltaRF10,e.JumpDeltaRF11,e.JumpDeltaRF12,...
                                       e.JumpDeltaRF13,e.JumpDeltaRF14,e.JumpDeltaRF15,e.JumpDeltaRF16,...
                                       e.JumpDeltaRF17,e.JumpDeltaRF18];
                   temp=[RF_Vector;Deviation_Vector/100];
                   temp=temp(:,1:validCount);
                   %randomSamples=randi([1,validCount],1,rfCounts);
                   randomSamples= zeros(1,validCount+mod(rfCounts,validCount),'int32');
                   rfCounts = double(rfCounts);
                   validCount = double(validCount);
                   for jj=1:int32(ceil(rfCounts/validCount))
                       randomSamples(validCount*(jj-1)+1:jj*validCount)=randperm(validCount);
                   end
                   randomSamples= randomSamples(1,1:rfCounts);
                   tempRFs      = temp(:,randomSamples);
                   rangeLimitedRandomNumber=2.*(rand(1,rfCounts)-0.5);
                   tempRFs(1,:) = tempRFs(1,:)+tempRFs(1,:).*(tempRFs(2,:).*rangeLimitedRandomNumber);
                   obj.RFVector = tempRFs(1,:);
                   
               case 'freq_step'
                   validCount=int32(max(1,min(18,e.Freq_StepN)));
                   RF_Vector        = [ e.Freq_StepRF1 ,e.Freq_StepRF2 ,e.Freq_StepRF3 ,e.Freq_StepRF4 ,...
                                        e.Freq_StepRF5 ,e.Freq_StepRF6 ,e.Freq_StepRF7 ,e.Freq_StepRF8 ,...
                                        e.Freq_StepRF9 ,e.Freq_StepRF10,e.Freq_StepRF11,e.Freq_StepRF12,...
                                        e.Freq_StepRF13,e.Freq_StepRF14,e.Freq_StepRF15,e.Freq_StepRF16,...
                                        e.Freq_StepRF17,e.Freq_StepRF18];
                   Deviation_Vector = [ e.StepDeltaRF1 ,e.StepDeltaRF2 ,e.StepDeltaRF3 ,e.StepDeltaRF4 ,...
                                        e.StepDeltaRF5 ,e.StepDeltaRF6 ,e.StepDeltaRF7 ,e.StepDeltaRF8 ,...
                                        e.StepDeltaRF9 ,e.StepDeltaRF10,e.StepDeltaRF11,e.StepDeltaRF12,...
                                        e.StepDeltaRF13,e.StepDeltaRF14,e.StepDeltaRF15,e.StepDeltaRF16,...
                                        e.StepDeltaRF17,e.StepDeltaRF18];
                   temp         = [RF_Vector;Deviation_Vector/100];
                   temp         = temp(:,1:validCount);
                   randomSamples= repmat(temp,1,int32(ceil(double(rfCounts)/double(validCount))));
                   tempRFs      = randomSamples(:,1:rfCounts);
                   rangeLimitedRandomNumber=2.*(rand(1,rfCounts)-0.5);
                   tempRFs(1,:) = tempRFs(1,:)+tempRFs(1,:).*(tempRFs(2,:).*rangeLimitedRandomNumber);
                   obj.RFVector = tempRFs(1,:);
                   
               case 'freq_switch'
                   validCount = e.SwitchNRf;
                   RF_Vector        =[e.Switch_Rf1      ,e.Switch_Rf2      ,e.Switch_Rf3      ,e.Switch_Rf4      ,...
                                      e.Switch_Rf5      ,e.Switch_Rf6      ,e.Switch_Rf7      ,e.Switch_Rf8      ,...
                                      e.Switch_Rf9      ,e.Switch_Rf10     ,e.Switch_Rf11     ,e.Switch_Rf12     ,...
                                      e.Switch_Rf13     ,e.Switch_Rf14     ,e.Switch_Rf15     ,e.Switch_Rf16     ,...
                                      e.Switch_Rf17     ,e.Switch_Rf18];
                   RF_Holdtime      =[e.Switch_RFt1     ,e.Switch_RFt2     ,e.Switch_RFt3     ,e.Switch_RFt4     ,...
                                      e.Switch_RFt5     ,e.Switch_RFt6     ,e.Switch_RFt7     ,e.Switch_RFt8     ,...
                                      e.Switch_RFt9     ,e.Switch_RFt10    ,e.Switch_RFt11    ,e.Switch_RFt12    ,...
                                      e.Switch_RFt13    ,e.Switch_RFt14    ,e.Switch_RFt15    ,e.Switch_RFt16    ,...
                                      e.Switch_RFt17    ,e.Switch_RFt18];
                   Deviation_Vector =[e.SwitchRFDeltat1 ,e.SwitchRFDeltat2 ,e.SwitchRFDeltat3 ,e.SwitchRFDeltat4 ,...
                                      e.SwitchRFDeltat5 ,e.SwitchRFDeltat6 ,e.SwitchRFDeltat7 ,e.SwitchRFDeltat8 ,...
                                      e.SwitchRFDeltat9 ,e.SwitchRFDeltat10,e.SwitchRFDeltat11,e.SwitchRFDeltat12,...
                                      e.SwitchRFDeltat13,e.SwitchRFDeltat14,e.SwitchRFDeltat15,e.SwitchRFDeltat16,...
                                      e.SwitchRFDeltat17,e.SwitchRFDeltat18];
                   RF_Holdtime  = int64(double(obj.FS).*RF_Holdtime(1:validCount));
                   temp         = [RF_Vector;Deviation_Vector/100];
                   temp         = temp(:,1:validCount);
                   resetCounts  = int32(ceil(double(obj.emitterDuration)/double(sum(RF_Holdtime))));
                   rf_ChangeArrivals=cumsum(repmat(RF_Holdtime,1,resetCounts));
                   priChangeArrivals=cumsum(obj.PRIVector);
                   randomSamples = zeros(1,rfCounts,'int32');
                   tempIndex=1;
                   for ii=1:rfCounts
                       while(rf_ChangeArrivals(1,tempIndex)<priChangeArrivals(1,ii))
                           tempIndex = tempIndex+1;
                       end
                       randomSamples(1,ii)=mod(tempIndex-1,validCount)+1;
                   end
                   tempRFs      = temp(:,randomSamples);
                   rangeLimitedRandomNumber=2.*(rand(1,rfCounts)-0.5);
                   tempRFs(1,:) = tempRFs(1,:)+tempRFs(1,:).*(tempRFs(2,:).*rangeLimitedRandomNumber);
                   obj.RFVector = tempRFs(1,:);
                   
               otherwise
                   obj.RFVector = ones(1,rfCounts,'double')*obj.struct_parameters.RF;
           end
           obj.RFVector = double(obj.RFVector);
           obj.generateModifiedRFVector();
       end
       function generateModifiedRFVector(obj)
           obj.RFModifiedVector = zeros(1,length(obj.PRIVector));
           obj.bandVector       = zeros(1,length(obj.RFVector),'int32');
           obj.LOVector         = zeros(1,length(obj.RFVector),'int64');
           Corner_Cases         = double([2.4e9 3.2e9 4e9 4.8e9 5.6e9]);
           
           LO_1 = 4e9;
           %LO_2 = 3.75e9;
           
           obj.RFVector(obj.RFVector<2e9) = 2e9;
           obj.RFVector(obj.RFVector>18e9)= 18e9;
           
           t=(obj.RFVector>= 02e9 & obj.RFVector <= 6e9);
           obj.RFModifiedVector (t) = obj.RFVector(t);
           obj.bandVector       (t) = 1;
           
           t=(obj.RFVector> 06e9 & obj.RFVector <= 10e9);
           obj.RFModifiedVector (t) = 12000000000-obj.RFVector(t);
           obj.bandVector       (t) = 2;
           
           t=(obj.RFVector> 10e9 & obj.RFVector <= 14e9);
           obj.RFModifiedVector (t) = 16000000000-obj.RFVector(t);
           obj.bandVector       (t) = 3;
           
           t=obj.RFVector> 14e9 & obj.RFVector <= 18e9;
           obj.RFModifiedVector (t) = abs(12000000000-obj.RFVector(t));
           obj.bandVector       (t) = 4;
           
           temp = repmat(obj.RFModifiedVector,5,1);  %because we have 5 corner cases
           temp = abs(double(temp)-transpose(Corner_Cases));
           temp = min(temp,[],1);
           
           obj.LOVector(:)          = LO_1;
           %obj.LOVector(temp<125e6) = LO_2;
           if(any(temp<125e6))
               disp('At this Time DCM LO switching logic is not yet finallized');
               disp('so RF signals which are close to CentreFrequencies cannot modeled by DIFM');
               disp('Currently Synthetic data generation tool can only model LO = 4 GHz');
%                assert(0==1);
           end
       end
       function returnVal=copy(obj)
           returnVal(size(obj,1),size(obj,2))   = EmitterClass();
          for ii=1:length(obj)
              returnVal(ii).freqType            =obj(ii).freqType;
              returnVal(ii).pwType              =obj(ii).pwType;
              returnVal(ii).priType             =obj(ii).priType;
              returnVal(ii).startTime           =obj(ii).startTime;
              returnVal(ii).emitterDuration     =obj(ii).emitterDuration;
              returnVal(ii).PRIVector           =obj(ii).PRIVector;
              returnVal(ii).pulseArrivalTime    =obj(ii).pulseArrivalTime;
              returnVal(ii).EmBlanking          =obj(ii).EmBlanking.copy();
              returnVal(ii).RFVector            =obj(ii).RFVector;
              returnVal(ii).RFModifiedVector    =obj(ii).RFModifiedVector;
              returnVal(ii).PWVector            =obj(ii).PWVector;
              returnVal(ii).AOA                 =obj(ii).AOA;
              returnVal(ii).AOA_El              =obj(ii).AOA_El;
              returnVal(ii).struct_parameters   =obj(ii).struct_parameters;
              returnVal(ii).RF_amplitude        =obj(ii).RF_amplitude;
              returnVal(ii).pulse_amplitude     =obj(ii).pulse_amplitude;
              returnVal(ii).LOVector            =obj(ii).LOVector;
              returnVal(ii).FS                  =obj(ii).FS;
              returnVal(ii).antennaArray        =obj(ii).antennaArray.copy();
              returnVal(ii).bandVector          =obj(ii).bandVector;
              returnVal(ii).scanGaindB          =obj(ii).scanGaindB;
              returnVal(ii).blankingGaindB      =obj(ii).blankingGaindB;
              returnVal(ii).receivedPower       =obj(ii).receivedPower;
          end
       end
   end
end