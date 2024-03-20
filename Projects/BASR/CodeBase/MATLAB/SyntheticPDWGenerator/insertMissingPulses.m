function newPdws=insertMissingPulses(pdws,percentage)
totalPulses = size(pdws,1);
missedPulsesCount=floor(totalPulses * percentage/100);

missingIndices = randperm(totalPulses,missedPulsesCount);
validIndices = setxor([1:totalPulses],[missingIndices]);
newPdws = pdws(validIndices,:);
end