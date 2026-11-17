#include "vonkarman.hpp"
#include "Eigen/Core"
#include <cmath>
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

    // source();

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
    for (Texture2D texture : textures) {
        UnloadTexture(texture);
    }
}

void VonKarman::init() {
    pixels = new Color[nx * ny];
    Image image = GenImageColor(nx, ny, BLACK);
    for (Texture2D &texture : textures) {
        texture = LoadTextureFromImage(image);
    }
    UnloadImage(image);
}
void VonKarman::start() {
    is_running = true;
    physics_thread = std::thread([this]() {
        while (is_running) {
            time_calc();
        }
    });
}

void VonKarman::load_image(const char *src, Rectangle rect) {
    Image img = LoadImage(src);
    ImageResizeNN(&img, rect.width, rect.height);

    Color c;
    for (int y = 0; y < rect.height; y++) {
        for (int x = 0; x < rect.width; x++) {
            c = GetImageColor(img, x, y);

            if (c.r + c.g + c.b < 100) {
                O(y+(int)rect.y, x+(int)rect.x) = 1;
            } else {
                O(y+(int)rect.y, x+(int)rect.x) = 0;
            }
        }
    }
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
    computation_time = computation_time_s / iteration;
    // if (iteration % 100 == 0) {
    //     std::cout << "Average computation time: " << computation_time << "
    //     µs"
    //               << std::endl;
    //     // computation_time_s = 0;
    // }
}

void VonKarman::source() {
    // double cx = 272 * factor;
    // double cy = 130 * factor;
    // double cx = 50;
    // double cy = 48;
    double cx = nx / 2 * factor;
    double cy = ny / 2 * factor;
    // double R = 4.0 * factor + 0.5;
    double R = 4 + 0.5;
    double rho_source = 0.2;
    double ux_source = 0.0;
    double uy_source = 0.0;

    auto dist2 = (X - cx).pow(2) + (Y - cy).pow(2);
    auto mask = (dist2 <= pow(R, 2));

    rho = mask.select(rho_source, rho);
    ux = mask.select(ux_source, ux);
    uy = mask.select(uy_source, uy);

    // Calcul de l'équilibre pour la source
    double u2 = ux_source * ux_source + uy_source * uy_source;

    for (int k = 0; k < 9; ++k) {
        double eu = ex[k] * ux_source + ey[k] * uy_source;

        // Valeur scalaire de f_eq pour la source (constante sur toute la zone)
        double f_source_val =
            w[k] * rho_source * (1.0 + 3.0 * eu + 4.5 * eu * eu - 1.5 * u2);

        // Remplacement exclusif dans la zone du masque
        f[k] = mask.select(f_source_val, f[k]);
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
#define LIM
#ifdef LIM
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
#endif

    // source();

    // Application du gradient nul sur les 4 bords pour absorber les particules
    // incidentes
    for (int k = 0; k < 9; ++k) {
        // Bord Gauche (x = 0)
        f[k].col(0) = f[k].col(1);

        // Bord Droit (x = nx-1)
        f[k].col(nx - 1) = f[k].col(nx - 2);

        // Bord Bas (y = 0)
        f[k].row(0) = f[k].row(1);

        // Bord Haut (y = ny-1)
        f[k].row(ny - 1) = f[k].row(ny - 2);
    }

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

Texture2D VonKarman::draw(DrawMode mode) {
    display_mat.setZero();

    double ex = 1;

    switch (mode) {
    case DENSITY:
        display_mat = rho;
        break;
    case VELOCITY:
        display_mat = ux.square() + uy.square();
        ex = 0.3;
        break;
    case VORTICITY: {
        std::lock_guard<std::mutex> lock(mtx);

        display_mat.block(1, 1, ny - 2, nx - 2) =
            0.5 * (uy.block(1, 2, ny - 2, nx - 2) -
                   uy.block(1, 0, ny - 2, nx - 2)) -
            0.5 * (ux.block(2, 1, ny - 2, nx - 2) -
                   ux.block(0, 1, ny - 2, nx - 2));
    }; break;
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
                pixels[index] = BLACK;
            } else {
                double u;

                    Color out;
                if (mode == VORTICITY) {
                    u = 1.0 / (1 + exp(-20.0 * (display_mat(y, x) - 0.5)));
                    // Color colorA = GetColor(0xff0000ff);
                    // Color colorB = GetColor(0xffffffff);
                    // Color colorC = GetColor(0x0000ffff);
                    Color colorA = GetColor(0xec8c5aff);
                    Color colorB = GetColor(0xcf56c6ff);
                    Color colorC = GetColor(0x375edaff);
                    if (u < 0.5)
                        out = ColorLerp(colorA, colorB, u * 2);
                    else
                        out = ColorLerp(colorB, colorC, (u - 0.5) * 2);
                } else {
                    Color colorA = GetColor(0x190F3Cff);
                    Color colorB = GetColor(0x3746FFff);
                    Color colorC = GetColor(0xFF808Bff);
                    u = pow(display_mat(y, x), ex);
                    if (u < 0.5)
                        out = ColorLerp(colorA, colorB, u * 2);
                    else
                        out = ColorLerp(colorB, colorC, (u - 0.5) * 2);
                }
                pixels[index] = out;
            }
        }
    }
    Texture2D &texture = textures[static_cast<int>(mode)];
    UpdateTexture(texture, pixels);
    return texture;
}