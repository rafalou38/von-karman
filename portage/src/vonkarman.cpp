#include "vonkarman.hpp"
#include "Eigen/Core"
#include <raylib.h>

VonKarman::VonKarman() : is_running(false) {
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

    display_mat = ArrayXXd::Zero(ny, nx);
    O = ArrayXXd::Zero(ny, nx);
    O.row(0).setConstant(1.0);
    O.row(ny - 1).setConstant(1.0);

    rho = ArrayXXd::Ones(ny, nx);
    uy = ArrayXXd::Zero(ny, nx);
    double H = ny - 2.0;
    ux = 4 * umax * (Y - 0.5) * (H - (Y - 0.5)) / (H * H);
    ux.row(0).setZero();
    ux.row(ny - 1).setZero();

    auto uu = ux.square() + uy.square();
    for (int i = 0; i < 9; i++) {
        auto eiu = ex[i] * ux + ey[i] * uy;
        f[i] = w[i] * rho * (1 + 3 * eiu + 4.5 * eiu.square() - 1.5 * uu);
    }
    std::cout << "Initialization complete." << std::endl;
}

VonKarman::~VonKarman() {
    stop();
    delete[] pixels;
    UnloadTexture(texture);
}

void VonKarman::init() {
    pixels = new Color[nx * ny];
    texture = LoadTextureFromImage(GenImageColor(nx, ny, BLACK));

    double r = 0.1 * ny;
    auto cylindre = ((X - 0.2 * nx).square() + (Y - 0.5 * ny - 1).square() <=
                     (r * r) + 1.0);
    O = cylindre.select(1.0, O);
}
void VonKarman::start() {
    is_running = true;
    physics_thread = std::thread([this]() {
        while (is_running) {
            time_calc();
        }
    });
}

void VonKarman::stop() {
    if (!is_running)
        return;

    is_running = false;
    if (physics_thread.joinable()) {
        physics_thread.join();
    }
}

void VonKarman::time_calc() {
    auto begin = std::chrono::steady_clock::now();
    calcul();
    auto end = std::chrono::steady_clock::now();

    computation_time_s +=
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
            .count();
    iteration++;
    if (iteration % 100 == 0) {
        computation_time = computation_time_s / 100;
        std::cout << "Average computation time: " << computation_time << " µs"
                  << std::endl;
        computation_time_s = 0;
    }
}

