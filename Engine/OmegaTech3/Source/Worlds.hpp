#include "Custom.hpp"


void GenHeights(Image heightmap, Vector3 size);

static int ScriptTimer = 0;
static float X, Y, Z, S, Rotation, W, H, L;
bool NextCollision = false;
bool NextAlwaysRender = false;

void LoadWDL() {
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
                                    {OTCoreData.RenderCamera.position.x + OTPlayer.Width / 2,
                                     OTCoreData.RenderCamera.position.y - OTPlayer.Height / 2,
                                     OTCoreData.RenderCamera.position.z - OTPlayer.Width / 2},
                                    1.0)) {
            OTPlayer.ObjectCollision = true;
            if (!IsSoundPlaying(OTSoundData.CollisionSound)) {
                PlaySound(OTSoundData.CollisionSound);
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

        if (OTCoreData.RenderCamera.position.z - OTCoreData.LODSwapRadius < Z &&
                OTCoreData.RenderCamera.position.z + OTCoreData.LODSwapRadius > Z ||
            CachedModels[i].ModelId == -1) {
            if (OTCoreData.RenderCamera.position.x - OTCoreData.LODSwapRadius < X &&
                    OTCoreData.RenderCamera.position.x + OTCoreData.LODSwapRadius > X ||
                CachedModels[i].ModelId == -1) {
                LOD = false;
            }
        }

        if (CachedModels[i].ModelId >= 1 && CachedModels[i].ModelId <= 20){
            if (LOD && LowDetail[i]){
                DrawModelEx(LowDetalMapModels[CachedModels[i].ModelId].ModelData, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            else {
                DrawModelEx(MapModels[CachedModels[i].ModelId].ModelData, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
        }
        else {
            switch (CachedModels[i].ModelId) {
                case -2:
                    if (CheckCollisionBoxes(OTPlayer.PlayerBounds,
                                            (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                        OTPlayer.ObjectCollision = true;
                        if (!IsSoundPlaying(OTSoundData.CollisionSound)) {
                            PlaySound(OTSoundData.CollisionSound);
                        }
                    }
                    break;
                case -1:
                    FDrawModelEx(OTCoreData.RenderCamera, TerrainData.HeightMap, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
            }
        }

        if (CachedModels[i].ModelId >= 200) {
            if (CheckCollisionBoxes(OTPlayer.PlayerBounds,
                                    (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                if (OTInputController.InteractPressed) {
                    OTPlayer.ObjectCollision = true;
                    if (ScriptTimer == 0) {
                        ParasiteScriptInit();
                        LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Script%i.ps",
                                              OTCoreData.LevelIndex,
                                              CachedModels[i].ModelId - 200));

                        ParasiteRunning = true;

                        while (ParasiteRunning) {
                            CycleInstruction();
                            ParasiteScriptCoreData.LineCounter++;
                        }

                        ScriptTimer = 180;
                    }
                }
            }
        }
        if (CachedModels[i].Collision) {
            BoundingBox ModelBox = CalculateBoundingBox(X, Y, Z, S * 2);
            if (CheckCollisionBoxes(OTPlayer.PlayerBounds, ModelBox)) {
                OTPlayer.ObjectCollision = true;
            }
        }
    }
}

void WDLProcess() {
    int Size = 0;

    FinalWDL = L"";
    FinalWDL = OtherWDLData + ExtraWDLInstructions;
    Size = GetWDLSize(OtherWDLData, ExtraWDLInstructions);

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

            if (OTCoreData.RenderCamera.position.z - OTCoreData.PopInRadius < Z &&
                OTCoreData.RenderCamera.position.z + OTCoreData.PopInRadius > Z) {
                if (OTCoreData.RenderCamera.position.x - OTCoreData.PopInRadius < X &&
                    OTCoreData.RenderCamera.position.x + OTCoreData.PopInRadius > X) {
                    Render = true;

                    if (Instruction == L"NE1") {
                        if (!IsMusicStreamPlaying(OTSoundData.NESound1))
                            PlayMusicStream(OTSoundData.NESound1);
                    }
                    if (Instruction == L"NE2") {
                        if (!IsMusicStreamPlaying(OTSoundData.NESound2))
                            PlayMusicStream(OTSoundData.NESound2);
                    }
                    if (Instruction == L"NE3") {
                        if (!IsMusicStreamPlaying(OTSoundData.NESound3))
                            PlayMusicStream(OTSoundData.NESound3);
                    }
                }
            }
        } else {
            if (Instruction == L"NE1") {
                StopMusicStream(OTSoundData.NESound1);
            }
            if (Instruction == L"NE2") {
                StopMusicStream(OTSoundData.NESound2);
            }
            if (Instruction == L"NE3") {
                StopMusicStream(OTSoundData.NESound3);
            }
        }

        if (Render) {
            int AudioValue = 0;

            if (Instruction == L"NE1") {
                AudioValue = FlipNumber(
                    GetDistance(X, Z, OTCoreData.RenderCamera.position.x, OTCoreData.RenderCamera.position.z));
                if (AudioValue > 0 && AudioValue < 100)
                    SetMusicVolume(OTSoundData.NESound1, float(AudioValue) / 100.0f);
                else {
                    SetMusicVolume(OTSoundData.NESound1, 0);
                }
            }
            if (Instruction == L"NE2") {
                AudioValue = FlipNumber(
                    GetDistance(X, Z, OTCoreData.RenderCamera.position.x, OTCoreData.RenderCamera.position.z));
                if (AudioValue > 0 && AudioValue < 100)
                    SetMusicVolume(OTSoundData.NESound2, float(AudioValue) / 100.0f);
                else {
                    SetMusicVolume(OTSoundData.NESound2, 0);
                }
            }
            if (Instruction == L"NE3") {
                AudioValue = FlipNumber(
                    GetDistance(X, Z, OTCoreData.RenderCamera.position.x, OTCoreData.RenderCamera.position.z));
                if (AudioValue > 0 && AudioValue < 100)
                    SetMusicVolume(OTSoundData.NESound3, float(AudioValue) / 100.0f);
                else {
                    SetMusicVolume(OTSoundData.NESound3, 0);
                }
            }

            if (Instruction == L"Collision") {
                if (CheckCollisionBoxes(OTPlayer.PlayerBounds,
                                        (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                    OTPlayer.ObjectCollision = true;
                }

                if (Debug) {
                    if (OTPlayer.ObjectCollision) {
                        DrawCubeWires({X, Y, Z}, S, S, S, GREEN);
                    } else {
                        DrawCubeWires({X, Y, Z}, S, S, S, RED);
                    }
                }
                
                if (OTPlayer.ObjectCollision) {
                    if (!IsSoundPlaying(OTSoundData.CollisionSound)) {
                        PlaySound(OTSoundData.CollisionSound);
                    }
                }
            }

            if (WReadValue(Instruction, 0, 5) == L"Script") {
                if (CheckCollisionBoxes(OTPlayer.PlayerBounds,
                                        (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}})) {
                    if (OTInputController.InteractPressed) {
                        OTPlayer.ObjectCollision = true;
                        if (ScriptTimer == 0) {
                            ParasiteScriptInit();
                            LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Script%i.ps",
                                                  OTCoreData.LevelIndex,
                                                  int(ToFloat(WReadValue(Instruction, 6, Instruction.size() - 1)))));

                            for (int x = 0; x <= ParasiteScriptCoreData.ProgramSize; x++) {
                                CycleInstruction();
                                ParasiteScriptCoreData.LineCounter++;
                            }

                            ScriptTimer = 180;
                        }
                    }
                }

                if (Debug) {
                    if (OTPlayer.ObjectCollision) {
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
                                        {OTCoreData.RenderCamera.position.x + OTPlayer.Width / 2,
                                         OTCoreData.RenderCamera.position.y - 1,
                                         OTCoreData.RenderCamera.position.z - OTPlayer.Width / 2},
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
                                            {OTCoreData.RenderCamera.position.x + OTPlayer.Width / 2,
                                             OTCoreData.RenderCamera.position.y - OTPlayer.Height / 2,
                                             OTCoreData.RenderCamera.position.z - OTPlayer.Width / 2},
                                            1.0))
                    OTPlayer.ObjectCollision = true;

                if (Debug) {
                    if (OTPlayer.ObjectCollision) {
                        DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}}, GREEN);
                    } else {
                        DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}}, PURPLE);
                    }
                }

                if (OTPlayer.ObjectCollision) {
                    if (!IsSoundPlaying(OTSoundData.CollisionSound)) {
                        PlaySound(OTSoundData.CollisionSound);
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
}

void UpdateNoiseEmitters() {
    if ( IsMusicReady(OTSoundData.NESound1) ) UpdateMusicStream(OTSoundData.NESound1);
    if ( IsMusicReady(OTSoundData.NESound2) )UpdateMusicStream(OTSoundData.NESound2);
    if ( IsMusicReady(OTSoundData.NESound3) )UpdateMusicStream(OTSoundData.NESound3);
}


void LoadNoiseEmitters();
void LoadAmbientLight();
void LoadSkybox();
void LoadHeightMap();
void LoadModels();
void LoadQMap();
void LoadSunLight();
void LoadWorldData();
void LoadBackgroundMusic();
void LoadScripts();

#ifdef UseGBEngine
void LoadGBEngine();
#endif

#ifdef UseLumina
void LoadLumina();
#endif

void LoadWorld() {

    PlayFadeIn();
    ClearLights();

    SceneIDMirror = OTCoreData.LevelIndex;

    #ifdef UseGBEngine
        LoadGBEngine();
    #endif

    #ifdef UseLumina
        LoadLumina();
    #endif
    
    
    LoadWorldData();
    LoadHeightMap();
    LoadModels();
    LoadQMap();
    LoadBackgroundMusic();
    
    LoadNoiseEmitters();

    LoadSkybox();
    LoadSunLight();
    LoadAmbientLight();
    LoadScripts();

    LoadCustom(OTCoreData.LevelIndex);

}

#ifdef UseGBEngine

void LoadGBEngine() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Scene.gb", OmegaTechData.LevelIndex))) {
        InitGB(convertToChar(TextFormat("GameData/Worlds/World%i/Scene.gb", OmegaTechData.LevelIndex)));
        RunGB();
        OmegaTechData.LevelIndex++;
    }
}

#endif


void LoadNoiseEmitters() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OTCoreData.LevelIndex))) {
        if (IsMusicStreamPlaying(OTSoundData.NESound1))StopMusicStream(OTSoundData.NESound1);
        OTSoundData.NESound1 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OTCoreData.LevelIndex));
    } else {
        if (IsMusicReady(OTSoundData.NESound1))UnloadMusicStream(OTSoundData.NESound1);
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OTCoreData.LevelIndex))) {
        if (IsMusicStreamPlaying(OTSoundData.NESound2))StopMusicStream(OTSoundData.NESound2);
        OTSoundData.NESound2 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OTCoreData.LevelIndex));
    } else {
        if (IsMusicReady(OTSoundData.NESound2))UnloadMusicStream(OTSoundData.NESound2);
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OTCoreData.LevelIndex))) {
        if (IsMusicStreamPlaying(OTSoundData.NESound2))StopMusicStream(OTSoundData.NESound2);
        OTSoundData.NESound2 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OTCoreData.LevelIndex));
    } else {
        if (IsMusicReady(OTSoundData.NESound2))UnloadMusicStream(OTSoundData.NESound2);
    }
}

