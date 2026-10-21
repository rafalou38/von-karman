clear;
clc;
close all;

consts;

x = 1:nx;
y = 1:ny;

[X, Y] = meshgrid(x, y);


O = zeros(ny, nx);
O(1,:) = 1;
O(ny,:) = 1;

f = init(X,Y);

% imagesc(O)


for i=1:50
    [rho, ux, uy] = calcul(f);
    f = collision(O, f,rho, ux, uy);
    f = propagation(f);


    imagesc(squeeze(f(2,:,:)))
    % imagesc(squeeze(ux))
    pause(0.2);
end