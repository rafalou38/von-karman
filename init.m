function f = init(X,Y)
    consts;

    f = ones(9, ny, nx);
    % f(2, :, 5:30) = 1;

    [rho, ux, uy] = calcul(f);

    
    ux = 4 * umax*(Y - 1.5).*(ny - 2 - (Y - 1.5)) / (ny - 2)^2;

    f = fieq(rho, ux, uy);
end