void LoadAmbientLight() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OTCoreData.LevelIndex))) {
        float Red = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OTCoreData.LevelIndex), 0)) / 255));
        float Green = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OTCoreData.LevelIndex), 1)) / 255));
        float Blue = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OTCoreData.LevelIndex), 2)) / 255));
        float Alpha = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OTCoreData.LevelIndex), 3)) / 255));

        AmbientLightValues[0] = Red;
        AmbientLightValues[1] = Green;
        AmbientLightValues[2] = Blue;
        AmbientLightValues[3] = Alpha;
    }
}

void LoadSkybox() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OTCoreData.LevelIndex))) {
        OTSkybox.LoadSkybox(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OTCoreData.LevelIndex));
    }
}

void LoadHeightMap() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OTCoreData.LevelIndex))) {
        TerrainData.HeightMapW =
            PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/HeightMap.conf", OTCoreData.LevelIndex), 0);
        TerrainData.HeightMapH =
            PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/HeightMap.conf", OTCoreData.LevelIndex), 1);

        TerrainData.HeightMapTexture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/HeightMapTexture.png", OTCoreData.LevelIndex));
        Image HeightMapImage =
            LoadImage(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OTCoreData.LevelIndex));
        Texture2D Texture = LoadTextureFromImage(HeightMapImage);

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Terrain.obj", OTCoreData.LevelIndex))) {
            TerrainData.HeightMap =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Terrain.obj", OTCoreData.LevelIndex));
            TerrainData.HeightMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = TerrainData.HeightMapTexture;
            TerrainData.HeightMap.materials[0].shader = OTCoreData.Lights;
        } else {
            Mesh Mesh1 = GenMeshHeightmap(HeightMapImage,
                                          (Vector3){TerrainData.HeightMapW, TerrainData.HeightMapH, TerrainData.HeightMapW});

            TerrainData.HeightMap = LoadModelFromMesh(Mesh1);
            TerrainData.HeightMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = TerrainData.HeightMapTexture;
            TerrainData.HeightMap.meshes[0] = Mesh1;
            TerrainData.HeightMap.materials[0].shader = OTCoreData.Lights;
        }

        GenHeights(HeightMapImage, {(Vector3){TerrainData.HeightMapW, TerrainData.HeightMapH, TerrainData.HeightMapW}});
    }
}

