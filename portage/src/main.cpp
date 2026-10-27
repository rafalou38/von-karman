// SPDX-FileCopyrightText: The Eigen Authors
// SPDX-License-Identifier: MPL-2.0

#include <Eigen/Dense>
#include <chrono>
#include <cstdint>
#include <cstdio>
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

    vk.init();
#ifdef PARALLEL_COMPUTE
    SetTargetFPS(60);
    vk.start();
#endif


    DrawMode mode = VELOCITY;

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            width = GetRenderWidth();
            height = GetRenderHeight();
        }

        if (IsKeyPressed(KEY_TAB)) {
            mode = static_cast<DrawMode>((mode + 1) % 2);
        }

        BeginDrawing();
        vk.draw(mode);

#ifndef PARALLEL_COMPUTE
  vk.time_calc();
#endif

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
        DrawText(mode_str, 10, 10, 20, NEUTRAL);

        #ifdef PARALLEL_COMPUTE
        sprintf(mode_str, "SIM: %.0f", 1e6 / vk.computation_time);
        DrawText(mode_str, 10, 30, 20, NEUTRAL);
        sprintf(mode_str, "FPS: %d", GetFPS());
        DrawText(mode_str, 10, 50, 20, NEUTRAL);
        sprintf(mode_str, "FRAME: %d", vk.iteration);
        DrawText(mode_str, 10, 70, 20, NEUTRAL);
        #else
        sprintf(mode_str, "FPS: %d", GetFPS());
        DrawText(mode_str, 10, 30, 20, NEUTRAL);
        sprintf(mode_str, "FRAME: %d", vk.iteration);
        DrawText(mode_str, 10, 50, 20, NEUTRAL);
        #endif
        // DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}