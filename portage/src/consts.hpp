#define factor 1
// #define nx (2846/2)
// #define ny (2846/2)
// #define nx (512*factor)
// #define ny (512*factor)
#define nx 400
#define ny 100

#define umax 0.1
#define nu 0.02
#define tau ((6*nu+1)/2)

#define OBSTACLE Color{60, 50, 168, 255}
#define NEUTRAL Color{168, 50, 60, 255}

// #define PARALLEL_COMPUTE
#define ITER_PER_FRAME 15
// #define RECORD

#define PART_CNT 10000
#define PART_HIST_SZ 30
#define PART_REM 5

#define SCALING 1.0

const int ex[] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
const int ey[] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
const double w[] = { 
    4.0 / 9.0,
    1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
};
