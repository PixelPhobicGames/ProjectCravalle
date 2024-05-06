#include "Custom.hpp"

void LoadSave();
void SaveGame();

void CacheWDL();

void SpawnWDLProcess(const char *Path) {
    wstring WData;

    if (GameDataEncoded) {
        WData = Encode(LoadFile(Path), MainKey);
    } else {
        WData = LoadFile(Path);
    }

    int WDLSize = 0;

    for (int i = 0; i <= WData.size(); i++) {
        if (WData[i] == L':') {
            WDLSize++;
        }
    }

    int EntityCounter = 0;

    for (int i = 0; i <= WDLSize; i++) {
        wstring Instruction = WSplitValue(WData, i);

        if (Instruction == L"Light") {
            PutLight({ToFloat(WSplitValue(WData, i + 1)),
                      ToFloat(WSplitValue(WData, i + 2)),
                      ToFloat(WSplitValue(WData, i + 3))});
        }

        i += 3;
    }
}

bool LoadFlag = false;

void LoadWorld() {

    GrassScan = true;

#ifdef UseGBEngine
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Scene.gb", OmegaTechData.LevelIndex))) {
        InitGB(convertToChar(TextFormat("GameData/Worlds/World%i/Scene.gb", OmegaTechData.LevelIndex)));
        RunGB();

        OmegaTechData.LevelIndex++;
    }
