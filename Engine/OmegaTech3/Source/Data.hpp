#include "Editor.hpp"
#include "Entities.hpp"

#include "Misc.hpp"
#include "Player.hpp"
#include "External/raygui/Style.h"
#include "Video.hpp"
#include "libPPG/Parasite/ParasiteScript.hpp"
#include "libPPG/ParticleDemon/ParticleDemon.hpp"
#include "libPPG/Rumble.hpp"
#include "libPPG/Security/SigCheck.hpp"

#include "External/rlights/rlights.h"
#include "raylib.h"

#include <cstddef>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

using namespace std;



// Define for GBEngine Functionality

// #define UseGBEngine  


// Define for Lumina Functionality

// #define UseLumina  

#ifdef UseLumina  

#include "Lumina/Lumina.hpp"

#endif

#ifdef UseGBEngine 

#include "External/GBEngine/GBEngine.hpp"

#endif 

#define MaxCachedModels 200
#define MaxMapSize 2000
#define MaxWStringAlloc 1048576

bool FloorCollision = true;
bool ObjectCollision = false;

static bool Debug = false;
static bool HeadBob = true;
static bool FPSEnabled = false;
static bool ConsoleToggle = false;
static bool UIToggle = true;
static RenderTexture2D Target;


static wstring WorldData;
static wstring OtherWDLData;
static wstring FinalWDL;

static float AmbientLightValues[4] = {0.1f, 0.1f, 0.1f, 0.1f};
static float TerrainHeightMap[MaxMapSize][MaxMapSize];

#define MaxGrass 1000

int RenderedGrass = 320;
Vector3 GrassPositions[MaxGrass];
bool GrassScan = true;
int GrassTicker = 0;


class EngineData {
  public:
    int LevelIndex = 1;
    Camera MainCamera = {0};

    Shader FinalShader;
    Shader Bloom;
    Shader Lights;

    Light GameLights[MAX_LIGHTS];

    ParticleSystem RainParticles;
    Texture HomeScreen;
    ray_video_t HomeScreenVideo;
    Music HomeScreenMusic;

    int Ticker = 0;
    int CameraSpeed = 1;
    int RenderRadius = 1000;
    int PopInRadius = 800;
    int LODSwapRadius = 160;

    bool UseCachedRenderer = true;
    int BadPreformaceCounter = 0;

    Texture2D Cursor;

    void InitCamera() {
        MainCamera.position = (Vector3){0.0f, 9.0f, 0.0f};
        MainCamera.target = (Vector3){0.0f, 10.0f, 0.0f};
        MainCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
        MainCamera.fovy = 60.0f;
        MainCamera.projection = CAMERA_PERSPECTIVE;
    }
};

static EngineData OmegaTechData;

void GenHeights(Image heightmap, Vector3 size) {
    #define GRAY_VALUE(c) ((float)(c.r + c.g + c.b) / 3.0f)

    int mapX = size.x;
    int mapZ = size.z;

    for (int z = 0; z < mapX - 1; z++) {
        for (int x = 0; x < mapZ - 1; x++) {
            TerrainHeightMap[z][x] = 0.0f;
        }
    }

    Color *pixels = LoadImageColors(heightmap);

    float Scale = heightmap.width / size.x;

    for (int z = 0; z < (mapZ)-1; z++) {
        for (int x = 0; x < (mapX)-1; x++) {
            float Index = ((x * Scale) + (z * Scale) * heightmap.width);
            TerrainHeightMap[z][x] = GRAY_VALUE(pixels[int(Index)]) / (255 / size.y);
        }
    }
}


class GameModels {
  public:

    Vector3 HeightMapPosition;
    int HeightMapW = 0;
    int HeightMapH = 0;
    Model HeightMap;
    Texture2D HeightMapTexture;

    Model Model1;
    Texture2D Model1Texture;

    Model Model2;
    Texture2D Model2Texture;

    Model Model3;
    Texture2D Model3Texture;

    Model Model4;
    Texture2D Model4Texture;

    Model Model5;
    Texture2D Model5Texture;

    Model Model6;
    Texture2D Model6Texture;

    Model Model7;
    Texture2D Model7Texture;

    Model Model8;
    Texture2D Model8Texture;

    Model Model9;
    Texture2D Model9Texture;

    Model Model10;
    Texture2D Model10Texture;

    Model Model11;
    Texture2D Model11Texture;

    Model Model12;
    Texture2D Model12Texture;

