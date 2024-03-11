#include "Custom.hpp"

void LoadSave();
void SaveGame();
void CacheWDL();
void PutLight(Vector3 Position);
void ClearLights();

class ParticleSystem;

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

    LODWDLModels.ClearModelInfo();

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OmegaTechData.LevelIndex))) {
        WDLModels.HeightMapW =
            PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/HeightMap.conf", OmegaTechData.LevelIndex), 0);
        WDLModels.HeightMapH =
            PullConfigValue(TextFormat("GameData/Worlds/World%i/Config/HeightMap.conf", OmegaTechData.LevelIndex), 1);

        WDLModels.HeightMapTexture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/HeightMapTexture.png", OmegaTechData.LevelIndex));
        Image HeightMapImage =
            LoadImage(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OmegaTechData.LevelIndex));
        Texture2D Texture = LoadTextureFromImage(HeightMapImage);

        // Generate Height map Unless pregenerated one exists.

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Terrain.obj", OmegaTechData.LevelIndex))) {
            WDLModels.HeightMap =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Terrain.obj", OmegaTechData.LevelIndex));
            WDLModels.HeightMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.HeightMapTexture;
            WDLModels.HeightMap.materials[0].shader = OmegaTechData.Lights;
        } else {
            Mesh Mesh1 = GenMeshHeightmap(HeightMapImage,
                                          (Vector3){WDLModels.HeightMapW, WDLModels.HeightMapH, WDLModels.HeightMapW});

            WDLModels.HeightMap = LoadModelFromMesh(Mesh1);
            WDLModels.HeightMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.HeightMapTexture;
            WDLModels.HeightMap.meshes[0] = Mesh1;
            WDLModels.HeightMap.materials[0].shader = OmegaTechData.Lights;
        }

        GenHeights(HeightMapImage, {(Vector3){WDLModels.HeightMapW, WDLModels.HeightMapH, WDLModels.HeightMapW}});
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model1.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model1 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model1.obj", OmegaTechData.LevelIndex));
        WDLModels.Model1Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model1Texture.png", OmegaTechData.LevelIndex));

        SetTextureFilter(WDLModels.Model1Texture, TEXTURE_FILTER_TRILINEAR);

        WDLModels.Model1.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model1Texture;
        WDLModels.Model1.materials[0].shader = OmegaTechData.Lights;

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model1LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model1 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model1LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model1.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model1Texture;
            LODWDLModels.Model1.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel1 = true;
        }
    } else {
        if (WDLModels.Model1.meshCount != 0) {
            UnloadModel(WDLModels.Model1);
        }
        if (WDLModels.Model1Texture.id != 0) {
            UnloadTexture(WDLModels.Model1Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model2.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model2 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model2.obj", OmegaTechData.LevelIndex));
        WDLModels.Model2Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model2Texture.png", OmegaTechData.LevelIndex));

        SetTextureFilter(WDLModels.Model2Texture, TEXTURE_FILTER_TRILINEAR);

        WDLModels.Model2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model2Texture;
        WDLModels.Model2.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model2LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model2 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model2LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model2Texture;
            LODWDLModels.Model2.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel2 = true;
        }
    } else {
        if (WDLModels.Model2.meshCount != 0) {
            UnloadModel(WDLModels.Model2);
        }
        if (WDLModels.Model2Texture.id != 0) {
            UnloadTexture(WDLModels.Model2Texture);
        }
    }
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model3.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model3 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model3.obj", OmegaTechData.LevelIndex));
        WDLModels.Model3Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model3Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model3Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model3.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model3Texture;
        WDLModels.Model3.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model3LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model3 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model3LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model3.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model3Texture;
            LODWDLModels.Model3.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel3 = true;
        }
    } else {
        if (WDLModels.Model3.meshCount != 0) {
            UnloadModel(WDLModels.Model13);
        }
        if (WDLModels.Model3Texture.id != 0) {
            UnloadTexture(WDLModels.Model3Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model4.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model4 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model4.obj", OmegaTechData.LevelIndex));
        WDLModels.Model4Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model4Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model4Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model4.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model4Texture;
        WDLModels.Model4.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model4LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model4 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model4LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model4.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model4Texture;
            LODWDLModels.Model4.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel4 = true;
        }
    } else {
        if (WDLModels.Model4.meshCount != 0) {
            UnloadModel(WDLModels.Model4);
        }
        if (WDLModels.Model4Texture.id != 0) {
            UnloadTexture(WDLModels.Model4Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model5.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model5 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model5.obj", OmegaTechData.LevelIndex));
        WDLModels.Model5Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model5Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model5Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model5.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model5Texture;
        WDLModels.Model5.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model5LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model5 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model5LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model5.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model5Texture;
            LODWDLModels.Model5.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel5 = true;
        }
    } else {
        if (WDLModels.Model5.meshCount != 0) {
            UnloadModel(WDLModels.Model5);
        }
        if (WDLModels.Model5Texture.id != 0) {
            UnloadTexture(WDLModels.Model5Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model6.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model6 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model6.obj", OmegaTechData.LevelIndex));
        WDLModels.Model6Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model6Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model6Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model6.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model6Texture;
        WDLModels.Model6.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model6LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model6 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model6LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model6.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model6Texture;
            LODWDLModels.Model6.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel6 = true;
        }
    } else {
        if (WDLModels.Model6.meshCount != 0) {
            UnloadModel(WDLModels.Model6);
        }
        if (WDLModels.Model6Texture.id != 0) {
            UnloadTexture(WDLModels.Model6Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model7.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model7 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model7.obj", OmegaTechData.LevelIndex));
        WDLModels.Model7Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model7Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model7Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model7.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model7Texture;
        WDLModels.Model7.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model7LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model7 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model7LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model7.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model7Texture;
            LODWDLModels.Model7.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel7 = true;
        }

    } else {
        if (WDLModels.Model7.meshCount != 0) {
            UnloadModel(WDLModels.Model7);
        }
        if (WDLModels.Model7Texture.id != 0) {
            UnloadTexture(WDLModels.Model7Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model8.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model8 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model8.obj", OmegaTechData.LevelIndex));
        WDLModels.Model8Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model8Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model8Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model8.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model8Texture;
        WDLModels.Model8.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model8LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model8 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model8LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model8.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model8Texture;
            LODWDLModels.Model8.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel8 = true;
        }

    } else {
        if (WDLModels.Model8.meshCount != 0) {
            UnloadModel(WDLModels.Model8);
        }
        if (WDLModels.Model8Texture.id != 0) {
            UnloadTexture(WDLModels.Model8Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model9.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model9 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model9.obj", OmegaTechData.LevelIndex));
        WDLModels.Model9Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model9Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model9Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model9.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model9Texture;
        WDLModels.Model9.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model9LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model9 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model9LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model9.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model9Texture;
            LODWDLModels.Model9.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel9 = true;
        }

    } else {
        if (WDLModels.Model9.meshCount != 0) {
            UnloadModel(WDLModels.Model9);
        }
        if (WDLModels.Model9Texture.id != 0) {
            UnloadTexture(WDLModels.Model9Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model10.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model10 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model10.obj", OmegaTechData.LevelIndex));
        WDLModels.Model10Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model10Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model10Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model10.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model10Texture;
        WDLModels.Model10.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model10LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model10 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model10LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model10.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model10Texture;
            LODWDLModels.Model10.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel10 = true;
        }

    } else {
        if (WDLModels.Model10.meshCount != 0) {
            UnloadModel(WDLModels.Model10);
        }
        if (WDLModels.Model10Texture.id != 0) {
            UnloadTexture(WDLModels.Model10Texture);
        }
    }
    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model11.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model11 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model11.obj", OmegaTechData.LevelIndex));
        WDLModels.Model11Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model11Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model11Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model11.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model11Texture;
        WDLModels.Model11.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model11LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model11 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model11LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model11.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model11Texture;
            LODWDLModels.Model11.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel11 = true;
        }

    } else {
        if (WDLModels.Model11.meshCount != 0) {
            UnloadModel(WDLModels.Model11);
        }
        if (WDLModels.Model11Texture.id != 0) {
            UnloadTexture(WDLModels.Model11Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model12.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model12 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model12.obj", OmegaTechData.LevelIndex));
        WDLModels.Model12Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model12Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model12Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model12.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model12Texture;
        WDLModels.Model12.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model12LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model12 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model12LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model12.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model12Texture;
            LODWDLModels.Model12.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel12 = true;
        }

    } else {
        if (WDLModels.Model12.meshCount != 0) {
            UnloadModel(WDLModels.Model12);
        }
        if (WDLModels.Model12Texture.id != 0) {
            UnloadTexture(WDLModels.Model12Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model13.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model13 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model13.obj", OmegaTechData.LevelIndex));
        WDLModels.Model13Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model13Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model13Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model13.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model13Texture;
        WDLModels.Model13.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model13LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model13 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model13LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model13.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model13Texture;
            LODWDLModels.Model13.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel13 = true;
        }

    } else {
        if (WDLModels.Model13.meshCount != 0) {
            UnloadModel(WDLModels.Model13);
        }
        if (WDLModels.Model13Texture.id != 0) {
            UnloadTexture(WDLModels.Model13Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model14.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model14 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model14.obj", OmegaTechData.LevelIndex));
        WDLModels.Model14Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model14Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model14Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model14.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model14Texture;
        WDLModels.Model14.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model14LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model14 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model14LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model14.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model14Texture;
            LODWDLModels.Model14.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel14 = true;
        }

    } else {
        if (WDLModels.Model14.meshCount != 0) {
            UnloadModel(WDLModels.Model14);
        }
        if (WDLModels.Model14Texture.id != 0) {
            UnloadTexture(WDLModels.Model14Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model15.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model15 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model15.obj", OmegaTechData.LevelIndex));
        WDLModels.Model15Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model15Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model15Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model15.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model15Texture;
        WDLModels.Model15.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model15LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model15 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model15LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model15.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model15Texture;
            LODWDLModels.Model15.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel15 = true;
        }

    } else {
        if (WDLModels.Model15.meshCount != 0) {
            UnloadModel(WDLModels.Model15);
        }
        if (WDLModels.Model15Texture.id != 0) {
            UnloadTexture(WDLModels.Model15Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model16.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model16 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model16.obj", OmegaTechData.LevelIndex));
        WDLModels.Model16Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model16Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model16Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model16.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model16Texture;
        WDLModels.Model16.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model16LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model16 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model16LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model16.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model16Texture;
            LODWDLModels.Model16.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel16 = true;
        }

    } else {
        if (WDLModels.Model16.meshCount != 0) {
            UnloadModel(WDLModels.Model16);
        }
        if (WDLModels.Model16Texture.id != 0) {
            UnloadTexture(WDLModels.Model16Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model17.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model17 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model17.obj", OmegaTechData.LevelIndex));
        WDLModels.Model17Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model17Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model17Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model17.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model17Texture;
        WDLModels.Model17.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model17LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model17 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model17LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model17.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model17Texture;
            LODWDLModels.Model17.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel17 = true;
        }

    } else {
        if (WDLModels.Model17.meshCount != 0) {
            UnloadModel(WDLModels.Model17);
        }
        if (WDLModels.Model17Texture.id != 0) {
            UnloadTexture(WDLModels.Model17Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model18.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model18 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model18.obj", OmegaTechData.LevelIndex));
        WDLModels.Model18Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model18Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model18Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model18.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model18Texture;
        WDLModels.Model18.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model18LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model18 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model18LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model18.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model18Texture;
            LODWDLModels.Model18.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel18 = true;
        }

    } else {
        if (WDLModels.Model18.meshCount != 0) {
            UnloadModel(WDLModels.Model18);
        }
        if (WDLModels.Model18Texture.id != 0) {
            UnloadTexture(WDLModels.Model18Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model19.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model19 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model19.obj", OmegaTechData.LevelIndex));
        WDLModels.Model19Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model19Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model19Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model19.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model19Texture;
        WDLModels.Model19.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model19LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model19 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model19LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model19.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model19Texture;
            LODWDLModels.Model19.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel19 = true;
        }

    } else {
        if (WDLModels.Model19.meshCount != 0) {
            UnloadModel(WDLModels.Model19);
        }
        if (WDLModels.Model19Texture.id != 0) {
            UnloadTexture(WDLModels.Model19Texture);
        }
    }

    if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model20.obj", OmegaTechData.LevelIndex))) {
        WDLModels.Model20 =
            LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model20.obj", OmegaTechData.LevelIndex));
        WDLModels.Model20Texture =
            LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model20Texture.png", OmegaTechData.LevelIndex));
        SetTextureFilter(WDLModels.Model20Texture, TEXTURE_FILTER_TRILINEAR);
        WDLModels.Model20.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model20Texture;
        WDLModels.Model20.materials[0].shader = OmegaTechData.Lights;
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model20LOD.obj", OmegaTechData.LevelIndex))) {
            LODWDLModels.Model20 =
                LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model20LOD.obj", OmegaTechData.LevelIndex));
            LODWDLModels.Model20.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model20Texture;
            LODWDLModels.Model20.materials[0].shader = OmegaTechData.Lights;
            LODWDLModels.LODAvailableModel20 = true;
        }

    } else {
        if (WDLModels.Model20.meshCount != 0) {
            UnloadModel(WDLModels.Model20);
        }
        if (WDLModels.Model20Texture.id != 0) {
            UnloadTexture(WDLModels.Model20Texture);
        }
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

void LoadLaunchConfig() {
    wstring Config = LoadFile("GameData/Launch.conf");

    wstring Resolution = WSplitValue(Config, 0);

    switch (Resolution[0]) {
    case L'1':
        SetWindowSize(640, 480);
        break;
    case L'2':
        SetWindowSize(1280, 720);
        break;
    case L'3':
        SetWindowSize(1980, 1080);
        break;
    case L'4':
        SetWindowSize(2560, 1440);
        break;
    case L'5':
        SetWindowSize(3840, 2160);
        break;
    default:
        SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
        ToggleFullscreen();
        break;
    }
}

int LightCounter = 1;

void UpdateLightSources() {
    float CameraPos[3] = {OmegaTechData.MainCamera.position.x,
                          OmegaTechData.MainCamera.position.y,
                          OmegaTechData.MainCamera.position.z};

    if (IsMouseButtonDown(1)) {
        OmegaTechData.GameLights[0].position = {OmegaTechData.MainCamera.position.x,
                                                OmegaTechData.MainCamera.position.y,
                                                OmegaTechData.MainCamera.position.z};
        OmegaTechData.GameLights[0].target = OmegaTechData.MainCamera.target;
        SetShaderValue(OmegaTechData.Lights,
                       OmegaTechData.Lights.locs[SHADER_LOC_VECTOR_VIEW],
                       CameraPos,
                       SHADER_UNIFORM_VEC3);
    } else {
        OmegaTechData.GameLights[0].position = {-1000, -1000, -1000};
        OmegaTechData.GameLights[0].target = {-1000, -1000, -1000};
        SetShaderValue(OmegaTechData.Lights,
                       OmegaTechData.Lights.locs[SHADER_LOC_VECTOR_VIEW],
                       CameraPos,
                       SHADER_UNIFORM_VEC3);
    }

    for (int i = 0; i < MAX_LIGHTS - 1; i++)
        UpdateLightValues(OmegaTechData.Lights, OmegaTechData.GameLights[i]);

    int AmbientLoc = GetShaderLocation(OmegaTechData.Lights, "ambient");
    SetShaderValue(OmegaTechData.Lights, AmbientLoc, AmbientLightValues, SHADER_UNIFORM_VEC4);
}

void ClearLights() {
    LightCounter = 1;
    OmegaTechData.GameLights[MAX_LIGHTS] = {0};
}

void PutLight(Vector3 Position) {
    OmegaTechData.GameLights[LightCounter] =
        CreateLight(LIGHT_DIRECTIONAL, Position, {0, 0, 0}, WHITE, OmegaTechData.Lights);
    LightCounter++;
}

void DrawLights() {
    for (int i = 1; i < MAX_LIGHTS; i++) {
        if (OmegaTechData.GameLights[i].enabled)
            DrawSphereEx(OmegaTechData.GameLights[i].position, 0.2f, 8, 8, OmegaTechData.GameLights[i].color);
        else
            DrawSphereWires(OmegaTechData.GameLights[i].position,
                            0.2f,
                            8,
                            8,
                            ColorAlpha(OmegaTechData.GameLights[i].color, 0.3f));
    }
}

void OmegaTechInit() {
    LoadLaunchConfig();
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

    if (IsPathFile("GameData/Global/FModels/FModel1.gltf")) {
        WDLModels.FastModel1 = LoadModel("GameData/Global/FModels/FModel1.gltf");
        WDLModels.FastModel1Texture = LoadTexture("GameData/Global/FModels/FModel1Texture.png");
        WDLModels.FastModel1.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel1Texture;
        WDLModels.FastModel1.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel2.gltf")) {
        WDLModels.FastModel2 = LoadModel("GameData/Global/FModels/FModel2.gltf");
        WDLModels.FastModel2Texture = LoadTexture("GameData/Global/FModels/FModel2Texture.png");
        WDLModels.FastModel2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel2Texture;
        WDLModels.FastModel2.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel3.gltf")) {
        WDLModels.FastModel3 = LoadModel("GameData/Global/FModels/FModel3.gltf");
        WDLModels.FastModel3Texture = LoadTexture("GameData/Global/FModels/FModel3Texture.png");
        WDLModels.FastModel3.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel3Texture;
        WDLModels.FastModel3.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel4.gltf")) {
        WDLModels.FastModel4 = LoadModel("GameData/Global/FModels/FModel4.gltf");
        WDLModels.FastModel4Texture = LoadTexture("GameData/Global/FModels/FModel4Texture.png");
        WDLModels.FastModel4.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel4Texture;
        WDLModels.FastModel4.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel5.gltf")) {
        WDLModels.FastModel5 = LoadModel("GameData/Global/FModels/FModel5.gltf");
        WDLModels.FastModel5Texture = LoadTexture("GameData/Global/FModels/FModel5Texture.png");
        WDLModels.FastModel5.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel5Texture;
        WDLModels.FastModel5.materials[0].shader = OmegaTechData.Lights;
    }

    Color LightColor = {1, 1, 1, 1};

    OmegaTechData.GameLights[0] =
        CreateLight(LIGHT_DIRECTIONAL, OmegaTechData.MainCamera.target, {0, 0, 0}, LightColor, OmegaTechData.Lights);

    Target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    ParasiteTarget = LoadRenderTexture(1280, 720);

    WorldData.reserve(MaxWStringAlloc);
    OtherWDLData.reserve(MaxWStringAlloc);
    ExtraWDLInstructions.reserve(MaxWStringAlloc);

    FinalWDL.reserve(MaxWStringAlloc * 3);

    InitObjects();

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

            PlayFMV();

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

BoundingBox CalculateBoundingBox(float initialX, float initialY, float initialZ, float scale) {
    BoundingBox box;

    float halfExtentX = 1.5 * scale;
    float halfExtentY = 1.5 * scale;
    float halfExtentZ = 1.5 * scale;

    box.min.x = initialX - halfExtentX;
    box.min.y = initialY - halfExtentY;
    box.min.z = initialZ - halfExtentZ;

    box.max.x = initialX + halfExtentX;
    box.max.y = initialY + halfExtentY;
    box.max.z = initialZ + halfExtentZ;

    return box;
}

Vector3 PointAverage[100];

bool IsModelOnScreen(Model Object, Vector3 modelPosition, float Scale) {
    int VertCount = Object.meshes->vertexCount;

    // Ensure we do not go beyond the available vertices
    int maxPoints = (VertCount < 100) ? VertCount : 100;

    // Calculate a step to evenly sample points from the model
    int step = (float)VertCount / maxPoints;

    // Assuming vertices is a float *
    float *floatVertices = Object.meshes->vertices;

    for (int i = 0; i < maxPoints; i++) {
        int index = (int)(i * step);

        // Interpret float value as Vector3
        Vector3 vertex = {floatVertices[index * 3], floatVertices[index * 3 + 1], floatVertices[index * 3 + 2]};

        // Apply scaling
        PointAverage[i] = Vector3Scale(vertex, Scale);

        // Translate by the model's position
        PointAverage[i] = Vector3Add(PointAverage[i], modelPosition);
    }

    // Check if any of the points are on the screen
    for (int i = 0; i < maxPoints; i++) {
        if (IsPointOnScreen(PointAverage[i], OmegaTechData.MainCamera)) {
            return true; // At least one point is on screen
        }
    }

    return false; // No points are on screen
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
            DrawModelEx(WDLModels.HeightMap, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            break;
        case 1:
            if (LODWDLModels.LODAvailableModel1 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model1, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model1, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model1, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model1, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 2:
            if (LODWDLModels.LODAvailableModel2 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model2, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model2, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model2, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model2, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 3:
            if (LODWDLModels.LODAvailableModel3 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model3, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model3, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model3, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model3, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 4:
            if (LODWDLModels.LODAvailableModel4 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model4, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model4, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model4, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model4, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 5:
            if (LODWDLModels.LODAvailableModel5 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model5, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model5, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model5, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model5, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 6:
            if (LODWDLModels.LODAvailableModel6 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model6, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model6, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model6, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model6, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 7:
            if (LODWDLModels.LODAvailableModel7 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model7, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model7, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model7, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model7, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 8:
            if (LODWDLModels.LODAvailableModel8 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model8, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model8, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model8, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model8, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 9:
            if (LODWDLModels.LODAvailableModel9 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model9, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model9, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model9, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model9, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 10:
            if (LODWDLModels.LODAvailableModel10 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model10, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model10, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model10, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model10, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 11:
            if (LODWDLModels.LODAvailableModel11 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model11, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model11, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model11, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model11, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 12:
            if (LODWDLModels.LODAvailableModel12 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model12, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model12, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model12, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model12, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 13:
            if (LODWDLModels.LODAvailableModel13 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model13, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model13, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model13, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model13, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 14:
            if (LODWDLModels.LODAvailableModel14 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model14, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model14, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model14, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model14, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 15:
            if (LODWDLModels.LODAvailableModel15 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model15, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model15, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model15, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model15, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 16:
            if (LODWDLModels.LODAvailableModel16 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model16, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model16, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model16, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model16, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 17:
            if (LODWDLModels.LODAvailableModel17 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model17, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model17, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model17, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model17, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 18:
            if (LODWDLModels.LODAvailableModel18 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model18, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model18, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model8, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model18, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 19:
            if (LODWDLModels.LODAvailableModel19 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model19, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model19, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model19, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model19, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        case 20:
            if (LODWDLModels.LODAvailableModel20 && LOD) {
                if (IsModelOnScreen(LODWDLModels.Model20, {X, Y, Z}, S))
                    DrawModelEx(LODWDLModels.Model20, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            } else {
                if (IsModelOnScreen(WDLModels.Model20, {X, Y, Z}, S))
                    DrawModelEx(WDLModels.Model20, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            }
            break;
        default:
            break;
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
            if (WReadValue(Instruction, 0, 4) == L"Model") {
                int Identifier = ToFloat(WReadValue(Instruction, 5, 6));
                switch (Identifier) {
                case 1:
                    DrawModelEx(WDLModels.Model1, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 2:
                    DrawModelEx(WDLModels.Model2, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 3:
                    DrawModelEx(WDLModels.Model3, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 4:
                    DrawModelEx(WDLModels.Model4, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 5:
                    DrawModelEx(WDLModels.Model5, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 6:
                    DrawModelEx(WDLModels.Model6, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 7:
                    DrawModelEx(WDLModels.Model7, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 8:
                    DrawModelEx(WDLModels.Model8, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 9:
                    DrawModelEx(WDLModels.Model9, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 10:
                    DrawModelEx(WDLModels.Model10, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 11:
                    DrawModelEx(WDLModels.Model11, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 12:
                    DrawModelEx(WDLModels.Model12, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 13:
                    DrawModelEx(WDLModels.Model13, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 14:
                    DrawModelEx(WDLModels.Model14, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 15:
                    DrawModelEx(WDLModels.Model15, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 16:
                    DrawModelEx(WDLModels.Model16, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 17:
                    DrawModelEx(WDLModels.Model17, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 18:
                    DrawModelEx(WDLModels.Model18, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 19:
                    DrawModelEx(WDLModels.Model19, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 20:
                    DrawModelEx(WDLModels.Model20, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                default:
                    break;
                }
            }

            if (NextCollision) {
                BoundingBox ModelBox = CalculateBoundingBox(X, Y, Z, S);

                if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds, ModelBox)) {
                    ObjectCollision = true;
                }
                NextCollision = false;
            }

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

        if (Instruction == L"HeightMap") { // ?
            WDLModels.HeightMapPosition.x = X;
            WDLModels.HeightMapPosition.y = Y;
            WDLModels.HeightMapPosition.z = Z;
            DrawModel(WDLModels.HeightMap, {X, Y, Z}, S, FadeColor);
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
        if (OmegaTechData.MainCamera.position.z >= 0 && OmegaTechData.MainCamera.position.z <= WDLModels.HeightMapW) {
            if (OmegaTechData.MainCamera.position.x >= 0 &&
                OmegaTechData.MainCamera.position.x <= WDLModels.HeightMapW) {
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
        OmegaTechData.Ticker++;
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