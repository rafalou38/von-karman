// SPDX-FileCopyrightText: The Eigen Authors
// SPDX-License-Identifier: MPL-2.0

#include <Eigen/Dense>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "vonkarman.hpp"

using Eigen::MatrixXd;

VonKarman vk;

int main() {
    int width = nx * SCALING;
    int height = ny * SCALING;

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

    char cmd[200];
    time_t now = time(0);

    sprintf(cmd,
            "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgba -s %dx%d "
            "-r 60 -i - -c:v libx264 -preset ultrafast -crf 18 output_%ld.mp4",
            width, height, now);
    FILE *ffmpeg = popen(cmd, "w");
    RenderTexture2D target = LoadRenderTexture(width, height);

    DrawMode mode = VORTICITY;

    int prev_frame_draw = 0;

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            width = GetRenderWidth();
            height = GetRenderHeight();
        }

        if (IsKeyPressed(KEY_TAB)) {
            mode = static_cast<DrawMode>((mode + 1) % 2);
        }

#ifndef PARALLEL_COMPUTE
        vk.time_calc();
#endif
        // BeginDrawing();
        BeginTextureMode(target);
        vk.draw(mode);

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
        EndTextureMode();

        if (vk.iteration - prev_frame_draw >= ITER_PER_FRAME) {
            prev_frame_draw = vk.iteration;
            // 3. Enregistrer l'image dans un fichier
            Image screen = LoadImageFromTexture(target.texture);
            ImageFlipVertical(&screen); // Indispensable : OpenGL stocke les
                                        // textures à l'envers
            fwrite(screen.data, 1, screen.width * screen.height * 4, ffmpeg);
            UnloadImage(screen);
        }

        // 4. Afficher la texture sur la vraie fenêtre
        BeginDrawing();
        ClearBackground(BLACK);
        // On inverse la hauteur (hauteur négative) pour remettre l'image à
        // l'endroit à l'écran
        DrawTextureRec(target.texture,
                       Rectangle{0, 0, (float)target.texture.width,
                                 (float)-target.texture.height},
                       Vector2{0, 0}, WHITE);

        DrawText(mode_str, 10, 5 + 10 / 2, 20 / 2, NEUTRAL);

#ifdef PARALLEL_COMPUTE
        sprintf(mode_str, "SIM: %.0f Hz", 1e6 / vk.computation_time);
        DrawText(mode_str, 10, 5 + 30 / 2, 20 / 2, NEUTRAL);
        sprintf(mode_str, "FPS: %d Hz", GetFPS());
        DrawText(mode_str, 10, 5 + 50 / 2, 20 / 2, NEUTRAL);
        sprintf(mode_str, "FRAME: %d", vk.iteration);
        DrawText(mode_str, 10, 5 + 70 / 2, 20 / 2, NEUTRAL);
#else
        sprintf(mode_str, "FPS: %d Hz", GetFPS());
        DrawText(mode_str, 10, 5 + 30 / 2, 10, NEUTRAL);
        sprintf(mode_str, "FRAME: %d Hz", vk.iteration);
        DrawText(mode_str, 10, 5 + 50 / 2, 10, NEUTRAL);
#endif
        EndDrawing();
    }

    CloseWindow();

    pclose(ffmpeg);
    return 0;
}