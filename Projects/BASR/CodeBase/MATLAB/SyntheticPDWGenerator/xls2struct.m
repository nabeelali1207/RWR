function outstruct=xls2struct(xlsFName,sheetnum) %#ok<STOUT>

if nargin<2
    sheetnum=1;
end
if nargin<1,  fprintf('Not enough input argument!\n');return; end
[~,~,raw] = xlsread(xlsFName,sheetnum);
for j = size(raw,1):-1:1 
    if ~isnan(raw{j,1})
        totalrow=j;
        break; 
    end
end 
for j = size(raw,2):-1:1
    if ~isnan(raw{1,j})
        totalcol=j;
        break;
    end
end 
raw=raw(1:totalrow,1:totalcol); header=raw(1,:);
content=raw(2:end,:); 

for i=1:length(header)
    headerName=header{1,i};
    if isnan(headerName)
        headerName = ['Column_' num2str(i)];
    end
    exp='\W{1}'; idx= regexp(headerName,exp); headerName(idx)=[]; 
    if isnumeric(str2double(headerName(1)))
        headerName=['' headerName];
    end
    ifHeaderExist=1;
    while ifHeaderExist==1 && i>1
        names=fields(outstruct); n=FindWhich(names,headerName); 
        if isempty(n)
            ifHeaderExist=0;
        else
            headerName=[headerName '_1']; %#ok<*AGROW>
        end
    end
    totalRecords = length(content(:,i));
    for j = 1 : totalRecords
        
        expression=['outstruct(',num2str(j),').',headerName,'=cell2mat(content(', num2str(j),',', num2str(i), '));'];
        eval(expression);
    end
    
end
end

%% Required sub-functions: 
function n=FindWhich(StringCellArray, TargetString)
sn=1:length(StringCellArray);
A=cellismember(StringCellArray,TargetString);
n=sn(A);
end

function Lia=cellismember(A,B) 
if ischar(B)
    B={B};
end
nanA=cellfun(@isnan, A, 'UniformOutput',false);
nanA=logical(cellfun(@sum, nanA));
A(nanA)={'This Cell is An NaN. '};
nanB=cellfun(@isnan, B, 'UniformOutput',false);
nanB=logical(cellfun(@sum, nanB));
B(nanB)={'This Cell is An NaN. '};
Lia=ismember(A,B);
end 

