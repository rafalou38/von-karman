function f_out = collision(O, f, rho, ux, uy)
    consts;

    f_out = f - (f-fieq(rho, ux, uy))/tau;

    % f_out(5,ny-1,:) = f_out(5,ny-1,:) + f_out(3,ny-1,:);
    % f_out(3,ny-1,:) = 0;


    % haut
    tmp3 = f_out(3,ny-1,:); tmp6 = f_out(6,ny-1,:); tmp7 = f_out(7,ny-1,:);
    f_out(3,ny-1,:) = f_out(5,ny-1,:); f_out(5,ny-1,:) = tmp3;
    f_out(6,ny-1,:) = f_out(8,ny-1,:); f_out(8,ny-1,:) = tmp6;
    f_out(7,ny-1,:) = f_out(9,ny-1,:); f_out(9,ny-1,:) = tmp7;

    % bas
    tmp3 = f_out(3,2,:); tmp6 = f_out(6,2,:); tmp7 = f_out(7,2,:);
    f_out(3,2,:) = f_out(5,2,:); f_out(5,2,:) = tmp3;
    f_out(6,2,:) = f_out(8,2,:); f_out(8,2,:) = tmp6;
    f_out(7,2,:) = f_out(9,2,:); f_out(9,2,:) = tmp7;
end