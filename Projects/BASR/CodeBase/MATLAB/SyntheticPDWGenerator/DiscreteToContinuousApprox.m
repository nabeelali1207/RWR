function res=DiscreteToContinuousApprox(sampledValues,continuousValues)

res=spline(1:length(sampledValues),sampledValues,continuousValues);
end