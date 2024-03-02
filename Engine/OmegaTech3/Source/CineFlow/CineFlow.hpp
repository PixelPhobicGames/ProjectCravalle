
/* CineFlow */

/* Cinematic Custscene System */

/* Liquid 2023 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../libPPG/PPGIO/PPGIO.hpp"
#include "raylib.h"

#include <cmath> // for std::abs

using namespace std;

#define MaxScriptSize 100

// Function to calculate linear interpolation
float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

class CineFlow {
  public:
    Camera CFCamera = {0};
    int ProgramCounter = 0;

    string ScriptData[MaxScriptSize];

    void InitCamera() {
        CFCamera.position = (Vector3){0.0f, 9.0f, 0.0f};
        CFCamera.target = (Vector3){0.0f, 1.0f, 0.0f};
        CFCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
        CFCamera.fovy = 60.0f;
        CFCamera.projection = CAMERA_PERSPECTIVE;
    }
};

static CineFlow CineFlowData;

static bool UsingCineFlow = false;

bool PlaceCFCameraFlag = false;
Vector3 PlaceCFCameraPosition = (Vector3){0.0f, 0.0f, 0.0f};

void LoadCineFlowScript(const char *Path) {
    for (size_t i = 0; i <= MaxScriptSize - 1; ++i) {
        CineFlowData.ScriptData[i] = "";
    }
    ifstream ScriptFile(Path);

    if (!ScriptFile.is_open()) {
        cerr << "Error: Could not open file " << Path << endl;
        return;
    }

    UsingCineFlow = true;
    CineFlowData.InitCamera();

    vector<string> FileLines;
    string Line;

    while (getline(ScriptFile, Line)) {
        FileLines.push_back(Line);
    }

    if (!ScriptFile.eof() && ScriptFile.fail()) {
        cerr << "Error reading from file " << Path << endl;
        return;
    }

    for (size_t i = 0; i < FileLines.size(); ++i) {
        CineFlowData.ScriptData[i] = FileLines[i];
        cout << CineFlowData.ScriptData[i] << endl;
    }

    CineFlowData.ProgramCounter = 0;
}

void UpdateCineFlow() {
    string Instruction = CineFlowData.ScriptData[CineFlowData.ProgramCounter];

    if (SplitValue(Instruction, 0) == "PointCamera") {
        CineFlowData.CFCamera.target.x = StringToFloat(SplitValue(Instruction, 1));
        CineFlowData.CFCamera.target.y = StringToFloat(SplitValue(Instruction, 2));
        CineFlowData.CFCamera.target.z = StringToFloat(SplitValue(Instruction, 3));
        CineFlowData.ProgramCounter++;
    }

    if (SplitValue(Instruction, 0) == "PlayFadeOut") {
        PlayFadeOut();
        CineFlowData.ProgramCounter++;
    }

    if (SplitValue(Instruction, 0) == "PlayFadeIn") {
        PlayFadeIn();
        CineFlowData.ProgramCounter++;
    }

    if (SplitValue(Instruction, 0) == "PlaceCamera") {

        CineFlowData.CFCamera.position.x = StringToFloat(SplitValue(Instruction, 1));
        CineFlowData.CFCamera.position.y = StringToFloat(SplitValue(Instruction, 2));
        CineFlowData.CFCamera.position.z = StringToFloat(SplitValue(Instruction, 3));
        CineFlowData.ProgramCounter++;
    }

    if (SplitValue(Instruction, 0) == "MoveCamera") {
        float targetX = StringToFloat(SplitValue(Instruction, 1));
        float targetY = StringToFloat(SplitValue(Instruction, 2));
        float targetZ = StringToFloat(SplitValue(Instruction, 3));

        // Set the speed factor (adjust as needed)
        float speed = 0.01f; // You can experiment with different values

        // Calculate the direction vector from current position to the target
        float deltaX = targetX - CineFlowData.CFCamera.position.x;
        float deltaY = targetY - CineFlowData.CFCamera.position.y;
        float deltaZ = targetZ - CineFlowData.CFCamera.position.z;

        // Calculate the distance to the target
        float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

        // Normalize the direction vector
        if (distance > 0.0f) {
            deltaX /= distance;
            deltaY /= distance;
            deltaZ /= distance;
        }

        // Move the camera using linear interpolation
        CineFlowData.CFCamera.position.x = Lerp(CineFlowData.CFCamera.position.x, targetX, speed);
        CineFlowData.CFCamera.position.y = Lerp(CineFlowData.CFCamera.position.y, targetY, speed);
        CineFlowData.CFCamera.position.z = Lerp(CineFlowData.CFCamera.position.z, targetZ, speed);

        // Check if the camera is close enough to the target
        float epsilon = 10.0f; // Adjust as needed
        if (std::abs(CineFlowData.CFCamera.position.x - targetX) < epsilon &&
            std::abs(CineFlowData.CFCamera.position.y - targetY) < epsilon &&
            std::abs(CineFlowData.CFCamera.position.z - targetZ) < epsilon) {
            // Camera is close enough to the target, consider it reached
            CineFlowData.ProgramCounter++;
        }
    }

    if (SplitValue(Instruction, 0) == "End") {
        UsingCineFlow = false;
    }

    if (IsKeyPressed(KEY_SPACE)) {
        UsingCineFlow = false;
    }
}
