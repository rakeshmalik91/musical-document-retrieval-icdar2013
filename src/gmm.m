function gmm()
fileID = fopen('DATA/zernike_feature.txt', 'r');
n = fscanf(fileID, '%d', 1);
for(i = 1:n)
    F = fscanf(fileID, '%g', [n, inf]);
    L = fscanf(fileID, '%s', 1);
    obj = gmdistribution.fit(F,2);
    P= posterior(obj,F);
end
fclose(fileID);