#endif

    PlayFadeIn();
    ClearLights();

    SceneIDMirror = OmegaTechData.LevelIndex;

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OmegaTechData.LevelIndex))) {
        StopMusicStream(OmegaTechSoundData.NESound1);
        OmegaTechSoundData.NESound1 =
            LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OmegaTechData.LevelIndex));
    } else {
        UnloadMusicStream(OmegaTechSoundData.NESound1);
    }
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex))) {
        StopMusicStream(OmegaTechSoundData.NESound2);
        OmegaTechSoundData.NESound2 =
            LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex));
    } else {
        UnloadMusicStream(OmegaTechSoundData.NESound2);
    }
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE3.mp3", OmegaTechData.LevelIndex))) {
        StopMusicStream(OmegaTechSoundData.NESound3);
        OmegaTechSoundData.NESound3 =
            LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE3.mp3", OmegaTechData.LevelIndex));
    } else {
        UnloadMusicStream(OmegaTechSoundData.NESound3);
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OmegaTechData.LevelIndex))) {
        float Red = float((int(PullConfigValue(
                               TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OmegaTechData.LevelIndex),
                               0)) /
                           255));
        float Green = float((int(PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf",
                                                            OmegaTechData.LevelIndex),
                                                 1)) /
                             255));
        float Blue = float((int(PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf",
                                                           OmegaTechData.LevelIndex),
                                                2)) /
                            255));
        float Alpha = float((int(PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf",
                                                            OmegaTechData.LevelIndex),
                                                 3)) /
                             255));

        AmbientLightValues[0] = Red;
        AmbientLightValues[1] = Green;
        AmbientLightValues[2] = Blue;
        AmbientLightValues[3] = Alpha;
    }

    //LODWDLModels.ClearModelInfo();

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OmegaTechData.LevelIndex))) {
        OTSkybox.LoadSkybox(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OmegaTechData.LevelIndex));
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OmegaTechData.LevelIndex))) {
        TerrainData.HeightMapW =
            PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/HeightMap.conf", OmegaTechData.LevelIndex), 0);
        TerrainData.HeightMapH =
            PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/HeightMap.conf", OmegaTechData.LevelIndex), 1);

        TerrainData.HeightMapTexture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/HeightMapTexture.png", OmegaTechData.LevelIndex));
        Image HeightMapImage =
            LoadImage(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OmegaTechData.LevelIndex));
        Texture2D Texture = LoadTextureFromImage(HeightMapImage);

        // Generate Height map Unless pregenerated one exists.

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Terrain.obj", OmegaTechData.LevelIndex))) {
            TerrainData.HeightMap =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Terrain.obj", OmegaTechData.LevelIndex));
            TerrainData.HeightMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = TerrainData.HeightMapTexture;
            TerrainData.HeightMap.materials[0].shader = OmegaTechData.Lights;
        } else {
            Mesh Mesh1 = GenMeshHeightmap(HeightMapImage,
                                          (Vector3){TerrainData.HeightMapW, TerrainData.HeightMapH, TerrainData.HeightMapW});

            TerrainData.HeightMap = LoadModelFromMesh(Mesh1);
            TerrainData.HeightMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = TerrainData.HeightMapTexture;
            TerrainData.HeightMap.meshes[0] = Mesh1;
            TerrainData.HeightMap.materials[0].shader = OmegaTechData.Lights;
        }

        GenHeights(HeightMapImage, {(Vector3){TerrainData.HeightMapW, TerrainData.HeightMapH, TerrainData.HeightMapW}});
    }

    for (int i = 1; i <= ModelCount - 1; i ++){

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%i.obj", OmegaTechData.LevelIndex , i))){
            WDLModels[i].ModelData = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%i.obj", OmegaTechData.LevelIndex , i));
            WDLModels[i].ModelTexture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model%iTexture.png", OmegaTechData.LevelIndex , i));
            SetTextureFilter(WDLModels[i].ModelTexture, TEXTURE_FILTER_TRILINEAR);
            WDLModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels[i].ModelTexture;
            WDLModels[i].ModelData.materials[0].shader = OmegaTechData.Lights;

            if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.obj", OmegaTechData.LevelIndex , i))){
                WLowDetailModels[i].ModelData = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.obj", OmegaTechData.LevelIndex , i));
                WLowDetailModels[i].ModelTexture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model%iTexture.png", OmegaTechData.LevelIndex , i));
                SetTextureFilter(WLowDetailModels[i].ModelTexture, TEXTURE_FILTER_TRILINEAR);
                WLowDetailModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WLowDetailModels[i].ModelTexture;
                WLowDetailModels[i].ModelData.materials[0].shader =  OmegaTechData.Lights;
                LowDetail[i] = true;
            }
            else {
                LowDetail[i] = false;
            }
        }

        else {

            /*
            UnloadModel(WDLModels[i].ModelData);
            UnloadTexture(WDLModels[i].ModelTexture);
    
            if (LowDetail[i]){
                UnloadModel(WDLModels[i].ModelData);
                UnloadTexture(WDLModels[i].ModelTexture);
            }
            */
        }

        
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex))){
        SunLightValues.r = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 0);
        SunLightValues.g = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 1);
        SunLightValues.b = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 2);
        SunLightValues.a = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 3);
    }

    if (GameDataEncoded) {
        WorldData =
            Encode(LoadFile(TextFormat("GameData/Worlds/World%i/World.wdl", OmegaTechData.LevelIndex)), MainKey);
    } else {
        WorldData = L"";
        WorldData = LoadFile(TextFormat("GameData/Worlds/World%i/World.wdl", OmegaTechData.LevelIndex));

#ifdef USESIG

        if (VerifyData(WstringToString(WorldData), Signatures[OmegaTechData.LevelIndex]) == false) {
            exit(0);
        } else {
            cout << "SigCheck: Signatures Match \n\n";
        }

#endif

        OtherWDLData = L"";
        CacheWDL();
    }

    if (OmegaTechSoundData.MusicFound)
        StopMusicStream(OmegaTechSoundData.BackgroundMusic);

    OmegaTechSoundData.MusicFound = false;

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OmegaTechData.LevelIndex))) {
        OmegaTechSoundData.BackgroundMusic =
            LoadMusicStream(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OmegaTechData.LevelIndex));
        OmegaTechSoundData.MusicFound = true;
        PlayMusicStream(OmegaTechSoundData.BackgroundMusic);
    }

    OriginY = OmegaTechData.MainCamera.position.y;

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Scripts/Launch.ps", OmegaTechData.LevelIndex))) {
        ParasiteScriptInit();
        LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Launch.ps", OmegaTechData.LevelIndex));

        ParasiteRunning = true;

        while (ParasiteRunning) {
            CycleInstruction();
            ParasiteScriptCoreData.LineCounter++;

            if (ParasiteScriptCoreData.LineCounter == ParasiteScriptCoreData.ProgramSize)
                ParasiteRunning = false;
        }
    }

#ifdef UseLumina

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/World.lum", OmegaTechData.LevelIndex))) {
        InitLumina(TextFormat("GameData/Worlds/World%i/", OmegaTechData.LevelIndex));
        UseLumina = true;
    }

#endif

    SaveGame();
    LoadCustom(OmegaTechData.LevelIndex);
}

