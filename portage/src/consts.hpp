#define factor 1
#define nx (2846/2)
#define ny (2846/2)
// #define nx (512*factor)
// #define ny (512*factor)

#define umax 0.1
#define nu 0.05
#define tau ((6*nu+1)/2)

#define OBSTACLE Color{60, 50, 168, 255}
#define NEUTRAL Color{168, 50, 60, 255}

// #define PARALLEL_COMPUTE
#define ITER_PER_FRAME 5
// #define RECORD

#define SCALING 1.0

const int ex[] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
const int ey[] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
const double w[] = { 
    4.0 / 9.0,
    1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
};