void LoadModels() {
    for (int i = 1; i <= ModelCount - 1; i++) {
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%i.obj", OTCoreData.LevelIndex, i))) {
            MapModels[i].ModelData =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%i.obj", OTCoreData.LevelIndex, i));
            MapModels[i].ModelTexture =
                LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model%iTexture.png", OTCoreData.LevelIndex, i));
            SetTextureFilter(MapModels[i].ModelTexture, TEXTURE_FILTER_TRILINEAR);
            MapModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = MapModels[i].ModelTexture;
            MapModels[i].ModelData.materials[0].shader = OTCoreData.Lights;

            if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.obj", OTCoreData.LevelIndex, i))) {
                LowDetalMapModels[i].ModelData =
                    LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.obj", OTCoreData.LevelIndex, i));
                LowDetalMapModels[i].ModelTexture =
                    LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model%iTexture.png", OTCoreData.LevelIndex, i));
                SetTextureFilter(LowDetalMapModels[i].ModelTexture, TEXTURE_FILTER_TRILINEAR);
                LowDetalMapModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LowDetalMapModels[i].ModelTexture;
                LowDetalMapModels[i].ModelData.materials[0].shader = OTCoreData.Lights;
                LowDetail[i] = true;
            } else {
                LowDetail[i] = false;
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%i.glb", OTCoreData.LevelIndex, i))) { // glb support
            MapModels[i].ModelData = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%i.glb", OTCoreData.LevelIndex, i));

            for (int x = 0 ; x <= MapModels[i].ModelData.materialCount - 1; x ++)MapModels[i].ModelData.materials[x].shader = OTCoreData.Lights;

            if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.glb", OTCoreData.LevelIndex, i))) {
                LowDetalMapModels[i].ModelData = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.glb", OTCoreData.LevelIndex, i));
                LowDetalMapModels[i].ModelData.materials[0].shader = OTCoreData.Lights;
                LowDetail[i] = true;
            } else {
                LowDetail[i] = false;
            }
        }

    }
}

