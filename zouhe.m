function [f_out, ux_out, uy_out, rho_out] = zouhe(X,Y,f,rho,ux,uy)
    consts;

    uy_out = uy;
    ux_out = ux;
    rho_out = rho;

    uy_out(:,1) = 0;
    ux_out(:,1) = 4 * umax * (Y(:, 1) - 1.5) .* (ny - 2 - (Y(:, 1) - 1.5))/(ny-2)^2;
    rho_out(:,1) = (f(1,:,1) + f(3,:,1) + f(5,:,1) + 2*(f(4,:,1)+ f(7,:,1) + f(8,:,1)))'./(1-ux_out(:,1));
    % rho_out(:,1) = 1;
    
    % rho_out(:,nx) = 1;
    % uy_out(:,nx) = 0;
    % ux_out(:,nx) = -1 + (f(1,:,nx) + f(3,:,nx) + f(5,:,nx) + 2*(f(2,:,nx)+ f(6,:,nx) + f(9,:,nx)))'./rho_out(:,nx);

    f_out = f;

    f_out(2,:,1) = f(4,:,1)  + rho_out(:,1)' * 2/3 .* ux_out(:,1)';
    f_out(6,:,1) = f(8,:,1) + (f(5,:,1) - f(3,:,1))/2 + rho_out(:,1)' .* uy_out(:,1)' / 2 + rho_out(:,1)' .* ux_out(:,1)' / 6;
    f_out(9,:,1) = f(7,:,1) + (f(3,:,1) - f(5,:,1))/2 - rho_out(:,1)' .* uy_out(:,1)' / 2 + rho_out(:,1)' .* ux_out(:,1)' / 6;

    f_out(4,:,nx) = f(2,:,nx) - rho_out(:,nx)' * 2/3 .* ux_out(:,nx)';
    f_out(8,:,nx) = f(6,:,nx) + (f(3,:,nx) - f(5,:,nx))/2 - rho_out(:,nx)' .* uy_out(:,nx)' / 2 - rho_out(:,nx)' .* ux_out(:,nx)' / 6;
    f_out(7,:,nx) = f(9,:,nx) + (f(5,:,nx) - f(3,:,nx))/2 + rho_out(:,nx)' .* uy_out(:,nx)' / 2 - rho_out(:,nx)' .* ux_out(:,nx)' / 6;
end