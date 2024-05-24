#include "Custom.hpp"

void GenHeights(Image heightmap, Vector3 size) {
    #define GRAY_VALUE(c) ((float)(c.r + c.g + c.b) / 3.0f)

    int mapX = size.x;
    int mapZ = size.z;

    Color *pixels = LoadImageColors(heightmap);

    float Scale = static_cast<float>(heightmap.width) / size.x;

    for (int z = 0; z < mapZ && z < MaxMapSize; ++z) {
        for (int x = 0; x < mapX && x < MaxMapSize; ++x) {
            float Index = (x * Scale) + (z * Scale * heightmap.width);
            // Ensure Index is within the bounds of pixels array
            int pixelIndex = static_cast<int>(Index);
            if (pixelIndex >= 0 && pixelIndex < heightmap.width * heightmap.height) {
                TerrainHeightMap[z][x] = GRAY_VALUE(pixels[pixelIndex]) / (255 / size.y);
            } else {
                // Handle out-of-bounds case
                // Maybe assign a default value or skip this pixel
                TerrainHeightMap[z][x] = 0.0f; // Default value
            }
        }
    }
    // Do something with TerrainHeightMap
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


}

void UpdateNoiseEmitters() {
    UpdateMusicStream(OmegaTechSoundData.NESound1);
    UpdateMusicStream(OmegaTechSoundData.NESound2);
    UpdateMusicStream(OmegaTechSoundData.NESound3);
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
    SceneIDMirror = OmegaTechData.LevelIndex;
    GrassScan = true;

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

    LoadCustom(OmegaTechData.LevelIndex);

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
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OmegaTechData.LevelIndex))) {
        if (IsMusicStreamPlaying(OmegaTechSoundData.NESound1))StopMusicStream(OmegaTechSoundData.NESound1);
        OmegaTechSoundData.NESound1 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OmegaTechData.LevelIndex));
    } else {
        if (IsMusicReady(OmegaTechSoundData.NESound1))UnloadMusicStream(OmegaTechSoundData.NESound1);
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex))) {
        if (IsMusicStreamPlaying(OmegaTechSoundData.NESound2))StopMusicStream(OmegaTechSoundData.NESound2);
        OmegaTechSoundData.NESound2 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex));
    } else {
        if (IsMusicReady(OmegaTechSoundData.NESound2))UnloadMusicStream(OmegaTechSoundData.NESound2);
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex))) {
        if (IsMusicStreamPlaying(OmegaTechSoundData.NESound2))StopMusicStream(OmegaTechSoundData.NESound2);
        OmegaTechSoundData.NESound2 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex));
    } else {
        if (IsMusicReady(OmegaTechSoundData.NESound2))UnloadMusicStream(OmegaTechSoundData.NESound2);
    }
}

void LoadAmbientLight() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OmegaTechData.LevelIndex))) {
        float Red = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OmegaTechData.LevelIndex), 0)) / 255));
        float Green = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OmegaTechData.LevelIndex), 1)) / 255));
        float Blue = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OmegaTechData.LevelIndex), 2)) / 255));
        float Alpha = float((int(PullConfigValue(
            TextFormat("GameData/Worlds/World%i/Config/AmbientLight.conf", OmegaTechData.LevelIndex), 3)) / 255));

        AmbientLightValues[0] = Red;
        AmbientLightValues[1] = Green;
        AmbientLightValues[2] = Blue;
        AmbientLightValues[3] = Alpha;
    }
}

void LoadSkybox() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OmegaTechData.LevelIndex))) {
        OTSkybox.LoadSkybox(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OmegaTechData.LevelIndex));
    }
}

void LoadHeightMap() {
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
}

void LoadModels() {
    for (int i = 1; i <= ModelCount - 1; i++) {
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%i.obj", OmegaTechData.LevelIndex, i))) {
            WDLModels[i].ModelData =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%i.obj", OmegaTechData.LevelIndex, i));
            WDLModels[i].ModelTexture =
                LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model%iTexture.png", OmegaTechData.LevelIndex, i));
            SetTextureFilter(WDLModels[i].ModelTexture, TEXTURE_FILTER_TRILINEAR);
            WDLModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels[i].ModelTexture;
            WDLModels[i].ModelData.materials[0].shader = OmegaTechData.Lights;

            if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.obj", OmegaTechData.LevelIndex, i))) {
                WLowDetailModels[i].ModelData =
                    LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model%iLOD.obj", OmegaTechData.LevelIndex, i));
                WLowDetailModels[i].ModelTexture =
                    LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model%iTexture.png", OmegaTechData.LevelIndex, i));
                SetTextureFilter(WLowDetailModels[i].ModelTexture, TEXTURE_FILTER_TRILINEAR);
                WLowDetailModels[i].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WLowDetailModels[i].ModelTexture;
                WLowDetailModels[i].ModelData.materials[0].shader = OmegaTechData.Lights;
                LowDetail[i] = true;
            } else {
                LowDetail[i] = false;
            }
        }
    }
}

