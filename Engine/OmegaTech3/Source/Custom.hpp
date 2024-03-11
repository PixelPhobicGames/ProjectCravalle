#include "Data.hpp"

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
            DrawModelEx(WDLModels.FastModel1, GrassPositions[i], {0, 0, 0}, 0, {1, 1, 1}, WHITE);
        }
    }
}

class Custom {
  public:
    // Custom Data
};

static Custom CustomData;

void LoadCustom(int ID) { // Custom  in Engine Level Behavior
    switch (ID) {}
}

void UpdateCustom(int ID) {
    switch (ID) {
    case 1:
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