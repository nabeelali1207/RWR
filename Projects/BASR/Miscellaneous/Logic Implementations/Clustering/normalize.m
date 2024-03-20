function [normalizedData] = normalize(dataIn, minRange, maxRange)

	if (maxRange - minRange) ~= 0
		range = maxRange - minRange;
	else
		range = maxRange;
	end
	
	normalizedData = (dataIn - minRange) ./ range;

end