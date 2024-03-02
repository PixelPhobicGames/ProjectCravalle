#include "Video.hpp"
#include "raylib.h"
#include <cstddef>

static int R = 0;
static int G = 0;
static int B = 0;

int Direction = 1;

bool FadeInDone = false;
bool FadeOutDone = false;

Color FadeColor = (Color){R, G, B, 255};


void PlayFadeIn() {
    Direction = 1;
    R = 0;
    G = 0;
    B = 0;
    FadeInDone = false;
}

void PlayFadeOut() {
    Direction = 0;
    R = 254;
    G = 254;
    B = 254;
    FadeInDone = false;
}

void UpdateFade() {
    if (Direction == 1) {
        FadeOutDone = false;
        R += 1;
        G += 1;
        B += 1;

        if (R == 254) {
            FadeInDone = true;
            Direction = 3;
        }
    }

    if (Direction == 0) {
        FadeInDone = false;
        R -= 1;
        G -= 1;
        B -= 1;

        if (R == 0) {
            FadeOutDone = true;
            Direction = 3;
        }
    }

    FadeColor = (Color){R, G, B, 255};
}

static ray_video_t LogoVideo;
static ray_video_t StudioVideo;

void PlayLogo() {

    LogoVideo = ray_video_open("GameData/Global/Opening.mpg");
    StudioVideo = ray_video_open("GameData/Global/Studio.mpg");

    while (true && !IsKeyPressed(KEY_SPACE)) {
        BeginDrawing();
        ray_video_update(&LogoVideo, GetFrameTime());

        UpdateFade();

        DrawTexturePro(LogoVideo.texture,
                       (Rectangle){0, 0, LogoVideo.texture.width, LogoVideo.texture.height},
                       (Rectangle){0, 0, float(GetScreenWidth()), float(GetScreenHeight())},
                       (Vector2){0, 0},
                       0.f,
                       WHITE);

        if (LogoVideo.video_state == RAY_VIDEO_STATE_DONE)
            break;

        EndDrawing();
    }

    PlayFadeIn();

    while (true && !IsKeyPressed(KEY_SPACE)) {
        BeginDrawing();
        ray_video_update(&StudioVideo, GetFrameTime());

        UpdateFade();

        DrawTexturePro(StudioVideo.texture,
                       (Rectangle){0, 0, LogoVideo.texture.width, LogoVideo.texture.height},
                       (Rectangle){0, 0, float(GetScreenWidth()), float(GetScreenHeight())},
                       (Vector2){0, 0},
                       0.f,
                       WHITE);

        if (StudioVideo.video_state == RAY_VIDEO_STATE_DONE) {
            PlayFadeOut();
            break;
        }

        EndDrawing();
    }
}