void LoadQMap() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/World.qmap", OTCoreData.LevelIndex))) {
        QMapSystem.LoadQMap(TextFormat("GameData/Worlds/World%i/World.qmap", OTCoreData.LevelIndex));

        QMapSystem.EmptyTexture = LoadTextureFromImage(GenImagePerlinNoise(100, 100, 0, 0, 10.0f));

        for (int i = 0; i <= MaxQMapTextures; i++) {
            if (IsPathFile(TextFormat("GameData/Worlds/World%i/QMap/Q%i.png", OTCoreData.LevelIndex, i))) {
                SetTextureWrap(QMapSystem.MapTextures[i], TEXTURE_WRAP_MIRROR_REPEAT);
                QMapSystem.MapTextures[i] = LoadTexture(TextFormat("GameData/Worlds/World%i/QMap/Q%i.png", OTCoreData.LevelIndex, i));
            }
        }
    } else {
        QMapSystem.UsingQMaps = false;
    }
}

void LoadSunLight() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OTCoreData.LevelIndex))) {
        SunLightValues.r = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OTCoreData.LevelIndex), 0);
        SunLightValues.g = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OTCoreData.LevelIndex), 1);
        SunLightValues.b = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OTCoreData.LevelIndex), 2);
        SunLightValues.a = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OTCoreData.LevelIndex), 3);
    }
}