void LoadQMap() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/World.qmap", OmegaTechData.LevelIndex))) {
        QMapSystem.LoadQMap(TextFormat("GameData/Worlds/World%i/World.qmap", OmegaTechData.LevelIndex));

        QMapSystem.EmptyTexture = LoadTextureFromImage(GenImagePerlinNoise(100, 100, 0, 0, 10.0f));

        for (int i = 0; i <= MaxQMapTextures; i++) {
            if (IsPathFile(TextFormat("GameData/Worlds/World%i/QMap/Q%i.png", OmegaTechData.LevelIndex, i))) {
                SetTextureWrap(QMapSystem.MapTextures[i], TEXTURE_WRAP_MIRROR_REPEAT);
                QMapSystem.MapTextures[i] = LoadTexture(TextFormat("GameData/Worlds/World%i/QMap/Q%i.png", OmegaTechData.LevelIndex, i));
            }
        }
    } else {
        QMapSystem.UsingQMaps = false;
    }
}

void LoadSunLight() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex))) {
        SunLightValues.r = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 0);
        SunLightValues.g = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 1);
        SunLightValues.b = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 2);
        SunLightValues.a = PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/SunLight.conf", OmegaTechData.LevelIndex), 3);
    }
}

void LoadWorldData() {
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/World.wdl", OmegaTechData.LevelIndex))) {
        if (GameDataEncoded) {
            WorldData = Encode(LoadFile(TextFormat("GameData/Worlds/World%i/World.wdl", OmegaTechData.LevelIndex)), MainKey);
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
    }
}

void LoadBackgroundMusic() {
    if (OmegaTechSoundData.MusicFound)
        StopMusicStream(OmegaTechSoundData.BackgroundMusic);

    OmegaTechSoundData.MusicFound = false;

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OmegaTechData.LevelIndex))) {
        OmegaTechSoundData.BackgroundMusic =
            LoadMusicStream(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OmegaTechData.LevelIndex));
        OmegaTechSoundData.MusicFound = true;
        PlayMusicStream(OmegaTechSoundData.BackgroundMusic);
    }
}

void LoadScripts() {
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
        if (IsModelLoaded(&WDLModels[i].ModelData)) {
            UnloadModel(WDLModels[i].ModelData);
        }

        if (WDLModels[i].ModelTexture.width != NULL) {
            UnloadTexture(WDLModels[i].ModelTexture);
        }

        if (LowDetail[i]) {
            if (IsModelLoaded(&WLowDetailModels[i].ModelData)) {
                UnloadModel(WLowDetailModels[i].ModelData);
            }

            if (WLowDetailModels[i].ModelTexture.width != NULL) {
                UnloadTexture(WLowDetailModels[i].ModelTexture);
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
    UnloadRenderTexture(Target);
}

void UnloadFonts() {
    UnloadFont(OmegaTechTextSystem.BarFont);
    UnloadFont(OmegaTechTextSystem.RussianBarFont);
    UnloadFont(OmegaTechTextSystem.LatinBarFont);
    UnloadFont(OmegaTechTextSystem.JapaneseBarFont);
}

void UnloadSounds() {
    UnloadTexture(OmegaTechTextSystem.Bar);
    UnloadTexture(OmegaTechData.Cursor);

    UnloadSound(OmegaTechSoundData.CollisionSound);
    UnloadSound(OmegaTechSoundData.WalkingSound);
    UnloadSound(OmegaTechSoundData.ChasingSound);
    UnloadSound(OmegaTechSoundData.UIClick);
    UnloadSound(OmegaTechSoundData.Death);
    UnloadSound(OmegaTechTextSystem.TextNoise);
}

void DrawTerrainMap() {
    for (int z = 0; z < MaxMapSize; z++) {
        for (int x = 0; x < MaxMapSize; x++) {
            DrawCube({float(x), TerrainHeightMap[z][x], float(z)}, 0.5, 0.5, 0.5, RED);
        }
    }
}
