#include "Worlds.hpp"


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

    if (MapClipping && !QMapSystem.UsingQMaps) {
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

    if (QMapSystem.UsingQMaps){
        QMapSystem.RenderQMap();
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