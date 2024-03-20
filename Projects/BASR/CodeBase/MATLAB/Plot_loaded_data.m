% Plot loaded file
Rfv_en = 0; % Make invalid RF = 0

PowerThres_en = 0;%   *********************ENABLE Power thresholding***************************
upperLim = 800;
lowerLim = 60;
PxPySum = 0; % make it 1 for the threshold to consider px + py
% rfv = rfv;

TimeWindow_en = 0 ;%   *********************ENABLE Time Filtering***************************
StartTime = 3260;
EndTime = 3290; %       if EndTIme = 0 , will consider it the end of file


% *********************ENABLE AoA Filtering***************************
AOA_filter_en = 0; %   0 = disable 1 = reg enable 2 = filter out between angles 
ANGLE_SCALE = 255;
LowerLimAoA = 193*255/ANGLE_SCALE;
UpperLimAoA = 196*255/ANGLE_SCALE;

BandFilter_en = 0;
Band_forFilter = 2; % 0 = E  3 = J
Band_forFilter2 = 2; % can make this the same as 1 to filter one at a time

QuadFilter_en = 0;
Quad_forFilter = 3; % 0 = E  3 = J
% Quad_forFilter2 = 0; % can make this the same as 1 to filter one at a time

if (exist('band', 'var') == 0)
    band = ones(size(rfv))*2;
end
if (exist('N', 'var') == 0)
    N = ones(size(rfv))*2;
end

if (QuadFilter_en || TimeWindow_en || AOA_filter_en || PowerThres_en || BandFilter_en)
    PDW = [PDW(:,1:5) px py pa pb pc pd N ant rfv band];
    PDW_original = PDW;
end
if (Rfv_en)
   PDW((rfv(:) == 1),1) = 0; % will be 20 when plotted (according to formula for Freq)
end
if (BandFilter_en)
    PDW( (band ~= Band_forFilter & band ~= Band_forFilter2 ),:) = [];   
end 
if(QuadFilter_en)
  PDW(N~=Quad_forFilter,:) = []; 
end
if(TimeWindow_en)
    if (EndTime == 0)
        EndTime = PDW(end,5)*50e-9;
    end
    PDW(((PDW(:,5) < (StartTime/50e-9)) | (PDW(:,5) > (EndTime/50e-9))),:) = [];       
end


if (AOA_filter_en)
    if (AOA_filter_en == 1)
        PDW(PDW(:,2) < LowerLimAoA  | PDW(:,2) > UpperLimAoA,:) = [];  
    else % if AOA en = 2 - filter out the data between the two angle limits
        PDW(PDW(:,2) > LowerLimAoA  & PDW(:,2) < UpperLimAoA,:) = [];  
    end
end
if (PowerThres_en)
    if (PxPySum)
        PDW(((PDW(:,6) + PDW(:,7)) < lowerLim | (PDW(:,6) + PDW(:,7)) > upperLim),:) = [];   
    else
        PDW((((PDW(:,6) > upperLim) | (PDW(:,7) > upperLim )) | ((PDW(:,6)  < lowerLim ) & (PDW(:,7)  < lowerLim )) ),:) = [];
    end    
end

 
if (QuadFilter_en ||TimeWindow_en || AOA_filter_en || PowerThres_en || BandFilter_en)
    px = PDW(:,6); py = PDW(:,7); pa = PDW(:,8); pb = PDW(:,9);pc = PDW(:,10);pd = PDW(:,11); 
    N = PDW(:,12); rfv  = PDW(:,14); ant = PDW(:,13); band = PDW(:,15);
    PDW = PDW(:,1:5);
end



    figure,
    % Px
    handlePx = plot( PDW(:,5)*50e-9, 100+(px*16*0.0318-75), '.r' )
    set(handlePx,'Visible','on');
%   set(handlePx,'Visible','off'); 
% set(handlePx, 'Color','m');
    hold on
    % Py
    handlePy = plot( PDW(:,5)*50e-9, 100+(py*16*0.0318-75), '.g' )
    set(handlePy,'Visible','on');
    % RF (scaled down to 100)
    handleRF = plot( PDW(:,5)*50e-9, 100*(0.244*PDW(:,1)+2000), '.m' )
    set(handleRF,'Visible','on');
    % AOA (on [0 255] scale)
    handleAOA = plot( PDW(:,5)*50e-9, PDW(:,2), '.c' )
    set(handleAOA,'Visible','on');
   % PW                          BLACK
    handlePW = plot( PDW(:,5)*50e-9, PDW(:,4)*2.5, '.k' )
    set(handlePW,'Visible','off');
%     set(handlePW,'Visible','on');
    % Quadrant code (scaled to 50)   YELLOW
    handleN = plot( PDW(:,5)*50e-9, N(:,1)*50, '.y', 'LineWidth', 2 )
    set(handleN, 'Visible', 'off'); 

    %Diff TOA (scaled to 1e6)
    handlediffTOA = plot( PDW(1:end-1,5)*50e-9, diff(PDW(:,5))/20 , ':.g');
%   set(handlediffTOA, 'Visible', 'off'); 
   set(handlediffTOA, 'Visible', 'on'); 
    % Pa
    handlePa = plot( PDW(:,5)*50e-9, 100+(pa*16*0.0318-75), '.b' );
    set(handlePa, 'Visible', 'off');
%     set(handlePa, 'Visible', 'on'); 
    % Pb
    handlePb = plot( PDW(:,5)*50e-9, 100+(pb*16*0.0318-75), '.c' );
    set(handlePb, 'Visible', 'off');
%     set(handlePb, 'Visible', 'on'); 
    % Pc
    handlePc = plot( PDW(:,5)*50e-9, 100+(pc*16*0.0318-75), 'xm' );
    set(handlePc, 'Visible', 'off');
%     set(handlePc, 'Visible', 'on'); 
    % Pd
    handlePd = plot( PDW(:,5)*50e-9, 100+(pd*16*0.0318-75), '.k' );
    set(handlePd, 'Visible', 'off');
%     set(handlePd, 'Visible', 'on');

    handleAnt = plot( PDW(:,5)*50e-9, ant*50, '.y' );
    
    legend('Px', 'Py', 'RF', 'AOA', 'PW', 'N', 'diffTOA', 'Pa','Pb','Pc','Pd', 'ant');
zoom xon;