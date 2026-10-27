#define nx 800
#define ny 200

#define umax 0.1
#define nu 0.02
#define tau ((6*nu+1)/2)

#define OBSTACLE Color{60, 50, 168, 255}
#define NEUTRAL Color{168, 50, 60, 255}

const int ex[] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
const int ey[] = { 0, 0, 1, 0, -1, 1, 1, -1, -1 };
const double w[] = { 
    4.0 / 9.0,
    1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
};