void OmegaTechInit() {
    ParasiteScriptTFlagWipe();

    GuiLoadStyle();

#ifdef USESIG
    InitSig();
#endif

    OmegaTechData.InitCamera();

    OmegaTechData.Bloom = LoadShader(0, "GameData/Shaders/Bloom.fs");
    OmegaTechData.Lights = LoadShader("GameData/Shaders/Lights/Lighting.vs", "GameData/Shaders/Lights/Fog.fs");

    OmegaTechData.GameLights[MAX_LIGHTS] = {0};

    OmegaTechData.Lights.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(OmegaTechData.Lights, "viewPos");

    float FogDensity = 0.005f;
    int FogDensityLoc = GetShaderLocation(OmegaTechData.Lights, "fogDensity");

    SetShaderValue(OmegaTechData.Lights, FogDensityLoc, &FogDensity, SHADER_UNIFORM_FLOAT);

    if (IsPathFile("GameData/Global/Title/Title.png"))
        OmegaTechData.HomeScreen = LoadTexture("GameData/Global/Title/Title.png");
    if (IsPathFile("GameData/Global/Title/Title.mpg"))
        OmegaTechData.HomeScreenVideo = ray_video_open("GameData/Global/Title/Title.mpg");
    if (IsPathFile("GameData/Global/Title/Title.mp3"))
        OmegaTechData.HomeScreenMusic = LoadMusicStream("GameData/Global/Title/Title.mp3");

    OmegaTechTextSystem.Bar = LoadTexture("GameData/Global/TextBar.png");
    OmegaTechTextSystem.BarFont = LoadFont("GameData/Global/Font.ttf");
    OmegaTechTextSystem.RussianBarFont = LoadFont("GameData/Global/Cyrillic.fnt");
    OmegaTechTextSystem.LatinBarFont = LoadFont("GameData/Global/Latin.fnt");
    OmegaTechTextSystem.JapaneseBarFont = LoadFont("GameData/Global/Hiragana.fnt");

    OmegaTechSoundData.CollisionSound = LoadSound("GameData/Global/Sounds/CollisionSound.mp3");
    OmegaTechSoundData.WalkingSound = LoadSound("GameData/Global/Sounds/WalkingSound.mp3");
    OmegaTechSoundData.ChasingSound = LoadSound("GameData/Global/Sounds/ChasingSound.mp3");
    OmegaTechSoundData.UIClick = LoadSound("GameData/Global/Title/Click.mp3");
    OmegaTechSoundData.Death = LoadSound("GameData/Global/Sounds/Hurt.mp3");

    OmegaTechTextSystem.TextNoise = LoadSound("GameData/Global/Sounds/TalkingNoise.mp3");
    OmegaTechData.Cursor = LoadTexture("GameData/Global/Cursor.png");

    for (int i = 1 ; i <= 5; i ++){
        if (IsPathFile(TextFormat("GameData/Global/FModels/FModel%i.gltf", i))) {
            FastModels[i].ModelData = LoadModel(TextFormat("GameData/Global/FModels/FModel%i.gltf", i));
            FastModels[i].ModelTexture = LoadTexture(TextFormat("GameData/Global/FModels/FModel%iTexture.png", i));
            FastModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = FastModels[i].ModelTexture;
            FastModels[i].ModelData.materials[0].shader = OmegaTechData.Lights;
        }
    }


    Target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    ParasiteTarget = LoadRenderTexture(1280, 720);

    WorldData.reserve(MaxWStringAlloc);
    OtherWDLData.reserve(MaxWStringAlloc);
    ExtraWDLInstructions.reserve(MaxWStringAlloc);

    FinalWDL.reserve(MaxWStringAlloc * 3);

    OTSkybox.InitSkybox();

    PlayMusicStream(OmegaTechData.HomeScreenMusic);
}



