function fspl=calculatefspl(distance,rf)
fspl = 2.*10.*log10((4.*pi.*distance.*rf)./3e8);
end