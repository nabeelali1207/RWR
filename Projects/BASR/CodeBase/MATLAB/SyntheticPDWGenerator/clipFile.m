Ne=1000;
ant = ant(1:Ne);
band = band(1:Ne);
lpi = lpi(1:Ne);
lpiv = lpiv (1:Ne);
N = N(1:Ne);
pa= pa(1:Ne);
pb= pb(1:Ne);
pc= pc(1:Ne);
pd= pd(1:Ne);
PDW= PDW(1:Ne,:);
percent_err= percent_err(1:Ne);
pop= pop(1:Ne);
pop_err= pop_err(1:Ne);
pulse_pop_id= pulse_pop_id(1:Ne);
pw_err= pw_err(1:Ne);
px= px(1:Ne);
py= py(1:Ne);
rf= rf(1:Ne);
rfv= rfv(1:Ne);
sep_err= sep_err(1:Ne);
Sfall_err= Sfall_err(1:Ne);
Srise_err= Srise_err(1:Ne);
toa_err= toa_err(1:Ne);
clear Ne

save("pri20ms.mat",'ant','band','lpi','lpiv','N','pa','pb','pc','pd','PDW','percent_err','pop','pop_err','pulse_pop_id','pw_err','px','py','rf','rfv','sep_err','Sfall_err','Srise_err','toa_err');