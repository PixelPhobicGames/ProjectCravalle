#include "Editor.hpp"
#include "Entities.hpp"
#include "External/raygui/Style.h"
#include "Misc.hpp"
#include "OTVideo.hpp"
#include "Player.hpp"
#include "QMaps.hpp"

#include "libPPG/Parasite/ParasiteScript.hpp"
#include "libPPG/ParticleDemon/ParticleDemon.hpp"
#include "libPPG/Security/SigCheck.hpp"

#include "External/rlights/rlights.h"
#include "raylib.h"

#include <chrono>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <thread>
#include <vector>

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

#define ModelCount 21
#define MaxCachedModels 200
#define MaxMapSize 4096
#define MaxWStringAlloc 1048576

static bool Debug = false;
static bool FPSEnabled = false;
static bool UIToggle = true;
static bool ConsoleToggle = false;


static wstring WorldData;
static wstring OtherWDLData;
static wstring FinalWDL;

static float AmbientLightValues[4] = {0.1f, 0.1f, 0.1f, 0.1f};
static float TerrainHeightMap[MaxMapSize][MaxMapSize];

class EngineData {
  public:

    Camera RenderCamera = {0};
    Shader Lights;
    Light GameLights[MAX_LIGHTS];
    RenderTexture2D RenderTarget;

    int LevelIndex = 1;
    int Ticker = 0;
    int CameraSpeed = 1;

    int PopInRadius = 800;
    int LODSwapRadius = 160;

    void InitCamera() {
        RenderCamera.position = (Vector3){0.0f, 9.0f, 0.0f};
        RenderCamera.target = (Vector3){0.0f, 10.0f, 0.0f};
        RenderCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
        RenderCamera.fovy = 60.0f;
        RenderCamera.projection = CAMERA_PERSPECTIVE;
    }
};

static EngineData OTCoreData;

class Skybox{
    public:

        Model SkyboxModel;
        Shader CubemapShader;
        Color SColour;

        void InitSkybox(){
            Mesh Cube = GenMeshCube(1.0f, 1.0f, 1.0f);
            SkyboxModel = LoadModelFromMesh(Cube);
            SkyboxModel.materials[0].shader = LoadShader("GameData/Shaders/Skybox/skybox.vs", "GameData/Shaders/Skybox/skybox.fs");

            bool UseHDR = false;

            int MaterialMapCubemap = MATERIAL_MAP_CUBEMAP;
            int DoGamma = UseHDR ? 1 : 0;
            int VFlipped = UseHDR ? 1 : 0;

            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "environmentMap"), &MaterialMapCubemap, SHADER_UNIFORM_INT);
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "doGamma"), &DoGamma, SHADER_UNIFORM_INT);
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "vflipped"), &VFlipped, SHADER_UNIFORM_INT);

            CubemapShader = LoadShader("GameData/Shaders/Skybox/cubemap.vs", "GameData/Shaders/Skybox/cubemap.fs");

            int EquirectangularMapValue = 0;
            SetShaderValue(CubemapShader, GetShaderLocation(CubemapShader, "equirectangularMap"), &EquirectangularMapValue, SHADER_UNIFORM_INT);
        }

        void LoadSkybox(const char* path){
            Image SkyboxImage = LoadImage(path);
            SkyboxModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(SkyboxImage, CUBEMAP_LAYOUT_AUTO_DETECT);
            UnloadImage(SkyboxImage);
        }

        void Draw(){
            rlDisableBackfaceCulling();
            rlDisableDepthMask();

                DrawModel(SkyboxModel, (Vector3){0, 0, 0}, 1.0f, SColour);

            rlEnableBackfaceCulling();
            rlEnableDepthMask();
        }
};

static Skybox OTSkybox;

class Terrain {
    public:
        Vector3 HeightMapPosition;

        int HeightMapW = 0;
        int HeightMapH = 0;

        Model HeightMap;
        Texture2D HeightMapTexture;
};

static Terrain TerrainData;

class GameModels {
  public:
    Model ModelData;
    Texture2D ModelTexture;
};

static GameModels MapModels[ModelCount];
static GameModels LowDetalMapModels[ModelCount];

bool LowDetail[ModelCount];

static GameModels FastModels[6];

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

static GameSounds OTSoundData;

void CheckKey() {
#ifdef Linux
    char OSHostname[100];

    gethostname(OSHostname, 100);

    wstring EncodedHostname = CharArrayToWString(OSHostname);

    EncodedHostname = Encode(EncodedHostname, MainKey);

    if (!IsPathFile("GameData/Key/Key.sig")) {
        ofstream SigFile("GameData/Key/Key.sig");

        if (SigFile.is_open()) {
            SigFile << TextFormat("%ls", EncodedHostname.c_str());
            SigFile.close();
        } 
    } else {
        wstring Data = LoadFile("GameData/Key/Key.sig");
        if (!(Data[3] == EncodedHostname[3])) {
            exit(0);
        }
    }
#endif
}

void SaveGame() {
    wstring TFlags = L"";

    for (int i = 0; i <= 99; i++) {
        if (ToggleFlags[i].Value == 1) {
            TFlags += L'1';
        }
        if (ToggleFlags[i].Value == 0) {
            TFlags += L'0';
        }
    }

    wofstream Outfile;
    Outfile.open("GameData/Saves/TF.sav");
    Outfile << TFlags;

    wstring Position =
        to_wstring(OTCoreData.RenderCamera.position.x) + L':' + to_wstring(OTCoreData.RenderCamera.position.y) +
        L':' + to_wstring(OTCoreData.RenderCamera.position.z) + L':' + to_wstring(OTCoreData.LevelIndex) + L':';

    wofstream Outfile1;
    Outfile1.open("GameData/Saves/POS.sav");
    Outfile1 << Position;

    wofstream Outfile2;
    Outfile2.open("GameData/Saves/Script.sav");
    Outfile2 << ExtraWDLInstructions;

    WritePMemPage();
}

void LoadSave() {
    wstring TFlags = LoadFile("GameData/Saves/TF.sav");

    for (int i = 0; i <= 99; i++) {
        if (TFlags[i] == L'1') {
            ToggleFlags[i].Value = 1;
        }
        if (TFlags[i] == L'0') {
            ToggleFlags[i].Value = 0;
        }
    }

    wstring Position = LoadFile("GameData/Saves/POS.sav");

    OTCoreData.LevelIndex = int(ToFloat(WSplitValue(Position, 3)));

    SetCameraFlag = true;

    int X = ToFloat(WSplitValue(Position, 0));
    int Y = ToFloat(WSplitValue(Position, 1));
    int Z = ToFloat(WSplitValue(Position, 2));
    SetCameraPos = {float(X), float(Y), float(Z)};

    ExtraWDLInstructions = LoadFile("GameData/Saves/Script.sav");

    RestorePMemPage();
}
