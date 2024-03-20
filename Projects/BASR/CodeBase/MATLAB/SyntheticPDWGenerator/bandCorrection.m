pdws.DIFM_BAND = pdws.DIFM_BAND-1;
i=pdws.DIFM_BAND<0;
pdws.DIFM_BAND(i)=0;
unique(pdws.DIFM_BAND)
convertPdwsToPlaybackFormat_TP(pdws);