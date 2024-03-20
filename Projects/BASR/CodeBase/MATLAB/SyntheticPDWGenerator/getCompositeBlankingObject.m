function obj=getCompositeBlankingObject(FS,EmittersArray)
obj                      = BlankingClass();
compBlankFlag            = EmittersArray.CompositeBlankFlag;
blkParameters.Type       = EmittersArray.CompositeBlankingType;
blkParameters.startPoint = EmittersArray.BlankTimestart;
blkParameters.pulseWidth = EmittersArray.Blankingpw;
blkParameters.pulseCount = EmittersArray.BlankPulses;
blkParameters.PRI        = EmittersArray.Blankingperiod;
obj.Initialize(compBlankFlag,FS,blkParameters);
end