#define factor 1
// #define nx (2846/2)
// #define ny (2846/2)
// #define nx (512*factor)
// #define ny (512*factor)
#define nx 800
#define ny 200

#define umax 0.03
#define nu 0.02
#define tau ((6*nu+1)/2)

#define OBSTACLE Color{60, 50, 168, 255}
#define NEUTRAL Color{168, 50, 60, 255}

#define ITER_PER_FRAME 10

#define PART_DENSITY 0.15
#define PART_CNT (PART_DENSITY * nx * ny)
#define PART_HIST_SZ 50
#define PART_REM 5

#define SCALING 3.0

const int ex[] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
const int ey[] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
const double w[] = { 
    4.0 / 9.0,
    1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
};
