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


O = O + nx^2 * (X./nx - 0.5).^2 + ny^2 *(Y./ny - 0.5).^2 < 40^2;
% O(1:(0.2*ny), 100:101) = 1;
% O((0.4*ny):(0.6*ny), 100:101) = 1;
% O((0.8*ny):ny, 100:101) = 1;

% O((0.1*ny):(0.9*ny), 150:151) = 1;


f = init(X,Y);

% imagesc(O)

[startX,startY] = meshgrid(5, 10:4:(ny-10));
% [startX,startY] = meshgrid( 5:3:(nx-5), 10:3:(ny-10));

fig = figure(Visible="on", Position=[100,100, nx, ny])
ax = axes(Parent=fig, Position=[0 0 1 1])

vid = VideoWriter('out-new3.avi', 'Uncompressed AVI');
open(vid)
it_cnt = 20000;
for i=1:it_cnt
    [rho, ux, uy] = calcul(f);
    
    if any(~isfinite(f(:))) || any(rho(:) <= 0, "all")
        error("Divergence à i=%d : rho=[%g,%g]", ...
            i, min(rho(:)), max(rho(:)));
    end

    [f, ux, uy, rho] = zouhe(X,Y,f,rho,ux,uy);
    f = collision(O, f,rho, ux, uy);
    f = propagation(f);


    if mod(i, 1) == 0
        fprintf("%.2f\t%d/%d\n", i/it_cnt * 100, i, it_cnt)

        cla(ax);

        title(string(i))
        hold on;
        
        v = ux.^2 + uy.^2;
        % [curlz, cav] = curl(X, Y, ux,uy);
        
        
        imagesc(ax, v);
        % imagesc(curlz);

        imagesc(ax, cat(3, 0 * O, 0 * O, 0 * O), AlphaData=O);
        

        clim(ax, [min(v(:)); max(v(:))]);
        axis(ax, "equal");
        axis(ax, "off");


        % h = streamline(ux,    uy, startX, startY,[0.1, 10000] , LineWidth=1, Color="w");
        % % h = streamline(ux, uy, startX, startY);
        % for k = 1:length(h)
        %     h(k).Color(4) = 0.3;
        % end
        

        % Transparence alpha (si supportée par ta version)
      

        % [C, h] = contour(X, Y, curlz, 2, LineColor="w", LineWidth=1.5);

        % subplot(2,1,1);
        % imagesc(ux);
        % subplot(2,1,2);
        % imagesc(uy);

        % w = [zeros(1, nx); diff(uy, 1, 1)] - [diff(ux, 1, 2) zeros(ny, 1)];
        
        frame = getframe(fig);
        frame.cdata = imresize(frame.cdata, [ny,nx]);
        writeVideo(vid,frame);
    end
end
close(vid)
close(fig)