    Model Model13;
    Texture2D Model13Texture;

    Model Model14;
    Texture2D Model14Texture;

    Model Model15;
    Texture2D Model15Texture;

    Model Model16;
    Texture2D Model16Texture;

    Model Model17;
    Texture2D Model17Texture;

    Model Model18;
    Texture2D Model18Texture;

    Model Model19;
    Texture2D Model19Texture;

    Model Model20;
    Texture2D Model20Texture;

    // GLTS Models

    // For Faster Rendering.

    Model FastModel1;
    Texture2D FastModel1Texture;
    Model FastModel2;
    Texture2D FastModel2Texture;
    Model FastModel3;
    Texture2D FastModel3Texture;
    Model FastModel4;
    Texture2D FastModel4Texture;
    Model FastModel5;
    Texture2D FastModel5Texture;
};

static GameModels WDLModels;

class GameLODModels {
  public:
    bool LODAvailableModel1 = false;
    bool LODAvailableModel2 = false;
    bool LODAvailableModel3 = false;
    bool LODAvailableModel4 = false;
    bool LODAvailableModel5 = false;
    bool LODAvailableModel6 = false;
    bool LODAvailableModel7 = false;
    bool LODAvailableModel8 = false;
    bool LODAvailableModel9 = false;
    bool LODAvailableModel10 = false;
    bool LODAvailableModel11 = false;
    bool LODAvailableModel12 = false;
    bool LODAvailableModel13 = false;
    bool LODAvailableModel14 = false;
    bool LODAvailableModel15 = false;
    bool LODAvailableModel16 = false;
    bool LODAvailableModel17 = false;
    bool LODAvailableModel18 = false;
    bool LODAvailableModel19 = false;
    bool LODAvailableModel20 = false;

    Model Model1;
    Model Model2;
    Model Model3;
    Model Model4;
    Model Model5;
    Model Model6;
    Model Model7;
    Model Model8;
    Model Model9;
    Model Model10;
    Model Model11;
    Model Model12;
    Model Model13;
    Model Model14;
    Model Model15;
    Model Model16;
    Model Model17;
    Model Model18;
    Model Model19;
    Model Model20;

    void ClearModelInfo() {
        // Unload models if they are loaded

        if (LODAvailableModel1) {
            UnloadModel(Model1);
            LODAvailableModel1 = false;
        }

        if (LODAvailableModel2) {
            UnloadModel(Model2);
            LODAvailableModel2 = false;
        }

        if (LODAvailableModel3) {
            UnloadModel(Model3);
            LODAvailableModel3 = false;
        }

        if (LODAvailableModel4) {
            UnloadModel(Model4);
            LODAvailableModel4 = false;
        }

        if (LODAvailableModel5) {
            UnloadModel(Model5);
            LODAvailableModel5 = false;
        }

        if (LODAvailableModel6) {
            UnloadModel(Model6);
            LODAvailableModel6 = false;
        }

        if (LODAvailableModel7) {
            UnloadModel(Model7);
            LODAvailableModel7 = false;
        }

        if (LODAvailableModel8) {
            UnloadModel(Model8);
            LODAvailableModel8 = false;
        }

        if (LODAvailableModel9) {
            UnloadModel(Model9);
            LODAvailableModel9 = false;
        }

        if (LODAvailableModel10) {
            UnloadModel(Model10);
            LODAvailableModel10 = false;
        }

        if (LODAvailableModel11) {
            UnloadModel(Model11);
            LODAvailableModel11 = false;
        }

        if (LODAvailableModel12) {
            UnloadModel(Model12);
            LODAvailableModel12 = false;
        }

        if (LODAvailableModel13) {
            UnloadModel(Model13);
            LODAvailableModel13 = false;
        }

        if (LODAvailableModel14) {
            UnloadModel(Model14);
            LODAvailableModel14 = false;
        }

        if (LODAvailableModel15) {
            UnloadModel(Model15);
            LODAvailableModel15 = false;
        }

        if (LODAvailableModel16) {
            UnloadModel(Model16);
            LODAvailableModel16 = false;
        }

        if (LODAvailableModel17) {
            UnloadModel(Model17);
            LODAvailableModel17 = false;
        }

        if (LODAvailableModel18) {
            UnloadModel(Model18);
            LODAvailableModel18 = false;
        }

        if (LODAvailableModel19) {
            UnloadModel(Model19);
            LODAvailableModel19 = false;
        }

        if (LODAvailableModel20) {
            UnloadModel(Model20);
            LODAvailableModel20 = false;
        }
    }
};

