// SPDX-FileCopyrightText: The Eigen Authors
// SPDX-License-Identifier: MPL-2.0

#include <Eigen/Dense>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iostream>

#include <ostream>
#include <raylib.h>
#include <raymath.h>

#include "recorder.hpp"
#include "vonkarman.hpp"

using Eigen::MatrixXd;

VonKarman vk;
bool flag_record = false;
bool flag_interactive = false;

Recorder recorder;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (std::string(argv[i]) == "--help" ||
                std::string(argv[i]) == "-h") {
                std::cout << "Usage: " << argv[0] << " [options]\n"
                          << "Options:\n"
                          << "  --help, -h    Show this help message\n"
                          << "  --record, -r  Record the simulation\n"
                          << "  --interactive, -i  Run in interactive mode\n";
                return 0;
            } else if (std::string(argv[i]) == "--interactive" ||
                       std::string(argv[i]) == "-i") {
                std::cout << "Interactive mode enabled.\n";
                flag_interactive = true;
            } else if (std::string(argv[i]) == "--record" ||
                       std::string(argv[i]) == "-r") {
                std::cout << "Recording enabled.\n";
                flag_record = true;
            }
        }
    }

    int width = nx * SCALING;
    int height = ny * SCALING;
    bool dual = !flag_interactive;

    if (dual) {
        height *= 2;
    }


    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(width, height, "Simulation de Von_Karman");

    vk.init();
    // if (!flag_interactive) {
        // double r = 0.1 * ny + 0.5;
        // double x0 = r*2;
        // double y0 = ny/2.0;
        // vk.O = ((vk.X - x0).square() + (vk.Y - y0).square() <= (r * r)).select(1.0, vk.O);

        // x0 = 4.5 * r;
        // y0 = ny/2.0 - 1.5*r;
        // vk.O = (((vk.X - x0).square() + (vk.Y - y0).square() <= (r * r))).select(1.0, vk.O);
        // x0 = 4.5 * r;
        // y0 = ny/2.0 + 1.5*r;
        // vk.O = (((vk.X - x0).square() + (vk.Y - y0).square() <= (r * r))).select(1.0, vk.O);

        // x0 = nx * 0.1;
        // y0 = ny/2.0 + 0.5;

        // auto c = nx/4;
        // auto t = sqrt((ny/10.0 + 1)/(1.1019*c));
        // auto profile = (t*c/0.2)*(0.2969*((vk.X - x0) / c).sqrt() - 0.126 * (vk.X - x0)/c - 0.3516*((vk.X - x0) / c).square() +0.2843*((vk.X - x0)/c).pow(3) - 0.1015*((vk.X - x0)/c).pow(4))>= (vk.Y - y0).abs() && ((vk.X - x0) < c) ;
        // vk.O = profile.select(1.0, vk.O);
    // }
    double r = 0.1 * ny + 0.5;
    auto cylindre =
        ((vk.X - nx*0.25).square() + (vk.Y - ny/2).square() <= (r * r));
    vk.O = cylindre.select(1.0, vk.O);


    // float h = ny*0.6;
    // float w = h*0.75;
    // Rectangle rect = {100,(ny-h)/2,w,h};
    // vk.load_image("./amogus.png", rect);
    // float h = ny*0.6;
    // float w = h*1.5;
    // Rectangle rect = {20,(ny-h)/2,w,h};
    // vk.load_image("./gta.png", rect);



    // https://arcade.pirillo.com/maze-maker.html
    if (flag_record) {
        recorder.start(width, height);
    } else {
        // Démarrage de la simulation en background
        SetTargetFPS(60);
        vk.start();
    }

    RenderTexture2D target = LoadRenderTexture(width, height);
    Texture2D buffer1;
    Texture2D buffer2;

    DrawMode current_mode = VELOCITY;

    int prev_frame_draw = 0;
    while (!WindowShouldClose()) {
        if (flag_record) {
            vk.time_calc();
        }

        if(flag_interactive){
            if (IsKeyPressed(KEY_TAB)) {
                current_mode = static_cast<DrawMode>(
                    (static_cast<int>(current_mode) + 1) % 3);
            }
        }

        if (dual) {
            buffer1 = vk.draw(VORTICITY);
            buffer2 = vk.draw(VELOCITY);
        } else {
            buffer1 = vk.draw(current_mode);
        }
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureRec(buffer1, Rectangle{0, 0, nx, ny}, Vector2{0, 0}, WHITE);

        if(dual) {
            DrawTextureRec(buffer2, Rectangle{0, 0, nx, ny}, Vector2{0, ny},
                           WHITE);
        }

        char txt[100];
        if (flag_record) {
            sprintf(txt, "RECORDING");
            DrawText(txt, 5, 5, 10, NEUTRAL);
            sprintf(txt, "FPS: %d Hz", GetFPS());
            DrawText(txt, 5, 15, 10, NEUTRAL);
            sprintf(txt, "FRAME: %d", vk.iteration);
            DrawText(txt, 5, 25, 10, NEUTRAL);
        } else {
            sprintf(txt, "SIM: %.0f Hz", 1e6 / vk.computation_time);
            DrawText(txt, 10, 5, 20 / 2, NEUTRAL);
            sprintf(txt, "FPS: %d Hz", GetFPS());
            DrawText(txt, 10, 15, 20 / 2, NEUTRAL);
            sprintf(txt, "FRAME: %d", vk.iteration);
            DrawText(txt, 10, 25, 20 / 2, NEUTRAL);
        }

        EndDrawing();


        if (flag_record && vk.iteration - prev_frame_draw >= ITER_PER_FRAME) {
            prev_frame_draw = vk.iteration;
            if(!dual) {
                buffer1 = vk.draw(VORTICITY);
                buffer2 = vk.draw(VELOCITY);
            }
            BeginTextureMode(target);
            DrawTextureRec(buffer1, Rectangle{0, 0, nx, ny}, Vector2{0, 0},
                           WHITE);
            DrawTextureRec(buffer2, Rectangle{0, 0, nx, ny}, Vector2{0, ny},
                           WHITE);
            EndTextureMode();
            recorder.record_frame(target.texture);
        }
    }

    CloseWindow();

    if (flag_record) {
        recorder.~Recorder();
    } else {
        vk.stop();
    }
    return 0;
}