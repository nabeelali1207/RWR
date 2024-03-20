function resSig= downSample_DLVACondition_Addnoise(videoSig)
% downSampling 
resSig=zeros(4,length(videoSig)/16,'single');
resSig(1,:)=downsample(videoSig(1,:),16);
resSig(2,:)=downsample(videoSig(2,:),16);
resSig(3,:)=downsample(videoSig(3,:),16);
resSig(4,:)=downsample(videoSig(4,:),16);

% dlvaConditioning
zeroIndexes=(resSig==0);
resSig=3.704+0.539.*log10(resSig);
resSig(resSig>3.704)=3.704;
resSig(zeroIndexes)=0;
resSig(resSig<0)=0;
% noise addition
nonZeroIndexes = ~zeroIndexes;
noise=1e-3*randi([-200,200],[4,length(resSig)]);
noise(nonZeroIndexes)=(noise(nonZeroIndexes)/10);

resSig=resSig+noise;

end