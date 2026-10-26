function f = init(X,Y)
    consts;

    rho = ones(ny, nx);
    uy = zeros(ny,nx);
    ux = 4 * umax*(Y - 1.5).*(ny - 2 - (Y - 1.5)) / (ny - 2)^2;
    % ux = zeros(ny,nx);

    f = fieq(rho, ux, uy);
end