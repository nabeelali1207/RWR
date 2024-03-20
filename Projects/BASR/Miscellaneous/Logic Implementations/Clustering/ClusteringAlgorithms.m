close all
clear all
clc



% % % 2023.02.22
% % % K-Means implementation 1
% 
% load fisheriris
% data = meas(:,3:4);
% rng default; % For reproducibility
% data = [randn(100,2)*0.75+ones(100,2);
%     randn(100,2)*0.5-ones(100,2)];
% 
% data_m = data;
% k = 2;
% 
% % center1(1) = randi([ceil(min(data(:, 1))), floor(max(data(:, 1)))]);
% % center1(2) = randi([ceil(min(data(:, 2))), floor(max(data(:, 2)))]);
% % center2(1) = randi([ceil(min(data(:, 1))), floor(max(data(:, 1)))]);
% % center2(2) = randi([ceil(min(data(:, 2))), floor(max(data(:, 2)))]);
% center(1, :) = [0, 0];
% center(2, :) = [.1, .1];
% 
% for i = 1: 3
% 	
% 	g1 = [];
% 	g2 = [];
% 	
% % 	d1(:, 1) = sqrt((data(:, 1) - center1(1)) .^ 2);
% % 	d1(:, 2) = sqrt((data(:, 2) - center1(2)) .^ 2);
% 	distance1 = sqrt((data(:, 1) - center(1, 1)) .^ 2 + (data(:, 2) - center(1, 2)) .^ 2);
% % 	d2(:, 1) = sqrt((data(:, 1) - center2(1)) .^ 2);
% % 	d2(:, 2) = sqrt((data(:, 2) - center2(2)) .^ 2);
% 	distance2 = sqrt((data(:, 1) - center(2, 1)) .^ 2 + (data(:, 2) - center(2, 2)) .^ 2);
% % 	distance1 = pdist2(data, center1);
% % 	distance2 = pdist2(data, center2);
% 	
% 	for j = 1: length(data)
% 		if (distance1(j) < distance2(j))
% 			g1(end + 1, :) = data(j, :);
% 		else
% 			g2(end + 1, :) = data(j, :);
% 		end
% 	end
% 	
% 	if not(isempty(g1))
% 		center(1, 1) = mean(g1(:, 1));
% 		center(1, 2) = mean(g1(:, 2));
% 	end
% 	if not(isempty(g2))
% 		center(2, 1) = mean(g2(:, 1));
% 		center(2, 2) = mean(g2(:, 2));
% 	end
% 	
% end
% 
% hold on
% % plot(data(:, 1), data(:, 2), '.')
% plot(g1(:, 1), g1(:, 2), 'r.', 'MarkerSize', 12)
% plot(g2(:, 1), g2(:, 2), 'b.', 'MarkerSize', 12)
% plot(center(1, 1), center(1, 2), '*')
% plot(center(2, 1), center(2, 2), 'kx')
% hold off










% % % 2023.02.23
% % % K-Means implementation 2
% 
% load fisheriris
% 
% k = 2;
% data = meas(:,3:4);
% % rng default; % For reproducibility
% % data = [randn(100,2)*0.75+ones(100,2);
% %     randn(100,2)*0.5-ones(100,2)];
% 
% colors = ['r', 'g', 'b', 'm', 'k', 'c'];
% symbols = ['x', '+', '*', 'o', 's', 'd'];
% distance = zeros(size(data, 1), k);
% [m, n] = size(data);
% center = zeros(k, n);
% for i = 1: k
% 	center(i, :) =  data(i, :);
% end
% 
% for i = 1: 20
% 	for j = 1: size(center, 1)
% 		tempCenter = center(j, :);
% 		tempDistance = sqrt(sum((data - tempCenter) .^ 2, 2));
% 		distance(:, j) = tempDistance;
% 	end
% 	
% 	[M, I] = min(distance, [], 2);
% 	
% 	for j = 1: k
% 		if not(isempty(data(I == j, :)))
% 			center(j, :) = mean(data(I == j, :));
% 		end
% 	end
% end
% 
% hold on
% for i = 1: m
% 	color_index = mod(i - 1, length(colors)) + 1;
%     color = colors(color_index); 
%     plot(data(I == i, 1), data(I == i, 2), '.', 'MarkerSize', 10, 'Color', color);
% end
% for i = 1: k
% 	symbol_index = mod(i - 1, length(symbols)) + 1;
% 	symbol = symbols(symbol_index);
% 	plot(center(i, 1), center(i, 2), symbol, 'MarkerSize', 15)
% end
% hold off










