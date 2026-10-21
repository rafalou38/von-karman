function [rho, ux, uy] = calcul(f)
    consts;
    rho = squeeze(sum(f, 1));
    ux = squeeze(sum(f .* (ex'), 1)) ./ rho;
    uy = squeeze(sum(f .* (ey'), 1)) ./ rho;
end