#include <Eigen/Dense>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iostream>

#include <raylib.h>
#include <raymath.h>


class Recorder {
  private:
    FILE *ffmpeg;
    bool started = false;

  public:
    Recorder();
    ~Recorder();
    void start(int width, int height);
    void record_frame(Texture2D texture);
};