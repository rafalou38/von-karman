#include <iostream>

#include "Eigen/Core"
#include <Eigen/Dense>
#include <raylib.h>

#include "consts.hpp"

using Eigen::MatrixXd;
using Eigen::ArrayXXd;

enum DrawMode { VELOCITY, VORTICITY,DENSITY };

class VonKarman
{
private:
    
public:
    std::array<ArrayXXd, 9> f;
    std::array<ArrayXXd, 9> f_temp;

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

    void calcul();   

    void draw(DrawMode mode = VELOCITY);
};