% % % 2023.02.23
% % % DBSCAN failed implementation 1
% 
% rng('default')
% N = 300; 
% r1 = 0.5;
% r2 = 5;
% theta = linspace(0, 2 * pi, N)';
% X1 = r1 * [cos(theta), sin(theta)] + rand(N, 1);
% X2 = r2 * [cos(theta), sin(theta)] + rand(N, 1);
% 
% data = [X1; X2];
% eps = .5;
% minPts = 5;
% 
% [m, n] = size(data);
% distance = zeros(m, n);
% dataInfo = [zeros(size(data, 1), 1) (-1 * ones(size(data, 1), 1))]; % column1 = visted or not, column2 = cluster group (-1 = none, 0 = noise)
% clusterID = 0;
% 
% 
% i = 1;
% while any(dataInfo(:, 1) == 0)
% 
% 	if dataInfo(i, 1) == 1
% 		i = i + 1;
% 		continue;
% 	end
% 	
% 	tempCenter = data(i, :);
% 	distance = sqrt(sum((data - tempCenter) .^ 2, 2));
% 	neighbors = find(distance <= eps);
% 	
% 	if (size(neighbors, 1) < minPts) && (dataInfo(i, 2) == -1)
% 		dataInfo(i, 2) = 0;
% 	elseif (size(neighbors, 1) > minPts) && (dataInfo(i, 2) == -1)
% 		clusterID = clusterID + 1;
% 		dataInfo(i, 2) = clusterID;
% 		dataInfo(neighbors, 2) = clusterID;
% 		[temp, i] = max(distance(neighbors));
% 		continue;
% 	end
% 	
% 	dataInfo(i, 1) = 1;
% 	
% 	i = i + 1;
% 	if (i > size(dataInfo, 1)) && any(dataInfo(:, 1) == 0)
% 		i = find(dataInfo(:, 1) == 0, 1, 'first');
% 	end
% end
% 
% plot(data(:,1),data(:,2), '.')









% % % 2023.02.24
% % % DBSCAN failed implementation 2
% 
% rng('default')
% N = 300; 
% r1 = 0.5;
% r2 = 5;
% theta = linspace(0, 2 * pi, N)';
% X1 = r1 * [cos(theta), sin(theta)] + rand(N, 1);
% X2 = r2 * [cos(theta), sin(theta)] + rand(N, 1);
% 
% dataIn = [X1; X2];
% eps = .1;
% minPts = 5;
% 
% data = (dataIn - min(dataIn)) ./ (max(dataIn) - min(dataIn));
% 
% buffer = [];
% [m, n] = size(data);
% distance = zeros(m, n);
% dataInfo = [zeros(size(data, 1), 1) (-1 * ones(size(data, 1), 1))]; % column1 = visted or not, column2 = cluster group (-1 = none, 0 = noise)
% clusterID = 0;
% 
% buffer(end + 1) = 1;
% 
% while any(dataInfo(:, 1) == 0)
% 
% 	index = buffer(1);
% 	
% 	if dataInfo(index, 1) == 1
% 		buffer = buffer(2: end);
% 		continue;
% 	else
% 		dataInfo(index, 1) = 1;
% 	end
% 	
% 	tempCenter = data(index, :);
% 	distance = sqrt(sum((data - tempCenter) .^ 2, 2));
% 	neighbors = find(distance <= eps);
% 	
% 	if (size(neighbors, 1) < minPts) && (dataInfo(index, 2) == -1)
% 		dataInfo(index, 2) = 0;
% 	elseif (size(neighbors, 1) > minPts) && (dataInfo(index, 2) == -1)
% 		if length(buffer) < 1
% 			clusterID = clusterID + 1;
% 		end
% 		dataInfo(neighbors, 2) = clusterID;
% 		[temp, tempIndex] = sort(distance(neighbors), 'descend');
% 		buffer = [buffer; tempIndex];
% 	end
% 	
% 	buffer = buffer(2: end);
% end
% 
% hold on
% for i = 1: clusterID
% 	plot(data(dataInfo(:, 2), 1), data(dataInfo(:, 2), 2), '.')
% end














