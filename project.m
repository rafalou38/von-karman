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


O = O + nx^2 * (X./nx - 0.5).^2 + ny^2 *(Y./ny - 0.5).^2 < 10^2;

f = init(X,Y);

% imagesc(O)


for i=1:1000
    [rho, ux, uy] = calcul(f);
    % [f, ux, uy, rho] = zouhe(X,Y,f,rho,ux,uy);
    f = collision(O, f,rho, ux, uy);
    f = propagation(f);

    imagesc(sqrt(ux.^2 + uy.^2) - 0.1 * O)
    axis equal
    colorbar
    pause(0.1);
end