void PlayFMV() {
    ray_video_t FMVVideo;
    FMVVideo = ray_video_open("GameData/Global/Movies/Opening.mpg");
    Sound VideoSound = LoadSound("GameData/Global/Movies/Opening.mp3");

    PlaySound(VideoSound);

    while (true && !WindowShouldClose()) {
        BeginTextureMode(Target);
        ClearBackground(BLACK);
        ray_video_update(&FMVVideo, GetFrameTime());
        DrawTexturePro(FMVVideo.texture,
                       (Rectangle){0, 0, FMVVideo.width, FMVVideo.height},
                       (Rectangle){0, 0, float(GetScreenWidth()), float(GetScreenHeight())},
                       (Vector2){0, 0},
                       0.f,
                       WHITE);
        EndTextureMode();
        BeginDrawing();

        DrawTexturePro(Target.texture,
                       (Rectangle){0, 0, Target.texture.width, -Target.texture.height},
                       (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                       (Vector2){0, 0},
                       0.f,
                       WHITE);

        EndDrawing();

        if (FMVVideo.video_state == RAY_VIDEO_STATE_DONE || (IsKeyPressed(KEY_SPACE))) {
            UnloadSound(VideoSound);
            UnloadRenderTexture(Target);
            Target = LoadRenderTexture(GetScreenWidth() / 4, GetScreenHeight() / 4);
            ray_video_destroy(&FMVVideo);
            break;
        }
    }
}

bool LDropdownBoxEditMode = false;
int LDropdownBoxActive = 0;

void PlayHomeScreen() {
    Texture2D TitleImage = LoadTexture("GameData/Global/Title/Title.png");

    int MousePoint = 0;

    while (true && !WindowShouldClose()) {
        BeginTextureMode(Target);
        UpdateMusicStream(OmegaTechData.HomeScreenMusic);

        ClearBackground(BLACK);

        if (OmegaInputController.InteractPressed) {
            PlaySound(OmegaTechSoundData.UIClick);
        }

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

            OmegaTechTextSystem.LanguageType = GlobalPackData.Type;

            UnloadMusicStream(OmegaTechData.HomeScreenMusic);
            ray_video_destroy(&OmegaTechData.HomeScreenVideo);

            // PlayFMV();
            
            UnloadRenderTexture(Target);
            Target = LoadRenderTexture(GetScreenWidth() , GetScreenHeight() );


            break;
        }

        if (GuiButton((Rectangle){144, 288, 176, 48}, "Load")) {
            UnloadRenderTexture(Target);
            Target = LoadRenderTexture(GetScreenWidth() / 4, GetScreenHeight() / 4);

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

            OmegaTechTextSystem.LanguageType = GlobalPackData.Type;

            UnloadMusicStream(OmegaTechData.HomeScreenMusic);
            ray_video_destroy(&OmegaTechData.HomeScreenVideo);

            if (IsPathFile("GameData/Saves/TF.sav")) {
                LoadSave();
                LoadFlag = true;
            }
            break;
        }

        EndTextureMode();
        BeginDrawing();

        DrawTexturePro(Target.texture,
                       (Rectangle){0, 0, Target.texture.width, -Target.texture.height},
                       (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                       (Vector2){0, 0},
                       0.f,
                       WHITE);

        if (IsGamepadAvailable(0)) {
            if (MousePoint == 0) {
                DrawTextureEx(OmegaTechData.Cursor, {168 + 176 + 5, 256 - 48}, 0.0f, 3, WHITE);

                if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) >= 0.25f) {
                    MousePoint = 1;
                }
            } else {
                DrawTextureEx(OmegaTechData.Cursor, {144 + 176 + 5, 288}, 0.0f, 3, WHITE);

                if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) <= 0.25f) {
                    MousePoint = 0;
                }
            }
        }

        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE))
            exit(0);
    }

    StopMusicStream(OmegaTechData.HomeScreenMusic);
}

static int ScriptTimer = 0;
static float X, Y, Z, S, Rotation, W, H, L;
bool NextCollision = false;
bool NextAlwaysRender = false;

void CacheWDL() { // Save Model Info for faster Access
                  // Less String operations
    wstring WData = WorldData;

    OtherWDLData = L"";

    CachedModelCounter = 0;
    CachedCollisionCounter = 0;

    bool NextCollision = false;

    for (int i = 0; i <= MaxCachedModels - 1; i++) {
        CachedModels[i].Init();
        CachedCollision[i].Init();
    }

    for (int i = 0; i <= GetWDLSize(WorldData, L""); i++) {
        if (CachedModelCounter == MaxCachedModels)
            break;

        wstring Instruction = WSplitValue(WData, i);

        if (WReadValue(Instruction, 0, 4) == L"Model" || WReadValue(Instruction, 0, 8) == L"HeightMap") {
            if (WReadValue(Instruction, 0, 8) != L"HeightMap") {
                CachedModels[CachedModelCounter].ModelId = int(ToFloat(WReadValue(Instruction, 5, 6)));
            } else {
                CachedModels[CachedModelCounter].ModelId = -1;
            }

            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));

            if (NextCollision) {
                CachedModels[CachedModelCounter].Collision = true;
                NextCollision = false;
            }
            if (NextAlwaysRender) {
                CachedModels[CachedModelCounter].AlwaysRender = true;
                NextAlwaysRender = false;
            }

            CachedModelCounter++;
        }

        if (WReadValue(Instruction, 0, 8) == L"Collision") {
            CachedModels[CachedModelCounter].ModelId = -2;
            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));
        }

        if (WReadValue(Instruction, 0, 11) == L"AdvCollision") {
            CachedCollision[CachedCollisionCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedCollision[CachedCollisionCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedCollision[CachedCollisionCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedCollision[CachedCollisionCounter].W = ToFloat(WSplitValue(WData, i + 6));
            CachedCollision[CachedCollisionCounter].H = ToFloat(WSplitValue(WData, i + 7));
            CachedCollision[CachedCollisionCounter].L = ToFloat(WSplitValue(WData, i + 8));
            CachedCollisionCounter++;
        }

        if (WReadValue(Instruction, 0, 5) == L"Script") {
            CachedModels[CachedModelCounter].ModelId = 200 + int(ToFloat(WReadValue(Instruction, 6, 6)));
            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));
            CachedModelCounter++;
        }

        if (WReadValue(Instruction, 0, 5) == L"Object" || WReadValue(Instruction, 0, 6) == L"ClipBox" ||
            WReadValue(Instruction, 0, 1) == L"NE") {
            OtherWDLData += WSplitValue(WData, i) + L":" + WSplitValue(WData, i + 1) + L":" +
                            WSplitValue(WData, i + 2) + L":" + WSplitValue(WData, i + 3) + L":" +
                            WSplitValue(WData, i + 4) + L":" + WSplitValue(WData, i + 5) + L":";
        }

        if (Instruction == L"C") {
            NextCollision = true;
        }
        if (Instruction == L"PERM") {
            NextAlwaysRender = true;
        }
    }
}

