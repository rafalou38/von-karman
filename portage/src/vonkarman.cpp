#include "vonkarman.hpp"
#include "Eigen/Core"
#include <raylib.h>

VonKarman::VonKarman() {
    f.fill(ArrayXXd::Zero(ny, nx));
    f_temp.fill(ArrayXXd::Zero(ny, nx));

    X = ArrayXXd::Zero(ny, nx);
    Y = ArrayXXd::Zero(ny, nx);
    for (int x = 0; x < nx; x++) {
        for (int y = 0; y < ny; y++) {
            X(y, x) = x;
            Y(y, x) = y;
        }
    }   

    O = ArrayXXd::Zero(ny, nx);
    O.row(0).setConstant(1.0);
    O.row(ny - 1).setConstant(1.0);

    rho = ArrayXXd::Ones(ny, nx);
    uy = ArrayXXd::Zero(ny, nx);
    // ux = ArrayXXd::Zero(ny, nx);
    double H = ny - 2.0;
    ux = 4 * umax * (Y - 0.5) * (H - (Y - 0.5)) / (H * H);
    ux.row(0).setZero();
    ux.row(ny - 1).setZero();

    auto uu = ux.square() + uy.square();
    for (int i = 0; i < 9; i++) {
        auto eiu = ex[i] * ux + ey[i] * uy;
        f[i] = w[i] * rho * (1 + 3 * eiu + 4.5 * eiu.square() - 1.5 * uu);
    }
    //   std::cout << "uy: " << uy.rows() << "x" << uy.cols() << std::endl;
    std::cout << "Initialization complete." << std::endl;
}

VonKarman::~VonKarman() {
    // Destructor implementation (if needed)
}

// f -> rho -> ux, uy
void VonKarman::calcul() {
    // Propagation
    f_temp[0] = f[0];
    f_temp[1].block(0, 1, ny, nx - 1) = f[1].block(0, 0, ny, nx - 1);
    f_temp[2].block(1, 0, ny - 1, nx) = f[2].block(0, 0, ny - 1, nx);
    f_temp[3].block(0, 0, ny, nx - 1) = f[3].block(0, 1, ny, nx - 1);
    f_temp[4].block(0, 0, ny - 1, nx) = f[4].block(1, 0, ny - 1, nx);
    f_temp[5].block(1, 1, ny - 1, nx - 1) = f[5].block(0, 0, ny - 1, nx - 1);
    f_temp[6].block(1, 0, ny - 1, nx - 1) = f[6].block(0, 1, ny - 1, nx - 1);
    f_temp[7].block(0, 0, ny - 1, nx - 1) = f[7].block(1, 1, ny - 1, nx - 1);
    f_temp[8].block(0, 1, ny - 1, nx - 1) = f[8].block(1, 0, ny - 1, nx - 1);
    
    // f = f_temp;
    for (int i = 0; i < 9; i++) {
        f[i].swap(f_temp[i]);
    }

    // Zou-He
    // -> Entrée 
    uy.col(0).setZero();
    double H = ny - 2.0;
    ux.col(0) = 4.0 * umax * (Y.col(0) - 0.5) * (H - (Y.col(0) - 0.5)) / (H * H);
    ux(0, 0) = 0.0;
    ux(ny - 1, 0) = 0.0;
    
    rho.col(0) = (f[0].col(0) + f[2].col(0) + f[4].col(0) + 
                  2.0 * (f[3].col(0) + f[6].col(0) + f[7].col(0))) / (1.0 - ux.col(0));

    f[1].col(0) = f[3].col(0) + rho.col(0) * (2.0 / 3.0) * ux.col(0);
    f[5].col(0) = f[7].col(0) + (f[4].col(0) - f[2].col(0)) / 2.0 + rho.col(0) * uy.col(0) / 2.0 + rho.col(0) * ux.col(0) / 6.0;
    f[8].col(0) = f[6].col(0) + (f[2].col(0) - f[4].col(0)) / 2.0 - rho.col(0) * uy.col(0) / 2.0 + rho.col(0) * ux.col(0) / 6.0;

    // -> Sortie
    rho.col(nx - 1).setConstant(1.0);
    uy.col(nx - 1).setZero();
    ux.col(nx - 1) = -1.0 + (f[0].col(nx - 1) + f[2].col(nx - 1) + f[4].col(nx - 1) +
                     2.0 * (f[1].col(nx - 1) + f[5].col(nx - 1) + f[8].col(nx - 1))) / rho.col(nx - 1);
    

    f[3].col(nx - 1) = f[1].col(nx - 1) - rho.col(nx - 1) * (2.0 / 3.0) * ux.col(nx - 1);
    f[7].col(nx - 1) = f[5].col(nx - 1) + (f[2].col(nx - 1) - f[4].col(nx - 1)) / 2.0 - rho.col(nx - 1) * uy.col(nx - 1) / 2.0 - rho.col(nx - 1) * ux.col(nx - 1) / 6.0;
    f[6].col(nx - 1) = f[8].col(nx - 1) + (f[4].col(nx - 1) - f[2].col(nx - 1)) / 2.0 + rho.col(nx - 1) * uy.col(nx - 1) / 2.0 - rho.col(nx - 1) * ux.col(nx - 1) / 6.0;

    // Collision avec les obstacles
    auto mask = (O == 1.0);
    ArrayXXd temp;
    temp = f[1];

    f[1] = mask.select(f[3], f[1]);
    f[3] = mask.select(temp, f[3]);

    temp = f[2];
    f[2] = mask.select(f[4], f[2]);
    f[4] = mask.select(temp, f[4]);

    temp = f[6];
    f[6] = mask.select(f[8], f[6]);
    f[8] = mask.select(temp, f[8]);

    temp = f[7];
    f[7] = mask.select(f[5], f[7]);
    f[5] = mask.select(temp, f[5]);

    // Calcul de rho, ux, uy
    rho = f[0] + f[1] + f[2] + f[3] + f[4] + f[5] + f[6] + f[7] + f[8];
    ux = ((f[1] + f[5] + f[8]) - (f[3] + f[6] + f[7])) / rho;
    uy = ((f[2] + f[5] + f[6]) - (f[4] + f[7] + f[8])) / rho;

    ux = mask.select(0.0, ux);
    uy = mask.select(0.0, uy);

    // Collision entre les particules
    ArrayXXd uu = ux.square() + uy.square();
    double omega = 1.0 / tau;
    for (int i = 0; i < 9; i++) {
        ArrayXXd eiu = ex[i] * ux + ey[i] * uy;
        f[i] = f[i] - omega * (f[i] - w[i] * rho * (1.0 + 3.0 * eiu + 4.5 * eiu.square() - 1.5 * uu));
    }
}
double m = 0.0;
void VonKarman::draw() {
    // double m = ux.maxCoeff();
    for (int x = 0; x < nx; x++) {
        for (int y = 0; y < ny; y++) {
            double u = ux(y, x)*ux(y, x) + uy(y, x)*uy(y, x);
            if (u > m) {
                m = u;
            }
            unsigned char v = 255 * (u / m);
            DrawRectangle(x * 2, y * 2, 2, 2, Color{v, v, v, 255});
        }
    }
}