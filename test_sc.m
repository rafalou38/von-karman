clear;
clc;
close all;

nx = 250;
ny = 200;
f= zeros(9, ny,nx);
f(:,3,6) = 1;

t = 0
while t<1000
    f=propagation(f);
    imagesc(squeeze(f(8,:,:)));
    axis equal
    pause(0.025);

    t = t+1;
end