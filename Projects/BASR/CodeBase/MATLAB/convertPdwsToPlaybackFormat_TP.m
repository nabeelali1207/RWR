%PDW structure was updated in May2020
function convertPdwsToPlaybackFormat_TP(pdws,varargin)

    pdws = sortrows(pdws,'TOA','ascend');
    if(isempty(varargin))
        fileName="71_default_library_testCase.bin";
    else
        fileName=varargin{1};
    end
    
    rawData=zeros(size(pdws,1),32,'uint8');
    
    ant=uint8(floor(pdws.DIFM_ANT));
    ant=reshape(ant,1,[])';
    
    popFlag=uint8(floor(pdws.POP_ERR));
    popFlag=reshape(popFlag,1,[])';
%     popFlag=uint8(0*floor(pdws.DIFM_ANT));
%     popFlag=reshape(popFlag,1,[])';
    
    lpiFlag=uint8(0*floor(pdws.DIFM_ANT));
    lpiFlag=reshape(lpiFlag,1,[])';
    
    lpiValidFlag=uint8(0*floor(pdws.DIFM_ANT));
    lpiValidFlag=reshape(lpiValidFlag,1,[])';
    
    blanking=uint8(0*floor(pdws.DIFM_ANT));
    blanking=reshape(blanking,1,[])';
    
    selfTestFlag=uint8(0*floor(pdws.DIFM_ANT));
    selfTestFlag=reshape(selfTestFlag,1,[])';
    
    filterAppliedFalg=uint8(0*floor(pdws.DIFM_ANT));
    filterAppliedFalg=reshape(filterAppliedFalg,1,[])';
   
    popInfoFlag=uint8(0*floor(pdws.DIFM_ANT));
    popInfoFlag=reshape(popInfoFlag,1,[])';
    
%     rf = uint16(floor(pdws.RF));
    rf = uint16(floor(pdws.RF/1e6));
    rf(pdws.RF_V==0)=0;
    rf=typecast(bitand(rf,hex2dec('FFFF')),'uint8');
    rf=reshape(rf,2,[])';
    
    pw=typecast(bitand(uint16(floor(pdws.PW)),hex2dec('FFFF')),'uint8');
    pw=reshape(pw,2,[])';
    
    rfValidFlag=uint8(pdws.RF_V==1);
    rfValidFlag=reshape(rfValidFlag,1,[])';
    
%     ovFlag=uint8(0*floor(pdws.DIFM_ANT));
%     ovFlag=reshape(ovFlag,1,[])';
    ovFlag=uint8(0*floor(pdws.DIFM_ANT));
    ovFlag=reshape(ovFlag,1,[])';
    
    band=uint8(pdws.DIFM_BAND);
    band=reshape(band,1,[])';
    
    N=uint8(0*pdws.DIFM_ANT);
    N=reshape(N,1,[])';
    
    rfAmp=bitshift(bitand(uint64(1500 + 0*pdws.PA),hex2dec('FFFF')),48);
    toaRFAmp=bitand(uint64(pdws.TOA),hex2dec('FFFFFFFFFFF'));
    toaRFAmp2=bitor(toaRFAmp,rfAmp);
    toa=typecast(toaRFAmp2,'uint8');
    toa=reshape(toa,8,[])';
    
    pa=typecast(uint16(pdws.PA),'uint8');
    pa=reshape(pa,2,[])';
    pb=typecast(uint16(pdws.PB),'uint8');
    pb=reshape(pb,2,[])';
    pc=typecast(uint16(pdws.PC),'uint8');
    pc=reshape(pc,2,[])';
    pd=typecast(uint16(pdws.PD),'uint8');
    pd=reshape(pd,2,[])';
    
    
    rawData(:,1)=ant;
    rawData(:,2)=popFlag;
    rawData(:,3)=lpiFlag;
    rawData(:,4)=lpiValidFlag;
    rawData(:,5)=blanking;
    rawData(:,6)=selfTestFlag;
    rawData(:,7)=filterAppliedFalg;
    rawData(:,8)=popInfoFlag;
    
    rawData(:,9:10)=rf;
    rawData(:,11:12)=pw;
    
    rawData(:,13)=rfValidFlag;
    rawData(:,14)=ovFlag;
    rawData(:,15)=band;
    rawData(:,16)=N;
    
    
    rawData(:,17:24)=toa;
    rawData(:,25:26)=pa;
    rawData(:,27:28)=pb;
    rawData(:,29:30)=pc;
    rawData(:,31:32)=pd;
    
    
    rawData1=reshape(rawData',1,[]);
    fid=fopen(fileName,'w');
    fwrite(fid,rawData1);
    fclose(fid);
end

% sum(origpdws.TOA==pdws.TOA)
% sum(origpdws.PW==pdws.PW)
% sum(origpdws.RF==pdws.RF)
% sum(origpdws.RF_V==pdws.RF_V)
% sum(origpdws.PA==pdws.PA)
% sum(origpdws.PB==pdws.PB)
% sum(origpdws.PC==pdws.PC)
% sum(origpdws.PD==pdws.PD)