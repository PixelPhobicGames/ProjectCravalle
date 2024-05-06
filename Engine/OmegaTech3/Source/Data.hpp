#include "Editor.hpp"
#include "Entities.hpp"
#include "External/raygui/Style.h"
#include "Misc.hpp"
#include "OTVideo.hpp"
#include "Player.hpp"

#include "libPPG/Parasite/ParasiteScript.hpp"
#include "libPPG/ParticleDemon/ParticleDemon.hpp"
#include "libPPG/Rumble.hpp"
#include "libPPG/Security/SigCheck.hpp"

#include "External/rlights/rlights.h"
#include "raylib.h"

#include <chrono>
#include <cstddef>
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

#define ModelCount 21

#define MaxCachedModels 200
#define MaxMapSize 4096
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

bool GrassScan = true;
int GrassTicker = 0;

Vector3 GrassPositions[MaxGrass];

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

            int materialMapCubemap = MATERIAL_MAP_CUBEMAP;
            int doGamma = UseHDR ? 1 : 0;
            int vFlipped = UseHDR ? 1 : 0;

            // Modify the problematic lines
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "environmentMap"), &materialMapCubemap, SHADER_UNIFORM_INT);
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "doGamma"), &doGamma, SHADER_UNIFORM_INT);
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "vflipped"), &vFlipped, SHADER_UNIFORM_INT);

            CubemapShader = LoadShader("GameData/Shaders/Skybox/cubemap.vs", "GameData/Shaders/Skybox/cubemap.fs");

            int equirectangularMapValue = 0;
            SetShaderValue(CubemapShader, GetShaderLocation(CubemapShader, "equirectangularMap"), &equirectangularMapValue, SHADER_UNIFORM_INT);
        }

        void LoadSkybox(const char* path){
            Image img = LoadImage(path);
            SkyboxModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
            UnloadImage(img);
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

static GameModels WDLModels[ModelCount];
static GameModels WLowDetailModels[ModelCount];

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

static GameSounds OmegaTechSoundData;



void UnloadGame(){

    // Unload World Data 

    if (IsModelLoaded(&TerrainData.HeightMap)){
        UnloadModel(TerrainData.HeightMap);
    }
    
    if (TerrainData.HeightMapTexture.width != NULL){
        UnloadTexture(TerrainData.HeightMapTexture);
    }

    for (int i = 1; i <= ModelCount - 1; i ++){ 
        if (IsModelLoaded(&WDLModels[i].ModelData)){
            UnloadModel(WDLModels[i].ModelData);
        }

        if (WDLModels[i].ModelTexture.width != NULL){
            UnloadTexture(WDLModels[i].ModelTexture);
        }

        if ( LowDetail[i] ){
            if (IsModelLoaded(&WLowDetailModels[i].ModelData)){
                UnloadModel(WLowDetailModels[i].ModelData);
            }

            if (WLowDetailModels[i].ModelTexture.width != NULL){
                UnloadTexture(WLowDetailModels[i].ModelTexture);
            }
        }
    }

    for (int i = 0; i <= 5 ; i ++ ){
        if (IsModelLoaded(&FastModels[i].ModelData)){
            UnloadModel(FastModels[i].ModelData);
        }

        if (FastModels[i].ModelTexture.width != NULL){
            UnloadTexture(FastModels[i].ModelTexture);
        }
    }

    UnloadShader(OTSkybox.CubemapShader);
    UnloadRenderTexture(ParasiteTarget);
    UnloadRenderTexture(Target);

    UnloadFont(OmegaTechTextSystem.BarFont );
    UnloadFont(OmegaTechTextSystem.RussianBarFont);
    UnloadFont(OmegaTechTextSystem.LatinBarFont );
    UnloadFont(OmegaTechTextSystem.JapaneseBarFont );

    UnloadTexture(OmegaTechTextSystem.Bar);
    UnloadTexture(OmegaTechData.Cursor);

    UnloadSound(OmegaTechSoundData.CollisionSound );
    UnloadSound(OmegaTechSoundData.WalkingSound);
    UnloadSound(OmegaTechSoundData.ChasingSound);
    UnloadSound(OmegaTechSoundData.UIClick);
    UnloadSound(OmegaTechSoundData.Death );
    UnloadSound(OmegaTechTextSystem.TextNoise );
}

void DrawTerrainMap() {
    for (int z = 0; z < MaxMapSize; z++) {
        for (int x = 0; x < MaxMapSize; x++) {
            DrawCube({float(x), TerrainHeightMap[z][x], float(z)}, 0.5, 0.5, 0.5, RED);
        }
    }
}

