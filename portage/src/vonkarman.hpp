#include <iostream>

#include "Eigen/Core"
#include <Eigen/Dense>
#include <raylib.h>

#include "consts.hpp"

using Eigen::MatrixXd;
using Eigen::ArrayXXd;

class VonKarman
{
private:
    
public:
    std::array<ArrayXXd, 9> f;
    std::array<ArrayXXd, 9> f_temp;

    ArrayXXd X;
    ArrayXXd Y;

    ArrayXXd rho;
    ArrayXXd ux;
    ArrayXXd uy;


    VonKarman();
    ~VonKarman();

    void calcul();   

    void draw();
};
