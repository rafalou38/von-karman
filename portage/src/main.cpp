// SPDX-FileCopyrightText: The Eigen Authors
// SPDX-License-Identifier: MPL-2.0

#include <Eigen/Dense>
#include <iostream>

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

  int sfps = 0;
  int nframes = 0;

  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      width = GetRenderWidth();
      height = GetRenderHeight();
    }

    vk.calcul();

    BeginDrawing();
    // ClearBackground({22, 22, 22, 255});
    vk.draw();

    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}