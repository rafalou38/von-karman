function f_out = collision(O, f, rho, ux, uy)
    consts;

    f_out = f - (f-fieq(rho, ux, uy))/tau;

    % f_out(5,ny-1,:) = f_out(5,ny-1,:) + f_out(3,ny-1,:);
    % f_out(3,ny-1,:) = 0;

    tmp = f_out(3,ny-1,:);
    f_out(3,ny-1,:) = f_out(5,ny-1,:);
    f_out(5,ny-1,:) = tmp;

    tmp = f_out(3,2,:);
    f_out(3,2,:) = f_out(5,2,:);
    f_out(5,2,:) = tmp;
end