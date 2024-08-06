#ifndef LUMINA_GAME_H
#define LUMINA_GAME_H

#include "raylib.h"
#include "../../libPPG/PPGIO/PPGIO.hpp"
#include "Json/cJSON.h"

#define MaxCollisionSize 500

class Lumina {
public:
    Texture2D TileLayer;
    Camera2D LCamera;

    Texture2D SingleFrame1;
    Texture2D SingleFrame2;

    int Collision[MaxCollisionSize][MaxCollisionSize];

    int PlayerX;
    int PlayerY;

    int Speed;

    int Ticker;

    bool PlayerIsJumping;
    int PlayerJumpCount;

    int LevelWidth;
    int LevelHeight;
};

extern RenderTexture2D LuminaTarget;
extern bool ExitLuminaFlag;
extern bool UseLumina;
extern Lumina LuminaData;

void ReadCSVFile(std::string filename, int Array[MaxCollisionSize][MaxCollisionSize], int& rows, int& cols);
void SetPlayer();
void InitLumina(const char* Path);
void LuminaDrawWorld();

#endif // LUMINA_GAME_H
