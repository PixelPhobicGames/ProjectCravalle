
#include "OTLights.hpp"

void UpdateGrassGeneration() {
    if (GrassEnabled) {
        if (GrassTicker != 60 * 3) {
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
                GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 ||
                GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) != 0)
                GrassTicker++;
        } else {
            GrassTicker = 0;
            GrassScan = true;
        }

        if (GrassScan) {
            int Range = 300;

            for (int i = 0; i <= RenderedGrass - 1; i++) {
                int GrassX = GetRandomValue(int(OmegaTechData.MainCamera.position.x - Range),
                                            int(OmegaTechData.MainCamera.position.x + Range));
                int GrassZ = GetRandomValue(int(OmegaTechData.MainCamera.position.z - Range),
                                            int(OmegaTechData.MainCamera.position.z + Range));
                int GrassHeight = TerrainHeightMap[GrassZ][GrassX];

                GrassPositions[i] = {GrassX, GrassHeight, GrassZ};
            }

            GrassScan = false;
        }

        for (int i = 0; i <= RenderedGrass - 1; i++) { // By Default  Grass Uses FModel 1
            DrawModelEx(FastModels[1].ModelData, GrassPositions[i], {0, 0, 0}, 0, {1, 1, 1}, WHITE);
        }
    }
}

class Custom {
  public:
    // Custom Data
    int SunValues = 0;
    int SunCounter = 0;
    int SunDirection = 0;

};

static Custom CustomData;

void LoadCustom(int ID) { // Custom  in Engine Level Behavior
    switch (ID) {
        case 1:
            CustomData.SunValues = 255;
            CustomData.SunDirection = 0;
            CustomData.SunCounter = 0;

            if (IsPathFile("GameData/Global/FModels/FModel1.gltf")) {
                FastModels[1].ModelData= LoadModel("GameData/Global/FModels/FModel1.gltf");
                FastModels[1].ModelTexture= LoadTexture("GameData/Global/FModels/FModel1Texture.png");
                FastModels[1].ModelData.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = FastModels[1].ModelTexture;
                FastModels[1].ModelData.materials[0].shader = OmegaTechData.Lights;
            }
            
            break;
    }
}

void UpdateCustom(int ID) {
    switch (ID) {
        case 1:

            // UpdateGrassGeneration();

            /*
            AmbientLightValues[0] = float(CustomData.SunValues / 255.0f);
            AmbientLightValues[1] = float(CustomData.SunValues / 255.0f);
            AmbientLightValues[2] = float(CustomData.SunValues / 255.0f);

            SColour = (Color){CustomData.SunValues , CustomData.SunValues , CustomData.SunValues , 255};

            if (CustomData.SunCounter >= 1){
                if (CustomData.SunValues == 0){
                    CustomData.SunDirection = 1;
                }

                if (CustomData.SunValues == 255){
                    CustomData.SunDirection = 0;
                }

                if (CustomData.SunDirection == 0){
                    CustomData.SunValues --;
                }
                if (CustomData.SunDirection == 1){
                    CustomData.SunValues ++;
                }

                CustomData.SunCounter = 0;
            }
            else {
                CustomData.SunCounter ++;
            }

            */

            break;

        default:
            break;
    }
}

auto UpdateCustomUI(int ID) {
    switch (ID) {
        case 1:


            break;

        default:
            break;
    }
}