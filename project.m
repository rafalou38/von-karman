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


for i=1:1000
    [rho, ux, uy] = calcul(f);
    
    f = collision(O, f,rho, ux, uy);
    f = propagation(f);

    [rho, ux, uy] = calcul(f);

    [f, ux, uy, rho] = zouhe(X,Y,f,rho,ux,uy);


    imagesc(sqrt(ux.^2 + uy.^2))
    pause(0.001);
end