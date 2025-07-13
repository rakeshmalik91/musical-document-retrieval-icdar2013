function zernike(DirName, OutName) 
[Files,Bytes,Names] = DIRR(DirName,'name');
fid=fopen(OutName, 'w');
fprintf(fid, '42\n');
for(n = 1:length(Names))
    im = imread(char( Names(n)) );
    m=centersquare(im, 41);
	zs=zernike_bf(41, 11);
	out=zernike_mom(m, zs);    
    fprintf(fid, '%f ', out);
    fprintf(fid, '%s\n', char(Names(n)));
end
fclose(fid);
quit