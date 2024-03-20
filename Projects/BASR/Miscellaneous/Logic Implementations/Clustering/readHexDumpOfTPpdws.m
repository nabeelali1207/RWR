%PDW structure was updated in May2020
function pdws = readHexDumpOfTPpdws(varargin)

	if(isempty(varargin))
		[pdwFileName, pdwFilePath] = uigetfile({'*.*'}, 'Pick the PUP1 live pdws file');
		pathAndFileName = fullfile(char(pdwFilePath), char(pdwFileName));
	else
		pathAndFileName = varargin{1};
	end
	fid = fopen(pathAndFileName);
	rawData = uint8(fread(fid, Inf, 'uint8=>uint8'));
	fclose(fid);
	pdws = [];

	if(length(rawData) < 32)
		return;
	end

	rawData = rawData(1: 32 * (floor(length(rawData) / 32)));
	t = reshape(rawData, 32, [])';
	ant = t(:, 1);
	st = t(:, 5);

	rf = typecast(reshape(t(:, 9:10)', [], 1), 'uint16');
	pw = typecast(reshape(t(:, 11:12)',[] , 1), 'uint16');
	rfv = t(:, 13);

	band = t(:, 15);
	N = t(:, 16);

	temp2 = typecast(reshape(t(:, 17:24)', [], 1),'int64');
	toa = bitand(temp2,hex2dec('FFFFFFFFFFFF'));
	rfAmp = bitshift(temp2, -48);

	pa = typecast(reshape(t(:, 25:26)', [], 1), 'uint16');
	pb = typecast(reshape(t(:, 27:28)', [] ,1), 'uint16');
	pc = typecast(reshape(t(:, 29:30)', [], 1), 'uint16');
	pd = typecast(reshape(t(:, 31:32)', [], 1), 'uint16');

	pdws = table();
	pdws.TOA = toa;
	pdws.ST = st;
	pdws.PA = pa;
	pdws.PB = pb;
	pdws.PC = pc;
	pdws.PD = pd;
	pdws.PW = pw;

	% aoa=calculateAOAFromPowers(pa,pb,pc,pd);
	% pdws.AOA=aoa';

	pdws.RFAmp = rfAmp;
	pdws.RF = rf;
	pdws.RF_V = (rfv == 1);
	pdws.Ant = ant;
	pdws.Band = band;
	pdws.N = N;
	% pdws.Px=max(pdws.PA,pdws.PC);
	% pdws.Py=max(pdws.PB,pdws.PD);
	% pdws.Quad= quad;

	% pdws = sortrows(pdws,'TOA','ascend');
end