void CWDLProcess() {
    for (int i = 0; i <= CachedCollisionCounter; i++) {
        X = CachedCollision[i].X;
        Y = CachedCollision[i].Y;
        Z = CachedCollision[i].Z;
        W = CachedCollision[i].W;
        H = CachedCollision[i].H;
        L = CachedCollision[i].L;

        if (CheckCollisionBoxSphere((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}},
                                    {OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,
                                     OmegaTechData.MainCamera.position.y - OmegaPlayer.Height / 2,
                                     OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},
                                    1.0)) {
            ObjectCollision = true;
            if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound)) {
                PlaySound(OmegaTechSoundData.CollisionSound);
            }
        }
    }

    OTSkybox.Draw();

    for (int i = 0; i <= CachedModelCounter; i++) {
        X = CachedModels[i].X;
        Y = CachedModels[i].Y;
        Z = CachedModels[i].Z;
        S = CachedModels[i].S;
        Rotation = CachedModels[i].R;

        bool LOD = true;

        if (OmegaTechData.MainCamera.position.z - OmegaTechData.LODSwapRadius < Z &&
                OmegaTechData.MainCamera.position.z + OmegaTechData.LODSwapRadius > Z ||
            CachedModels[i].ModelId == -1) {
            if (OmegaTechData.MainCamera.position.x - OmegaTechData.LODSwapRadius < X &&
                    OmegaTechData.MainCamera.position.x + OmegaTechData.LODSwapRadius > X ||
                CachedModels[i].ModelId == -1) {
                LOD = false;
            }
        }

        if (CachedModels[i].ModelId >= 1 && CachedModels[i].ModelId <= 20){
            if (LOD && LowDetail[i]){
                DrawModelEx(WLowDetailModels[CachedModels[i].ModelId].ModelData, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            else {
                DrawModelEx(WDLModels[CachedModels[i].ModelId].ModelData, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
        }
        else {
            switch (CachedModels[i].ModelId) {
                case -2:
                    if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds,
                                            (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                        ObjectCollision = true;
                        if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound)) {
                            PlaySound(OmegaTechSoundData.CollisionSound);
                        }
                    }
                    break;
                case -1:
                    DrawModelEx(TerrainData.HeightMap, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
            }
        }

        if (CachedModels[i].ModelId >= 200) {
            if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds,
                                    (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                if (OmegaInputController.InteractPressed) {
                    ObjectCollision = true;
                    if (ScriptTimer == 0) {
                        ParasiteScriptInit();
                        LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Script%i.ps",
                                              OmegaTechData.LevelIndex,
                                              CachedModels[i].ModelId - 200));

                        ParasiteRunning = true;

                        while (ParasiteRunning) {
                            CycleInstruction();
                            ParasiteScriptCoreData.LineCounter++;
                        }

                        ScriptTimer = 180;
                    }
                } else {
                    if (OmegaTechTextSystem.LanguageType == 0) {
                        ScriptCollisionMessage = true;
                    }
                }
            }
        }
        if (CachedModels[i].Collision) {
            BoundingBox ModelBox = CalculateBoundingBox(X, Y, Z, S * 2);
            if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds, ModelBox)) {
                ObjectCollision = true;
            }
        }
    }
}

