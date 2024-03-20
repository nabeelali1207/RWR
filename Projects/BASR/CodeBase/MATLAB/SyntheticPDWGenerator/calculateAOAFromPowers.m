function angleReturn=calculateAOAFromPowers(PA,PB,PC,PD,varargin)
load('Emitter/AoaFinding_anteenapattern.mat');
load('Emitter/AoaFinding_theta_table_updated_20_dec2018.mat');

if(isempty(varargin))
    Quad           =bi2de([~(PA>PC),~(PB>PD)]);
else
    Quad=varargin{1};
end

ang = 0:1:360;
ang180 = wrapTo180(ang)';

%sort anges and values
for i = 1:length(ang180)
    
res(i,1) = find(ang180(i) == aoa);
val(i,1) = data(res(i,1),1);


end

val = val';

% val = val(1:end-1,1);




% figure
% angle_power_polar_plot = polarpattern(ang,finalValue_A);
% add(angle_power_polar_plot,ang,finalValue_B);
% add(angle_power_polar_plot,ang,finalValue_C);
% add(angle_power_polar_plot,ang,finalValue_D);

% figure
% plot(ang,finalValue_A)
% hold on
% plot(ang,finalValue_B)
% plot(ang,finalValue_C)
% plot(ang,finalValue_D)
% xlabel('Angle of arrival of incoming signal');
% ylabel('Gain (dB)');







ch_A=PA;
ch_B=PB;
ch_C=PC;
ch_D=PD;

n = zeros(1,361);
n(1,1:44) = 2;
n(1,45:134) = 0;
n(1,135:224) = 1;
n(1,225:314) = 3;
n(1,315:361) = 2;


AOA_TP=zeros(1,length(PA));
newAOA=zeros(1,length(PA));
for angle = 0:length(PA)-1
    angindex = angle + 1;

    quad = Quad(angindex);
    adc1 = ceil((ch_A(angindex) ));
    adc2 = ceil((ch_B(angindex) ));
    adc3 = ceil((ch_C(angindex) ));
    adc4 = ceil((ch_D(angindex) ));


    if(quad == 0)
        px = adc1;
        py = adc2;
        tempQ = 0;
    elseif(quad == 1)
        px = adc3;
        py = adc2;
        tempQ = 4096;
    elseif(quad == 2)
        px = adc1;
        py = adc4;
        tempQ = 8192;
    elseif(quad == 3)
        px = adc3;
        py = adc4;
        tempQ = 12288;
    end

    % Extract AOA
    if((px+py) > 50*16) %|| (py > DynamicUpperThreshold_I_Band))

        if((px < 10*16) || (py < 10*16))

            if(px > py)

                if( (quad == 0) || (quad == 2) )
                    AOA_TP(angindex) = 32;
                else
                    AOA_TP(angindex) = 160;
                end
            else
                if( (quad == 2) || (quad == 3) )
                    AOA_TP(angindex) =  224;
                else
                    AOA_TP(angindex) =  96;
                end
            end
        else        
            temp = dec2bin((typecast(int16(px - py),'uint16'))/16);
            r = bin2dec(temp);
         %   AOA_TP(angindex) = thetatable(bitor(tempQ, r)+1).value;
         AOA_TP(angindex) = thetatable2(bitor(tempQ, r)+1);   
         test(angindex)=bitor(tempQ, r)+1;
        end
        newAOA(angindex) = ((AOA_TP(angindex)/255)*360);
        %newAOA(angindex) = (AOA_TP(angindex));
    end


end


angleReturn=newAOA;
end