static GameLODModels LODWDLModels;

class GameData {
  public:
    float X;
    float Y;
    float Z;
    float R;
    float S;
    int ModelId;
    bool Collision;
    bool AlwaysRender;

    void Init() {
        X = 0;
        Y = 0;
        Z = 0;
        R = 0;
        S = 0;
        ModelId = 0;
        Collision = false;
        AlwaysRender = false;
    }
};

static int CachedModelCounter = 0;
static GameData CachedModels[MaxCachedModels];

class CollisionData {
  public:
    float X;
    float Y;
    float Z;
    float W;
    float H;
    float L;

    void Init() {
        X = 0;
        Y = 0;
        Z = 0;
        W = 0;
        H = 0;
        L = 0;
    }
};

static int CachedCollisionCounter = 0;
static CollisionData CachedCollision[MaxCachedModels];

class GameSounds {
  public:
    Sound CollisionSound;
    Sound WalkingSound;
    Sound UIClick;
    Sound ChasingSound;
    Sound Death;
    Music NESound1;
    Music NESound2;
    Music NESound3;

    Music BackgroundMusic;

    bool MusicFound = false;
};

static GameSounds OmegaTechSoundData;

void DrawTerrainMap() {
    for (int z = 0; z < MaxMapSize; z++) {
        for (int x = 0; x < MaxMapSize; x++) {
            DrawCube({float(x), TerrainHeightMap[z][x], float(z)}, 0.5, 0.5, 0.5, RED);
        }
    }
}

bool TextBox001EditMode = false;
char TextBox001Text[128] = "";
bool TextBox002EditMode = false;
char TextBox002Text[1024] = "OmegaConsole\n";

void PrintConsole(const char *Message) {
    strcat(TextBox002Text, Message);
}

void ConsoleParseCommand(const char *Command) {
    bool Found = false;

    string CommandValue(Command);

    if (CommandValue == "ExportTerrain") {
        PrintConsole("Exporting Terrain As Mesh\n");
        if (ExportMesh(WDLModels.HeightMap.meshes[0], "Terrain.obj")) {
            PrintConsole("Done.. Stored in Terrain.obj\n");
        }

        Found = true;
    }

    if (CommandValue == "SigGen") {

        string Data = WstringToString(WorldData);
        string NewSig = SignatureGen<string>(Data);

        cout << NewSig << "\n";

        PrintConsole("Done.\n");
        Found = true;
    }

    if (CommandValue == "Rumble") {
        RumblePulse();
        Found = true;
    }

    if (!Found) {
        PrintConsole("Command Not Found: ");
        PrintConsole(Command);
        PrintConsole("<--\n");
    }
}

int CommandCounter = 0;

void DrawConsole() {
    GuiPanel((Rectangle){0, 40, 616, 288}, NULL);

    if (GuiTextBox((Rectangle){8, 296, 600, 24}, TextBox001Text, 128, TextBox001EditMode))
        TextBox001EditMode = !TextBox001EditMode;

    if (GuiTextBox((Rectangle){8, 48, 600, 240}, TextBox002Text, 1024, TextBox002EditMode)) {
    }

    if (IsKeyPressed(KEY_ENTER)) {
        ConsoleParseCommand(TextBox001Text);
        TextBox001Text[0] = '\0';
        CommandCounter ++;

        if (CommandCounter == 2){
            TextBox002Text[0] = '\0';
            CommandCounter = 0;
        }
    }
}

void CheckKey() {
    #ifdef Linux
        char hostname[100];
        gethostname(hostname, 100);

        cout << "USER: " << hostname << "\n";

        wstring encodedData = CharArrayToWString(hostname);
        encodedData = Encode(encodedData, MainKey);

        if (!IsPathFile("GameData/Key/Key.sig")) {
            ofstream SigFile("GameData/Key/Key.sig");

            if (SigFile.is_open()) {
                SigFile << TextFormat("%ls", encodedData.c_str());
                SigFile.close();
            } else {
                cerr << "Error opening file for writing.\n";
            }
        } else {
            wstring Data = LoadFile("GameData/Key/Key.sig");

            if (!(Data[3] == encodedData[3])) {
                wcout << Data << " != " << encodedData << "\n";
                exit(0);
            }
        }
    #endif
}
