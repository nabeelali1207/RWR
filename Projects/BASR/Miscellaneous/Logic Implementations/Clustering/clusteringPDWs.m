function [] = clusteringPDWs(data)

	eps = .1;
	minPts = 5;

	data(:, 1) = normalize(data(:, 1), 0, 18000); % RF
	data(:, 3) = normalize(data(:, 3), 0, 1000); % PW
	data(:, 4:7) = normalize(data(:, 4:7), 0, 4096); % Powers

	[dataInfo, clusterID] = myDBSCAN(data, eps, minPts);

	hold on
	for i = 0:clusterID
		% plot(data(dataInfo(:, 2) == i, 1), data(dataInfo(:, 2) == i, 2), '.', 'MarkerSize', 5) 
		plot(1e-5 * pdws.TOA(dataInfo(:, 2) == i, 1), data(dataInfo(:, 2) == i, 1), '.', 'MarkerSize', 5) 	
	end
	hold off
	
end