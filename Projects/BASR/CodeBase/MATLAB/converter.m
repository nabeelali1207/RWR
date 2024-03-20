% This converts BASR pdws mat file with name xyz.PDWS to pup_1.5 format
% Please load input file
filename = '0ut';  % output file name
%rf = (PDW(:,1)-2000)/0.244;
% pdws = struct2table(pdws);
pa = double(floor((pa./16)));
pb = double(floor(pb./16));
pc = double(floor(pc./16));
pd = double(floor(pd./16));
px = zeros(length(pa),1);
py = zeros(length(pa),1);
for i = 1:length(pa)
if(pa(i) > pc(i))
    px(i) = double(floor((pa(i))));
else
    px(i) = double(floor((pc(i))));
end
if(pb(i) > pd(i))
    py(i) = double(floor((pb(i))));
else
    py(i) = double(floor((pd(i))));
end
end
rfv = double(~rfv);
rf = double(floor(((rf)/100 -2000)/(0.244)));
% rf = double(floor(((PDW(:,1))/10^6 -2000)/(0.244)));
%rf = pdws.RF;
rf = rf.*~rfv;
TOA = double(floor(PDW(:,4)/5));
pw = double(floor(PDW(:,3)/2.5));
z = zeros(length(pa),1);
ant = double(ant);
N = double(N);
% band = double(pdws.DIFM_BAND);
band = zeros(length(band),1);
PDW = [ rf z z pw TOA];
save(filename,'PDW','px','py','pa','pb','pc','pd','N','ant','rfv','band')
plot(diff(PDW(:,5)),'.');