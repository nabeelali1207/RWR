% file splitter
filename = 'Case_7b_sPDWs';
SinglePower = 1;

newLength = round(length(pa));
PDW = PDW(1:newLength,:);
pa = pa(1:newLength);
pb = pb(1:newLength);
pc = pc(1:newLength);
pd = pd(1:newLength);
px = px(1:newLength);
py = py(1:newLength);
rfv = rfv(1:newLength);
N = N(1:newLength);
band = band(1:newLength);
ant = ant(1:newLength);
if(SinglePower == 1)
pa = 0.*pa;
pb = 0.*pb;
pd = 0.*pd;
py = 0.*py;
rfv = 0.*rfv;
end
% newPDWs = band == Band_forFilter;
% PDW = PDW(newPDWs,:);
% pa = pa(newPDWs);
% pb = pb(newPDWs);
% pc = pc(newPDWs);
% pd = pd(newPDWs);
% px = px(newPDWs);
% py = py(newPDWs);
% rfv = rfv(newPDWs);
% N = N(newPDWs);
% band = band(newPDWs);
% ant = ant(newPDWs);
save(filename,'PDW','px','py','pa','pb','pc','pd','N','ant','rfv','band')