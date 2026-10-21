function fout = propagation(fin)

    nx = size(fin, 3);
    ny = size(fin, 2);

    ex = [0 1 0 -1 0 1 -1 -1 1];
    ey = [0 0 1 0 -1 1 1 -1 -1];

    ftemp = zeros(9,ny+2, nx+2);
    ftemp(:,2:ny+1,2:nx+1) = fin;
    ftemp(:,1,:) = ftemp(:,ny+1,:);
    ftemp(:,ny+2,:) = ftemp(:,2,:);
    ftemp(:,:,1) = ftemp(:,:,nx+1);
    ftemp(:,:,nx+2) = ftemp(:,:,2);

    for i=1:9
        fout(i,:,:) = ftemp(i,2-ey(i):ny+1-ey(i), 2-ex(i):nx+1-ex(i));
    end
end