// SPDX-FileCopyrightText: The Eigen Authors
// SPDX-License-Identifier: MPL-2.0

#include <Eigen/Dense>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <chrono>

#include <raylib.h>
#include <raymath.h>

#include "vonkarman.hpp"

using Eigen::MatrixXd;

VonKarman vk;

int main() {
  int width = nx * 2;
  int height = ny * 2;

  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_MSAA_4X_HINT);

  InitWindow(width, height, "Simulation de Von_Karman");
  // SetWindowState(FLAG_WINDOW_RESIZABLE);
  // SetWindowState(FLAG_WINDOW_MAXIMIZED);
  // SetTargetFPS(60);

  vk.init();

  long int m1 = 0;
  long int m2 = 0;
  int cnt=0;

  DrawMode mode = VELOCITY;

  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      width = GetRenderWidth();
      height = GetRenderHeight();
    }

    if(IsKeyPressed(KEY_TAB)) {
      mode = static_cast<DrawMode>((mode + 1) % 2);
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    vk.calcul();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cnt++;
    m1 += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    BeginDrawing();
    // ClearBackground({22, 22, 22, 255});
    begin = std::chrono::steady_clock::now();
    vk.draw(mode);
    end = std::chrono::steady_clock::now();
    m2 += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    if(cnt % 100 == 0) {
      std::cout << "Iteration = " << cnt << std::endl;
      std::cout << "Calc time = " << m1/cnt << "[µs]" << std::endl;
      std::cout << "Draw time = " << m2/cnt << "[µs]" << std::endl << std::endl;
    }

    char mode_str[100];
    switch (mode) {
      case DENSITY:
        sprintf(mode_str, "DENSITY");
        break;
      case VELOCITY:
        sprintf(mode_str, "VELOCITY");
        break;
      case VORTICITY:
        sprintf(mode_str, "VORTICITY");
        break;
    }
    DrawText(mode_str, 10, 30, 20, NEUTRAL);
    sprintf(mode_str, "FPS: %d", GetFPS());
    DrawText(mode_str, 10, 10, 20, NEUTRAL);
    // DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}