#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <iostream>

#include "recorder.hpp"

Recorder::Recorder() {}

void Recorder::start(int width, int height) {
    if (!started) {
        started = true;
    }

    char cmd[200];
    time_t now = time(0);
    sprintf(cmd,
            "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgba -s %dx%d "
            "-r 60 -i - -c:v libx264 -preset ultrafast -crf 18 out/%ld.mp4",
            width, height, now);
    ffmpeg = popen(cmd, "w");
}

Recorder::~Recorder() {
    if (ffmpeg) {
        pclose(ffmpeg);
    }
}

void Recorder::record_frame(Texture2D texture){
    Image screen = LoadImageFromTexture(texture);
    ImageFlipVertical(&screen);
    fwrite(screen.data, 1, screen.width * screen.height * 4, ffmpeg);
    UnloadImage(screen);
}