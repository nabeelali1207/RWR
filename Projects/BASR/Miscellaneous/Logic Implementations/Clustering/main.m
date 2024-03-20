
pdws = readHexDumpOfTPpdws();

data = [pdws.RF pdws.PW pdws.PA pdws.PB pdws.PC pdws.PD];
clusterTime = 0.03;
rangeIndex = find((1e-8 .* double(pdws.TOA)) > clusterTime, 1, 'first');
data = double(data(1:(rangeIndex - 1), :));

clusteringPDWs(data);