% % % 2023.03.01
% % % DBSCAN working implementation 3
% 
% rng('default')
% N = 300; 
% r1 = 0.5;
% r2 = 5;
% theta = linspace(0, 2 * pi, N)';
% X1 = r1 * [cos(theta), sin(theta)] + rand(N, 1);
% X2 = r2 * [cos(theta), sin(theta)] + rand(N, 1);
% dataIn = [X1; X2];
% % rng default; % For reproducibility
% % dataIn = [randn(100,2)*0.75+ones(100,2);
% %     randn(100,2)*0.5-ones(100,2)];
% eps = .1;
% minPts = 5;
% 
% data = (dataIn - min(dataIn)) ./ (max(dataIn) - min(dataIn));
% 
% buffer = [];
% [m, n] = size(data);
% distance = zeros(m, n);
% dataInfo = [zeros(size(data, 1), 1) (-1 * ones(size(data, 1), 1))]; % column1 = visted or not, column2 = cluster group (-1 = none, 0 = noise)
% clusterID = 1;
% clusterSize = 0;
% 
% buffer(end + 1) = 1;
% 
% while any(dataInfo(:, 1) == 0)
% 
% 	if isempty(buffer) && (clusterSize > 0) % check if buffer is empty
% 		clusterID = clusterID + 1;
% 		clusterSize = 0;
% 		buffer = min(find(dataInfo(:, 1) == 0));
% 	elseif isempty(buffer)
% 		buffer = min(find(dataInfo(:, 1) == 0));
% 	end
% 	
% 	index = buffer(1);
% 	
% 	if dataInfo(index, 1) == 1 % check if index has been visited before
% 		buffer = buffer(2: end);
% 		continue;
% 	else
% 		dataInfo(index, 1) = 1;
% 	end
% 	
% 	tempCenter = data(index, :);
% 	distance = sqrt(sum((data - tempCenter) .^ 2, 2));
% 	neighbors = find(distance <= eps);
% 	neighbors = neighbors(dataInfo(neighbors, 1) == 0);
% 	
% % 	if isempty(buffer) && (size(neighbors, 1) < minPts) % noise check
% % 		dataInfo(index, 2) = 0;
% % 	elseif isempty(buffer) && (size(neighbors, 1) > minPts) % new cluster
% % 		clusterID = clusterID + 1;
% % 	end
% 	
% 	if not(isempty(buffer))	&& ((size(neighbors, 1) < minPts) && (clusterSize == 0)) % noise
% % 		clusterID = clusterID - 1;
% 		dataInfo(neighbors, 2) = 0;
% 	elseif not(isempty(buffer))	&& (size(neighbors, 1) < minPts)
% 		dataInfo(index, 2) = clusterID;
% 		clusterSize = clusterSize + 1;
% 	elseif not(isempty(buffer))	&& (size(neighbors, 1) > minPts) % cluster
% 		dataInfo(neighbors, 2) = clusterID;
% 		[temp, tempIndex] = sort(distance(neighbors), 'descend');
% 		tempIndex = neighbors(tempIndex);
% 		tempIndex = setdiff(tempIndex, buffer);
% 		buffer = [buffer; tempIndex];
% 		clusterSize = clusterSize + 1;
% 	end
% 	
% 	buffer = buffer(2: end);
% end
% 
% hold on
% for i = 0: clusterID
% 	plot(data(dataInfo(:, 2) == i, 1), data(dataInfo(:, 2) == i, 2), '.') 	
% end
% 
% % plot(data(:,1),data(:,2), '.')












% % 2023.03.01
% % DBSCAN modular implementation 4

% rng('default')
% N = 300; 
% r1 = 0.5;
% r2 = 5;
% theta = linspace(0, 2 * pi, N)';
% X1 = r1 * [cos(theta), sin(theta)] + rand(N, 1);
% X2 = r2 * [cos(theta), sin(theta)] + rand(N, 1);
% dataIn = [X1; X2];
% % rng default; % For reproducibility
% % dataIn = [randn(100,2)*0.75+ones(100,2);
% %     randn(100,2)*0.5-ones(100,2)];
% eps = .1;
% minPts = 5;

function [data, dataInfo, clusterID] = myDBSCAN(dataIn, eps, minPts)

	data = (dataIn - min(dataIn)) ./ (max(dataIn) - min(dataIn));

	buffer = [];
	[m, n] = size(data);
	distance = zeros(m, n);
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
			dataInfo(neighbors, 2) = 0;
		elseif not(isempty(buffer))	&& (size(neighbors, 1) < minPts)
			dataInfo(index, 2) = clusterID;
			clusterSize = clusterSize + 1;
		elseif not(isempty(buffer))	&& (size(neighbors, 1) > minPts) % cluster
			if clusterSize == 0
				clusterID = clusterID + 1;
			end
			dataInfo(neighbors, 2) = clusterID;
			[temp, tempIndex] = sort(distance(neighbors), 'descend');
			tempIndex = neighbors(tempIndex);
			tempIndex = setdiff(tempIndex, buffer);
			buffer = [buffer; tempIndex];
			clusterSize = clusterSize + 1;
		end
		
		buffer = buffer(2: end);
	end
end

hold on
for i = 0: clusterID
	plot(data(dataInfo(:, 2) == i, 1), data(dataInfo(:, 2) == i, 2), '.') 	
end
hold off

