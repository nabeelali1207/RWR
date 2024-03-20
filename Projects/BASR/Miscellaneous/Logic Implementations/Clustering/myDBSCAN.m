function [dataInfo, clusterID] = myDBSCAN(dataIn, eps, minPts)

	% [m, n] = size(dataIn);
	data = dataIn;
	buffer = [];
	%distance = zeros(m, n);
	dataInfo = [zeros(size(data, 1), 1) (-1 * ones(size(data, 1), 1))]; % column1 = visted or not, column2 = cluster group (-1 = none, 0 = noise)
	clusterID = 1;
	clusterSize = 0;

	buffer(end + 1) = 1;

	while any(dataInfo(:, 1) == 0)
		
		if isempty(buffer) && (clusterSize > 0) % check if buffer is empty
			clusterSize = 0;
			buffer = min(find(dataInfo(:, 1) == 0));
		elseif isempty(buffer)
			buffer = min(find(dataInfo(:, 1) == 0));
		end
		
		index = buffer(1);
		
		if dataInfo(index, 1) == 1 % check if index has been visited before
			buffer = buffer(2: end);
			continue;
		else
			dataInfo(index, 1) = 1;
		end
		
		tempCenter = data(index, :);
		distance = sqrt(sum((data - tempCenter) .^ 2, 2));
		neighbors = find(distance <= eps);
		neighbors = neighbors(dataInfo(neighbors, 1) == 0);
		
		% 	if isempty(buffer) && (size(neighbors, 1) < minPts) % noise check
		% 		dataInfo(index, 2) = 0;
		% 	elseif isempty(buffer) && (size(neighbors, 1) > minPts) % new cluster
		% 		clusterID = clusterID + 1;
		% 	end
		
		if not(isempty(buffer))	&& ((size(neighbors, 1) < minPts) && (clusterSize == 0)) % noise
			% 		clusterID = clusterID - 1;
			dataInfo(index, 2) = 0;
		elseif not(isempty(buffer))	&& (size(neighbors, 1) < minPts)
			dataInfo(index, 2) = clusterID;
			clusterSize = clusterSize + 1;
		elseif not(isempty(buffer))	&& (size(neighbors, 1) > minPts) % cluster
			if clusterSize == 0
				clusterID = clusterID + 1;
			end
			dataInfo(neighbors, 2) = clusterID;
			[~, tempIndex] = sort(distance(neighbors), 'descend');
			tempIndex = neighbors(tempIndex);
			tempIndex = setdiff(tempIndex, buffer);
			buffer = [buffer; tempIndex];
			clusterSize = clusterSize + 1;
		end
		
		buffer = buffer(2: end);
	end
end