void LoadWorldData() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/World.wdl", OTCoreData.LevelIndex))) {
        if (GameDataEncoded) {
            WorldData = Encode(LoadFile(TextFormat("GameData/Worlds/World%i/World.wdl", OTCoreData.LevelIndex)), MainKey);
        } else {
            WorldData = L"";
            WorldData = LoadFile(TextFormat("GameData/Worlds/World%i/World.wdl", OTCoreData.LevelIndex));

#ifdef USESIG

            if (VerifyData(WstringToString(WorldData), Signatures[OmegaTechData.LevelIndex]) == false) {
                exit(0);
            } else {
                cout << "SigCheck: Signatures Match \n\n";
            }

#endif

            OtherWDLData = L"";
            LoadWDL();
        }
    }
}

void LoadBackgroundMusic() {
    if (IsMusicReady(OTSoundData.BackgroundMusic)) StopMusicStream(OTSoundData.BackgroundMusic);

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OTCoreData.LevelIndex))) {
        if (IsMusicReady(OTSoundData.BackgroundMusic)) OTSoundData.BackgroundMusic = LoadMusicStream(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OTCoreData.LevelIndex));
        if (IsMusicReady(OTSoundData.BackgroundMusic)) PlayMusicStream(OTSoundData.BackgroundMusic);
    }
}

void LoadScripts() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Scripts/Launch.ps", OTCoreData.LevelIndex))) {
        ParasiteScriptInit();
        LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Launch.ps", OTCoreData.LevelIndex));

        ParasiteRunning = true;

        while (ParasiteRunning) {
            CycleInstruction();
            ParasiteScriptCoreData.LineCounter++;

            if (ParasiteScriptCoreData.LineCounter == ParasiteScriptCoreData.ProgramSize)
                ParasiteRunning = false;
        }
    }
}

#ifdef UseLumina
void LoadLumina() {

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/World.lum", OmegaTechData.LevelIndex))) {
        InitLumina(TextFormat("GameData/Worlds/World%i/", OmegaTechData.LevelIndex));
        UseLumina = true;
    }

}
#endif


void UnloadModels();
void UnloadTerrain();
void UnloadFastModels();
void UnloadQMapTextures();
void UnloadShadersAndRenderTextures();
void UnloadFonts();
void UnloadSounds();

