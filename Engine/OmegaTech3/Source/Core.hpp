#include "Worlds.hpp"

void OTInit() {
    ParasiteScriptTFlagWipe();

    GuiLoadStyle();

#ifdef USESIG
    InitSig();
#endif

    OTCoreData.InitCamera();

    OTCoreData.Lights = LoadShader("GameData/Shaders/Lights/Lighting.vs", "GameData/Shaders/Lights/Fog.fs");

    OTCoreData.GameLights[MAX_LIGHTS] = {0};

    OTCoreData.Lights.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(OTCoreData.Lights, "viewPos");

    float FogDensity = 0.005f;
    int FogDensityLoc = GetShaderLocation(OTCoreData.Lights, "fogDensity");

    SetShaderValue(OTCoreData.Lights, FogDensityLoc, &FogDensity, SHADER_UNIFORM_FLOAT);

    OTTextSystem.Bar = LoadTexture("GameData/Global/TextBar.png");
    OTTextSystem.BarFont = LoadFont("GameData/Global/Font.ttf");
    OTTextSystem.RussianBarFont = LoadFont("GameData/Global/Cyrillic.fnt");
    OTTextSystem.LatinBarFont = LoadFont("GameData/Global/Latin.fnt");
    OTTextSystem.JapaneseBarFont = LoadFont("GameData/Global/Hiragana.fnt");

    OTSoundData.CollisionSound = LoadSound("GameData/Global/Sounds/CollisionSound.mp3");
    OTSoundData.WalkingSound = LoadSound("GameData/Global/Sounds/WalkingSound.mp3");
    OTSoundData.ChasingSound = LoadSound("GameData/Global/Sounds/ChasingSound.mp3");
    OTSoundData.UIClick = LoadSound("GameData/Global/Title/Click.mp3");
    OTSoundData.Death = LoadSound("GameData/Global/Sounds/Hurt.mp3");

    OTTextSystem.TextNoise = LoadSound("GameData/Global/Sounds/TalkingNoise.mp3");

    for (int i = 1 ; i <= 5; i ++){
        if (IsPathFile(TextFormat("GameData/Global/FModels/FModel%i.gltf", i))) {
            FastModels[i].ModelData = LoadModel(TextFormat("GameData/Global/FModels/FModel%i.gltf", i));
            FastModels[i].ModelTexture = LoadTexture(TextFormat("GameData/Global/FModels/FModel%iTexture.png", i));
            FastModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = FastModels[i].ModelTexture;
            FastModels[i].ModelData.materials[0].shader = OTCoreData.Lights;
        }
    }


    OTCoreData.RenderTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    ParasiteTarget = LoadRenderTexture(1280, 720);

    WorldData.reserve(MaxWStringAlloc);
    OtherWDLData.reserve(MaxWStringAlloc);
    ExtraWDLInstructions.reserve(MaxWStringAlloc);

    FinalWDL.reserve(MaxWStringAlloc * 3);

    OTSkybox.InitSkybox();
}


bool LDropdownBoxEditMode = false;
int LDropdownBoxActive = 0;

void PlayHomeScreen() {

    // OTVideo HomeScreenVideo;
    Music HomeScreenMusic;
    Texture2D Cursor;
    Texture2D TitleImage = LoadTexture("GameData/Global/Title/Title.png");

    int MousePoint = 0;

    // if (IsPathFile("GameData/Global/Title/Title.mpg")) HomeScreenVideo.Load("GameData/Global/Title/Title.mpg");

    if (IsPathFile("GameData/Global/Title/Title.mp3")){
        HomeScreenMusic = LoadMusicStream("GameData/Global/Title/Title.mp3");
        PlayMusicStream(HomeScreenMusic);
    }

    if (IsPathFile("GameData/Global/Cursor.png")) Cursor = LoadTexture("GameData/Global/Cursor.png");
    

    while (true && !WindowShouldClose()) {

        if (IsMusicReady(HomeScreenMusic)) UpdateMusicStream(HomeScreenMusic);

        BeginTextureMode(OTCoreData.RenderTarget);

        ClearBackground(BLACK);

        if (OTInputController.InteractPressed) PlaySound(OTSoundData.UIClick);
        
        DrawTexture(TitleImage, 0, 0, WHITE);

        GuiLine((Rectangle){144, 264, 216, 16}, NULL);
        GuiLine((Rectangle){136, 336, 216, 16}, NULL);

        if (GuiButton((Rectangle){168, 256 - 48, 176, 48}, "Start") ||
            (MousePoint == 0 && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) {
            if (LDropdownBoxActive == EnglishPack) {
                LoadLanguagePack(EnglishPack, &GlobalPackData);
            }
            if (LDropdownBoxActive == RussianPack) {
                LoadLanguagePack(RussianPack, &GlobalPackData);
            }
            if (LDropdownBoxActive == SpanishPack) {
                LoadLanguagePack(SpanishPack, &GlobalPackData);
            }
            if (LDropdownBoxActive == JapanesePack) {
                LoadLanguagePack(JapanesePack, &GlobalPackData);
            }
            if (LDropdownBoxActive == FrenchPack) {
                LoadLanguagePack(FrenchPack, &GlobalPackData);
            }

            OTTextSystem.LanguageType = GlobalPackData.Type;

            if (IsMusicReady(HomeScreenMusic)) UnloadMusicStream(HomeScreenMusic);
            
            UnloadTexture(Cursor);
            UnloadTexture(TitleImage);
            
            UnloadRenderTexture(OTCoreData.RenderTarget);
            OTCoreData.RenderTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
            break;
        }

        if (GuiButton((Rectangle){144, 288, 176, 48}, "Load")) {
            UnloadRenderTexture(OTCoreData.RenderTarget);
            OTCoreData.RenderTarget = LoadRenderTexture(GetScreenWidth() / 4, GetScreenHeight() / 4);

            if (LDropdownBoxActive == EnglishPack) {
                LoadLanguagePack(EnglishPack, &GlobalPackData);
            }
            if (LDropdownBoxActive == RussianPack) {
                LoadLanguagePack(RussianPack, &GlobalPackData);
            }
            if (LDropdownBoxActive == SpanishPack) {
                LoadLanguagePack(SpanishPack, &GlobalPackData);
            }
            if (LDropdownBoxActive == JapanesePack) {
                LoadLanguagePack(JapanesePack, &GlobalPackData);
            }
            if (LDropdownBoxActive == FrenchPack) {
                LoadLanguagePack(FrenchPack, &GlobalPackData);
            }

            OTTextSystem.LanguageType = GlobalPackData.Type;

            if (IsMusicReady(HomeScreenMusic))UnloadMusicStream(HomeScreenMusic);

            UnloadTexture(Cursor);
            UnloadTexture(TitleImage);

            if (IsPathFile("GameData/Saves/TF.sav")) {
                LoadSave();
            }
            break;
        }

        EndTextureMode();
        BeginDrawing();

        DrawTexturePro(OTCoreData.RenderTarget.texture,
                       (Rectangle){0, 0, OTCoreData.RenderTarget.texture.width, -OTCoreData.RenderTarget.texture.height},
                       (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                       (Vector2){0, 0},
                       0.f,
                       WHITE);
        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE)) exit(0);
    }

    if (IsMusicReady(HomeScreenMusic)) StopMusicStream(HomeScreenMusic);
}