void WDLProcess() {
    int Size = 0;

    FinalWDL = L"";

    if (OmegaTechData.UseCachedRenderer) {
        FinalWDL = OtherWDLData + ExtraWDLInstructions;
        Size = GetWDLSize(OtherWDLData, ExtraWDLInstructions);
    } else {
        FinalWDL = WorldData + ExtraWDLInstructions;
        Size = GetWDLSize(WorldData, ExtraWDLInstructions);
    }

    bool Render = false;
    bool FoundPlatform = false;
    float PlatformHeight = 0.0f;

    if (FinalWDL == L"") {
        return;
    }

    for (int i = 0; i <= Size; i++) {
        wstring Instruction = WSplitValue(FinalWDL, i);

        if (Instruction == L"C") {
            NextCollision = true;
        }

        if (WReadValue(Instruction, 0, 4) == L"Model" || WReadValue(Instruction, 0, 1) == L"NE" ||
            WReadValue(Instruction, 0, 6) == L"ClipBox" || WReadValue(Instruction, 0, 5) == L"Object" ||
            WReadValue(Instruction, 0, 5) == L"Script" || WReadValue(Instruction, 0, 8) == L"HeightMap" ||
            WReadValue(Instruction, 0, 8) == L"Collision" || WReadValue(Instruction, 0, 11) == L"AdvCollision") {
            X = ToFloat(WSplitValue(FinalWDL, i + 1));
            Y = ToFloat(WSplitValue(FinalWDL, i + 2));
            Z = ToFloat(WSplitValue(FinalWDL, i + 3));
            S = ToFloat(WSplitValue(FinalWDL, i + 4));

            Rotation = ToFloat(WSplitValue(FinalWDL, i + 5));

            if (OmegaTechData.MainCamera.position.z - OmegaTechData.PopInRadius < Z &&
                OmegaTechData.MainCamera.position.z + OmegaTechData.PopInRadius > Z) {
                if (OmegaTechData.MainCamera.position.x - OmegaTechData.PopInRadius < X &&
                    OmegaTechData.MainCamera.position.x + OmegaTechData.PopInRadius > X) {
                    Render = true;

                    if (Instruction == L"NE1") {
                        if (!IsMusicStreamPlaying(OmegaTechSoundData.NESound1))
                            PlayMusicStream(OmegaTechSoundData.NESound1);
                    }
                    if (Instruction == L"NE2") {
                        if (!IsMusicStreamPlaying(OmegaTechSoundData.NESound2))
                            PlayMusicStream(OmegaTechSoundData.NESound2);
                    }
                    if (Instruction == L"NE3") {
                        if (!IsMusicStreamPlaying(OmegaTechSoundData.NESound3))
                            PlayMusicStream(OmegaTechSoundData.NESound3);
                    }
                }
            }
        } else {
            if (Instruction == L"NE1") {
                StopMusicStream(OmegaTechSoundData.NESound1);
            }
            if (Instruction == L"NE2") {
                StopMusicStream(OmegaTechSoundData.NESound2);
            }
            if (Instruction == L"NE3") {
                StopMusicStream(OmegaTechSoundData.NESound3);
            }
        }

        if (Render) {
            int AudioValue = 0;

            if (Instruction == L"NE1") {
                AudioValue = FlipNumber(
                    GetDistance(X, Z, OmegaTechData.MainCamera.position.x, OmegaTechData.MainCamera.position.z));
                if (AudioValue > 0 && AudioValue < 100)
                    SetMusicVolume(OmegaTechSoundData.NESound1, float(AudioValue) / 100.0f);
                else {
                    SetMusicVolume(OmegaTechSoundData.NESound1, 0);
                }
            }
            if (Instruction == L"NE2") {
                AudioValue = FlipNumber(
                    GetDistance(X, Z, OmegaTechData.MainCamera.position.x, OmegaTechData.MainCamera.position.z));
                if (AudioValue > 0 && AudioValue < 100)
                    SetMusicVolume(OmegaTechSoundData.NESound2, float(AudioValue) / 100.0f);
                else {
                    SetMusicVolume(OmegaTechSoundData.NESound2, 0);
                }
            }
            if (Instruction == L"NE3") {
                AudioValue = FlipNumber(
                    GetDistance(X, Z, OmegaTechData.MainCamera.position.x, OmegaTechData.MainCamera.position.z));
                if (AudioValue > 0 && AudioValue < 100)
                    SetMusicVolume(OmegaTechSoundData.NESound3, float(AudioValue) / 100.0f);
                else {
                    SetMusicVolume(OmegaTechSoundData.NESound3, 0);
                }
            }

            if (Instruction == L"Collision") {
                if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds,
                                        (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                    ObjectCollision = true;
                }

                if (Debug) {
                    if (ObjectCollision) {
                        DrawCubeWires({X, Y, Z}, S, S, S, GREEN);
                    } else {
                        DrawCubeWires({X, Y, Z}, S, S, S, RED);
                    }
                }
                if (ObjectCollision) {
                    if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound)) {
                        PlaySound(OmegaTechSoundData.CollisionSound);
                    }
                }
            }

            if (WReadValue(Instruction, 0, 5) == L"Script") {
                if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds,
                                        (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                    if (OmegaInputController.InteractPressed) {
                        ObjectCollision = true;
                        if (ScriptTimer == 0) {
                            ParasiteScriptInit();
                            LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Script%i.ps",
                                                  OmegaTechData.LevelIndex,
                                                  int(ToFloat(WReadValue(Instruction, 6, Instruction.size() - 1)))));

                            for (int x = 0; x <= ParasiteScriptCoreData.ProgramSize; x++) {
                                CycleInstruction();
                                ParasiteScriptCoreData.LineCounter++;
                            }

                            ScriptTimer = 180;
                        }
                    } else {
                        if (OmegaTechTextSystem.LanguageType == 0) {
                            ScriptCollisionMessage = true;
                        } else {
                        }
                    }
                }

                if (Debug) {
                    if (ObjectCollision) {
                        DrawCubeWires({X, Y, Z}, S, S, S, GREEN);
                    } else {
                        DrawCubeWires({X, Y, Z}, S, S, S, YELLOW);
                    }
                }
            }
        }

        if (Instruction == L"ClipBox") {
            W = ToFloat(WSplitValue(FinalWDL, i + 6));
            H = ToFloat(WSplitValue(FinalWDL, i + 7));
            L = ToFloat(WSplitValue(FinalWDL, i + 8));

            if (CheckCollisionBoxSphere((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}},
                                        {OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,
                                         OmegaTechData.MainCamera.position.y - 1,
                                         OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},
                                        1.0)) {
                PlatformHeight = H;
                FoundPlatform = true;
            }

            if (FoundPlatform)
                DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H - 5, L}}, PURPLE);
            else {
                DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H - 5, L}}, RED);
            }

            i += 3;
        }
        if (Instruction == L"AdvCollision") { // Collision
            if (Render) {
                W = ToFloat(WSplitValue(FinalWDL, i + 6));
                H = ToFloat(WSplitValue(FinalWDL, i + 7));
                L = ToFloat(WSplitValue(FinalWDL, i + 8));

                if (CheckCollisionBoxSphere((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}},
                                            {OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,
                                             OmegaTechData.MainCamera.position.y - OmegaPlayer.Height / 2,
                                             OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},
                                            1.0))
                    ObjectCollision = true;

                if (Debug) {
                    if (ObjectCollision) {
                        DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}}, GREEN);
                    } else {
                        DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}}, PURPLE);
                    }
                }

                if (ObjectCollision) {
                    if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound)) {
                        PlaySound(OmegaTechSoundData.CollisionSound);
                    }
                }
            }

            i += 3;
        }

        if (!NextCollision) {
            i += 5;
        }

        Render = false;
    }

    DrawCube({OmegaTechData.MainCamera.position.x,
              OmegaTechData.MainCamera.position.y - OmegaPlayer.Height,
              OmegaTechData.MainCamera.position.z},
             1,
             0.1,
             1,
             RED);

    if (FoundPlatform) {
        OmegaTechData.MainCamera.position.y = (PlatformHeight);
        UsingClipBox = true;
    } else {
        if (UsingClipBox)
            OmegaTechData.MainCamera.position.y = OriginY;
        UsingClipBox = false;
    }
}