void UnloadGame() {
    std::vector<std::thread> threads;
    threads.push_back(std::thread(UnloadTerrain));
    threads.push_back(std::thread(UnloadModels));
    threads.push_back(std::thread(UnloadFastModels));
    threads.push_back(std::thread(UnloadQMapTextures));
    threads.push_back(std::thread(UnloadShadersAndRenderTextures));
    threads.push_back(std::thread(UnloadFonts));
    threads.push_back(std::thread(UnloadSounds));

    // Wait for all threads to finish
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void UnloadTerrain() {
    if (IsModelLoaded(&TerrainData.HeightMap)) {
        UnloadModel(TerrainData.HeightMap);
    }

    if (TerrainData.HeightMapTexture.width != NULL) {
        UnloadTexture(TerrainData.HeightMapTexture);
    }
}

void UnloadModels() {
    for (int i = 1; i <= ModelCount - 1; i++) {
        if (IsModelLoaded(&MapModels[i].ModelData)) {
            UnloadModel(MapModels[i].ModelData);
        }

        if (MapModels[i].ModelTexture.width != NULL) {
            UnloadTexture(MapModels[i].ModelTexture);
        }

        if (LowDetail[i]) {
            if (IsModelLoaded(&LowDetalMapModels[i].ModelData)) {
                UnloadModel(LowDetalMapModels[i].ModelData);
            }

            if (LowDetalMapModels[i].ModelTexture.width != NULL) {
                UnloadTexture(LowDetalMapModels[i].ModelTexture);
            }
        }
    }
}

void UnloadFastModels() {
    for (int i = 0; i <= 5; i++) {
        if (IsModelLoaded(&FastModels[i].ModelData)) {
            UnloadModel(FastModels[i].ModelData);
        }

        if (FastModels[i].ModelTexture.width != NULL) {
            UnloadTexture(FastModels[i].ModelTexture);
        }
    }
}

void UnloadQMapTextures() {
    for (int i = 0; i <= MaxQMapTextures - 1; i++) {
        if (IsTextureReady(QMapSystem.MapTextures[i])) {
            UnloadTexture(QMapSystem.MapTextures[i]);
        }
    }
}

void UnloadShadersAndRenderTextures() {
    UnloadShader(OTSkybox.CubemapShader);
    UnloadRenderTexture(ParasiteTarget);
    UnloadRenderTexture(OTCoreData.RenderTarget);
}

void UnloadFonts() {
    UnloadFont(OTTextSystem.BarFont);
    UnloadFont(OTTextSystem.RussianBarFont);
    UnloadFont(OTTextSystem.LatinBarFont);
    UnloadFont(OTTextSystem.JapaneseBarFont);
}

void UnloadSounds() {
    UnloadTexture(OTTextSystem.Bar);
    UnloadSound(OTSoundData.CollisionSound);
    UnloadSound(OTSoundData.WalkingSound);
    UnloadSound(OTSoundData.ChasingSound);
    UnloadSound(OTSoundData.UIClick);
    UnloadSound(OTSoundData.Death);
    UnloadSound(OTTextSystem.TextNoise);
}

void DrawTerrainMap() {
    for (int z = 0; z < MaxMapSize; z++) {
        for (int x = 0; x < MaxMapSize; x++) {
            DrawCube({float(x), TerrainHeightMap[z][x], float(z)}, 0.5, 0.5, 0.5, RED);
        }
    }
}

void GenHeights(Image heightmap, Vector3 size) {
    #define GRAY_VALUE(c) ((float)(c.r + c.g + c.b) / 3.0f)

    int mapX = size.x;
    int mapZ = size.z;

    Color *pixels = LoadImageColors(heightmap);

    float Scale = static_cast<float>(heightmap.width) / size.x;

    for (int z = 0; z < mapZ && z < MaxMapSize; ++z) {
        for (int x = 0; x < mapX && x < MaxMapSize; ++x) {
            float Index = (x * Scale) + (z * Scale * heightmap.width);

            int pixelIndex = static_cast<int>(Index);
            if (pixelIndex >= 0 && pixelIndex < heightmap.width * heightmap.height) {
                TerrainHeightMap[z][x] = GRAY_VALUE(pixels[pixelIndex]) / (255 / size.y);
            } else {

                TerrainHeightMap[z][x] = 0.0f;
            }
        }
    }
}