void VonKarman::calcul() {
    // ###############
    // # PROPAGATION #
    // ###############
    f_temp[0] = f[0];
    f_temp[1].block(0, 1, ny, nx - 1) = f[1].block(0, 0, ny, nx - 1);
    f_temp[2].block(1, 0, ny - 1, nx) = f[2].block(0, 0, ny - 1, nx);
    f_temp[3].block(0, 0, ny, nx - 1) = f[3].block(0, 1, ny, nx - 1);
    f_temp[4].block(0, 0, ny - 1, nx) = f[4].block(1, 0, ny - 1, nx);
    f_temp[5].block(1, 1, ny - 1, nx - 1) = f[5].block(0, 0, ny - 1, nx - 1);
    f_temp[6].block(1, 0, ny - 1, nx - 1) = f[6].block(0, 1, ny - 1, nx - 1);
    f_temp[7].block(0, 0, ny - 1, nx - 1) = f[7].block(1, 1, ny - 1, nx - 1);
    f_temp[8].block(0, 1, ny - 1, nx - 1) = f[8].block(1, 0, ny - 1, nx - 1);

    for (int i = 0; i < 9; i++) {
        f[i].swap(f_temp[i]);
    }

    // ##########################
    // # Conditions aux limites #
    // #        Zou He          #
    // ##########################
    // -> Entrée
    uy.col(0).setZero();
    double H = ny - 2.0;
    ux.col(0) =
        4.0 * umax * (Y.col(0) - 0.5) * (H - (Y.col(0) - 0.5)) / (H * H);
    ux(0, 0) = 0.0;
    ux(ny - 1, 0) = 0.0;

    rho.col(0) = (f[0].col(0) + f[2].col(0) + f[4].col(0) +
                  2.0 * (f[3].col(0) + f[6].col(0) + f[7].col(0))) /
                 (1.0 - ux.col(0));

    f[1].col(0) = f[3].col(0) + rho.col(0) * (2.0 / 3.0) * ux.col(0);
    f[5].col(0) = f[7].col(0) + (f[4].col(0) - f[2].col(0)) / 2.0 +
                  rho.col(0) * uy.col(0) / 2.0 + rho.col(0) * ux.col(0) / 6.0;
    f[8].col(0) = f[6].col(0) + (f[2].col(0) - f[4].col(0)) / 2.0 -
                  rho.col(0) * uy.col(0) / 2.0 + rho.col(0) * ux.col(0) / 6.0;

    // -> Sortie
    rho.col(nx - 1).setConstant(1.0);
    uy.col(nx - 1).setZero();
    ux.col(nx - 1) =
        -1.0 +
        (f[0].col(nx - 1) + f[2].col(nx - 1) + f[4].col(nx - 1) +
         2.0 * (f[1].col(nx - 1) + f[5].col(nx - 1) + f[8].col(nx - 1))) /
            rho.col(nx - 1);

    f[3].col(nx - 1) =
        f[1].col(nx - 1) - rho.col(nx - 1) * (2.0 / 3.0) * ux.col(nx - 1);
    f[7].col(nx - 1) = f[5].col(nx - 1) +
                       (f[2].col(nx - 1) - f[4].col(nx - 1)) / 2.0 -
                       rho.col(nx - 1) * uy.col(nx - 1) / 2.0 -
                       rho.col(nx - 1) * ux.col(nx - 1) / 6.0;
    f[6].col(nx - 1) = f[8].col(nx - 1) +
                       (f[4].col(nx - 1) - f[2].col(nx - 1)) / 2.0 +
                       rho.col(nx - 1) * uy.col(nx - 1) / 2.0 -
                       rho.col(nx - 1) * ux.col(nx - 1) / 6.0;

    // -> Paroi
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

    // ################
    // # Calcul macro #
    // ################
    rho = f[0] + f[1] + f[2] + f[3] + f[4] + f[5] + f[6] + f[7] + f[8];
    {
        std::lock_guard<std::mutex> lock(mtx);

        ux = ((f[1] + f[5] + f[8]) - (f[3] + f[6] + f[7])) / rho;
        uy = ((f[2] + f[5] + f[6]) - (f[4] + f[7] + f[8])) / rho;

        ux = mask.select(0.0, ux);
        uy = mask.select(0.0, uy);
    }

    // #####################
    // # Collision fluides #
    // #####################
    ArrayXXd uu = ux.square() + uy.square();
    double omega = 1.0 / tau;

#pragma omp parallel for
    for (int i = 0; i < 9; i++) {
        ArrayXXd eiu = ex[i] * ux + ey[i] * uy;
        ArrayXXd f_eq =
            w[i] * rho * (1.0 + 3.0 * eiu + 4.5 * eiu.square() - 1.5 * uu);
        f[i] = mask.select(f[i], f[i] - omega * (f[i] - f_eq));
    }
}

void VonKarman::draw(DrawMode mode) {
    display_mat.setZero();
    // Vorticité

    switch (mode) {
    case DENSITY:
        display_mat = rho;
        break;
    case VELOCITY:
        display_mat = ux.square() + uy.square();
        break;
    case VORTICITY: 
        {
            std::lock_guard<std::mutex> lock(mtx);

            display_mat.block(1, 1, ny - 2, nx - 2) =
                0.5 * (uy.block(1, 2, ny - 2, nx - 2) -
                    uy.block(1, 0, ny - 2, nx - 2)) -
                0.5 * (ux.block(2, 1, ny - 2, nx - 2) -
                    ux.block(0, 1, ny - 2, nx - 2));
        };
        break;
    }

    double m = display_mat.maxCoeff();
    double mi = display_mat.minCoeff();

    if (mode == DENSITY) {
        mi = 0.9;
        m = 1.1;
    } else if (mode == VELOCITY) {
        mi = 0.0;
        m = std::max(1e-6, m);
    } else if (mode == VORTICITY) {
        m = std::max({1e-6, std::abs(m), std::abs(mi)});
        mi = -m;
    }

    display_mat = (display_mat - mi) / (m - mi);

    for (int x = 0; x < nx; x++) {
        for (int y = 0; y < ny; y++) {
            int index = y * nx + x;

            if (O(y, x) == 1.0) {
                pixels[index] = OBSTACLE;
            } else {

                double u = display_mat(y, x);
                unsigned char v = 255 * u;
                pixels[index] = Color{v, v, v, 255};
            }
        }
    }
    UpdateTexture(texture, pixels);
    DrawTextureEx(texture, Vector2{0.0f, 0.0f}, 0.0f, 2.0f, WHITE);
}