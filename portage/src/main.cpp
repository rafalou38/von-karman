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
bool flag_record = false;

FILE *ffmpeg;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  --help, -h    Show this help message\n"
                      << "  --record, -r  Record the simulation\n";
            return 0;
        } else if (std::string(argv[1]) == "--record" ||
                   std::string(argv[1]) == "-r") {
            std::cout << "Recording enabled.\n";
            flag_record = true;
        }
    }

    int width = nx * SCALING;
    int height = ny * SCALING * 2;

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(width, height, "Simulation de Von_Karman");

    vk.init();
    if (flag_record) {
        char cmd[200];
        time_t now = time(0);
        sprintf(
            cmd,
            "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgba -s %dx%d "
            "-r 60 -i - -c:v libx264 -preset ultrafast -crf 18 output_%ld.mp4",
            width, height, now);
        ffmpeg = popen(cmd, "w");
    } else {
        SetTargetFPS(60);
        vk.start();
    }
    RenderTexture2D target = LoadRenderTexture(width, height / 2);
    RenderTexture2D target_final = LoadRenderTexture(width, height);

    int prev_frame_draw = 0;
    while (!WindowShouldClose()) {
        if (flag_record) {
            vk.time_calc();
        }

        BeginTextureMode(target);
        vk.draw(VORTICITY);
        EndTextureMode();
        BeginTextureMode(target_final);
        vk.draw(VELOCITY);
        DrawTextureRec(target.texture,
                       Rectangle{0, 0, (float)target.texture.width,
                                 (float)-target.texture.height},
                       Vector2{0, ny}, WHITE);
        EndTextureMode();

        if (flag_record && vk.iteration - prev_frame_draw >= ITER_PER_FRAME) {
            prev_frame_draw = vk.iteration;
            // 3. Enregistrer l'image dans un fichier
            Image screen = LoadImageFromTexture(target_final.texture);
            ImageFlipVertical(&screen); // Indispensable : OpenGL stocke les
                                        // textures à l'envers
            fwrite(screen.data, 1, screen.width * screen.height * 4, ffmpeg);
            UnloadImage(screen);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureRec(target_final.texture,
                       Rectangle{0, 0, (float)target_final.texture.width,
                                 (float)-target_final.texture.height},
                       Vector2{0, 0}, WHITE);

        char mode_str[100];

        if (flag_record) {
            sprintf(mode_str, "RECORDING");
            DrawText(mode_str, 5, 5, 10, NEUTRAL);
            sprintf(mode_str, "FPS: %d Hz", GetFPS());
            DrawText(mode_str, 5, 15, 10, NEUTRAL);
            sprintf(mode_str, "FRAME: %d Hz", vk.iteration);
            DrawText(mode_str, 5, 25, 10, NEUTRAL);
        } else {
            sprintf(mode_str, "SIM: %.0f Hz", 1e6 / vk.computation_time);
            DrawText(mode_str, 10, 5, 20 / 2, NEUTRAL);
            sprintf(mode_str, "FPS: %d Hz", GetFPS());
            DrawText(mode_str, 10, 15, 20 / 2, NEUTRAL);
            sprintf(mode_str, "FRAME: %d", vk.iteration);
            DrawText(mode_str, 10, 25, 20 / 2, NEUTRAL);
        }

        EndDrawing();
    }

    CloseWindow();

    if (flag_record) {
        pclose(ffmpeg);
    }
    return 0;
}