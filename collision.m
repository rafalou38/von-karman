function f_out = collision(O, f, rho, ux, uy)
    consts;

    f_out = f - (f-fieq(rho, ux, uy))/tau;


    swaps = [ 2 4 ; 3 5 ; 7 9 ; 8 6];

    for k = 1:4
        i = swaps(k,1);
        j = swaps(k,2);

        temp = f(i,:,:);
        f_out(i,:,:) = O .* squeeze(f_out(j,:,:)) + (1 - O) .* squeeze(f_out(i,:,:));
        f_out(j,:,:) = O .* squeeze(temp) + (1 - O) .* squeeze(f_out(j,:,:));
    end
end