void UpdatePlayer() {
    if (IsKeyDown(KEY_W) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 && !Debug) {
        if (HeadBob) {
            if (OmegaTechData.Ticker % 2 == 0) {
                OmegaTechData.MainCamera.target.y += OmegaPlayer.HeadBob;
                if (OmegaPlayer.HeadBobDirection == 1) {
                    if (OmegaPlayer.HeadBob != 1) {
                        OmegaPlayer.HeadBob++;
                    } else {
                        OmegaPlayer.HeadBobDirection = 0;
                    }
                } else {
                    if (OmegaPlayer.HeadBob != -1) {
                        OmegaPlayer.HeadBob--;
                    } else {
                        OmegaPlayer.HeadBobDirection = 1;
                    }
                }
            }
        }
        if (!IsSoundPlaying(OmegaTechSoundData.WalkingSound)) {
            PlaySound(OmegaTechSoundData.WalkingSound);
        }
    }

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
        GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) != 0) {
        if (!IsSoundPlaying(OmegaTechSoundData.WalkingSound)) {
            PlaySound(OmegaTechSoundData.WalkingSound);
        }
    } else {
        if (IsSoundPlaying(OmegaTechSoundData.WalkingSound)) {
            StopSound(OmegaTechSoundData.WalkingSound);
        }
    }

    OmegaPlayer.PlayerBounds = (BoundingBox){(Vector3){OmegaTechData.MainCamera.position.x - OmegaPlayer.Width / 2,
                                                       OmegaTechData.MainCamera.position.y - OmegaPlayer.Height,
                                                       OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},
                                             (Vector3){OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,
                                                       OmegaTechData.MainCamera.position.y,
                                                       OmegaTechData.MainCamera.position.z + OmegaPlayer.Width / 2}};

    if (MapClipping && !UsingClipBox) {
        if (OmegaTechData.MainCamera.position.z >= 0 && OmegaTechData.MainCamera.position.z <= TerrainData.HeightMapW) {
            if (OmegaTechData.MainCamera.position.x >= 0 &&
                OmegaTechData.MainCamera.position.x <= TerrainData.HeightMapW) {
                if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
                    GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 ||
                    GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) != 0) {
                    OmegaTechData.MainCamera.position.y = 8 +
                                                          TerrainHeightMap[int(OmegaTechData.MainCamera.position.z)]
                                                                          [int(OmegaTechData.MainCamera.position.x)];
                }
            }
        }
    }
}

