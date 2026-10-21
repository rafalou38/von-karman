function f_out = fieq(rho, ux, uy)
    consts;

    w = [4/9 1/9 * ones(1, 4) 1/36 * ones(1, 4)];

    uu = ux .* ux + uy .* uy;
    
    for i =1:9
        eiu = ex(i)*ux + ey(i)*uy;
        f_out(i,:,:) = w(i) * rho .* (1 + 3*eiu + 9/2 * eiu.^2 - 3/2*uu);
    end
end