void UpdatePlayer() {
    if (IsKeyDown(KEY_W) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 && !Debug) {
        if (!IsSoundPlaying(OTSoundData.WalkingSound)) {
            PlaySound(OTSoundData.WalkingSound);
        }
    }

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
        GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) != 0) {
        if (!IsSoundPlaying(OTSoundData.WalkingSound)) {
            PlaySound(OTSoundData.WalkingSound);
        }
    } else {
        if (IsSoundPlaying(OTSoundData.WalkingSound)) {
            StopSound(OTSoundData.WalkingSound);
        }
    }

    if (MapClipping && !QMapSystem.UsingQMaps) {
        if (OTCoreData.RenderCamera.position.z >= 0 && OTCoreData.RenderCamera.position.z <= TerrainData.HeightMapW) {
            if (OTCoreData.RenderCamera.position.x >= 0 &&
                OTCoreData.RenderCamera.position.x <= TerrainData.HeightMapW) {
                if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
                    GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 ||
                    GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) != 0) {
                    OTCoreData.RenderCamera.position.y = 8 +
                                                          TerrainHeightMap[int(OTCoreData.RenderCamera.position.z)]
                                                                          [int(OTCoreData.RenderCamera.position.x)];
                }
            }
        }
    }

    OTPlayer.PlayerBounds = (BoundingBox){(Vector3){OTCoreData.RenderCamera.position.x - OTPlayer.Width / 2,
                                                       OTCoreData.RenderCamera.position.y - OTPlayer.Height,
                                                       OTCoreData.RenderCamera.position.z - OTPlayer.Width / 2},
                                             (Vector3){OTCoreData.RenderCamera.position.x + OTPlayer.Width / 2,
                                                       OTCoreData.RenderCamera.position.y,
                                                       OTCoreData.RenderCamera.position.z + OTPlayer.Width / 2}};
}

void DrawWorld() {
    BeginTextureMode(OTCoreData.RenderTarget);
    ClearBackground(BLACK);

    if (!UsingCineFlow) {
        BeginMode3D(OTCoreData.RenderCamera);
    } else {
        BeginMode3D(CineFlowData.CFCamera);
    }

    if (IsMusicReady(OTSoundData.BackgroundMusic)) UpdateMusicStream(OTSoundData.BackgroundMusic);

    UpdateNoiseEmitters();


    CWDLProcess();
    WDLProcess();
    

    if (QMapSystem.UsingQMaps) QMapSystem.RenderQMap();
    if (!UsingCineFlow) UpdatePlayer();
    
    
    if (OTPlayer.ObjectCollision) {
        OTCoreData.RenderCamera.position.x = OTPlayer.OldX;
        OTCoreData.RenderCamera.position.y = OTPlayer.OldY;
        OTCoreData.RenderCamera.position.z = OTPlayer.OldZ;
        OTPlayer.ObjectCollision = false;
    }

    UpdateCustom(OTCoreData.LevelIndex);

    EndMode3D();

    EndTextureMode();

    if (SetSceneFlag) {
        OTCoreData.LevelIndex = SetSceneId;
        LoadWorld();
        SetSceneFlag = false;
    }

    if (SetCameraFlag) {
        OTCoreData.RenderCamera.position = SetCameraPos;
        SetCameraFlag = false;
    }

    if (ScriptTimer != 0) ScriptTimer--;

    if (OTCoreData.Ticker != 60) {
        OTCoreData.Ticker ++;
    } else {
        OTCoreData.Ticker = 0;
    }
}