void UpdateNoiseEmitters() {
    UpdateMusicStream(OmegaTechSoundData.NESound1);
    UpdateMusicStream(OmegaTechSoundData.NESound2);
    UpdateMusicStream(OmegaTechSoundData.NESound3);
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
        to_wstring(OmegaTechData.MainCamera.position.x) + L':' + to_wstring(OmegaTechData.MainCamera.position.y) +
        L':' + to_wstring(OmegaTechData.MainCamera.position.z) + L':' + to_wstring(OmegaTechData.LevelIndex) + L':';

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

    OmegaTechData.LevelIndex = int(ToFloat(WSplitValue(Position, 3)));

    SetCameraFlag = true;

    int X = ToFloat(WSplitValue(Position, 0));
    int Y = ToFloat(WSplitValue(Position, 1));
    int Z = ToFloat(WSplitValue(Position, 2));
    SetCameraPos = {float(X), float(Y), float(Z)};

    ExtraWDLInstructions = LoadFile("GameData/Saves/Script.sav");

    RestorePMemPage();
}

void DrawWorld() {
    BeginTextureMode(Target);
    ClearBackground(BLACK);

    if (!UsingCineFlow) {
        BeginMode3D(OmegaTechData.MainCamera);
    } else {
        BeginMode3D(CineFlowData.CFCamera);
    }

    if (OmegaTechSoundData.MusicFound) {
        UpdateMusicStream(OmegaTechSoundData.BackgroundMusic);
    }

    UpdateNoiseEmitters();

    if (!OmegaTechData.UseCachedRenderer) {
        WDLProcess();
    } else {
        CWDLProcess();
        WDLProcess();
    }

    if (!UsingCineFlow) {
        UpdatePlayer();
    }

    if (Debug) {
        DrawLights();
    }

    if (ObjectCollision) {
        OmegaTechData.MainCamera.position.x = OmegaPlayer.OldX;
        OmegaTechData.MainCamera.position.y = OmegaPlayer.OldY;
        OmegaTechData.MainCamera.position.z = OmegaPlayer.OldZ;
        ObjectCollision = false;
    }

    UpdateCustom(OmegaTechData.LevelIndex);

    EndMode3D();

    EndTextureMode();

    if (SetSceneFlag) {
        OmegaTechData.LevelIndex = SetSceneId;
        LoadWorld();
        SetSceneFlag = false;
    }

    if (SetCameraFlag) {
        OmegaTechData.MainCamera.position = SetCameraPos;
        SetCameraFlag = false;
    }

    if (ScriptTimer != 0) {
        ScriptTimer--;
    }

    if (OmegaTechData.Ticker != 60) {
        OmegaTechData.Ticker ++;
    } else {
        OmegaTechData.Ticker = 0;
    }
}

void DisplayInteractText() {
    if (!IsGamepadAvailable(0)) {
        DrawTextEx(OmegaTechTextSystem.BarFont,
                   "Press Left Click to Interact",
                   {GetScreenWidth() / 2 -
                        MeasureTextEx(OmegaTechTextSystem.BarFont, "Press Left Click to Interact", 25, 1).x / 2,
                    720 / 2},
                   25,
                   1,
                   WHITE);
    } else {
        DrawTextEx(
            OmegaTechTextSystem.BarFont,
            "Press A to Interact",
            {GetScreenWidth() / 2 - MeasureTextEx(OmegaTechTextSystem.BarFont, "Press A to Interact", 25, 1).x / 2,
             720 / 2},
            25,
            1,
            WHITE);
    }
}