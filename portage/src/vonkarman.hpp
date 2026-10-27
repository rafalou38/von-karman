#include <iostream>

#include "Eigen/Core"
#include <Eigen/Dense>
#include <raylib.h>
#include <atomic>
#include <thread>
#include <mutex>

#include "consts.hpp"

using Eigen::MatrixXd;
using Eigen::ArrayXXd;

enum DrawMode { VELOCITY, VORTICITY,DENSITY };

class VonKarman
{
private:
    std::atomic<bool> is_running;
    std::thread physics_thread;
    std::mutex mtx;
public:
    unsigned int iteration = 0;
    unsigned int computation_time = 0;
    unsigned int computation_time_s = 0;

    std::array<ArrayXXd, 9> f;
    std::array<ArrayXXd, 9> f_temp;

    ArrayXXd vorticity;

    ArrayXXd O;

    ArrayXXd X;
    ArrayXXd Y;

    ArrayXXd rho;
    ArrayXXd ux;
    ArrayXXd uy;


    Color* pixels;
    Texture2D texture;
    Eigen::ArrayXXd display_mat;


    VonKarman();
    ~VonKarman();

    void init();

    void start();
    void stop();

    void calcul();   
    void time_calc();

    void draw(DrawMode mode = VELOCITY);
};
