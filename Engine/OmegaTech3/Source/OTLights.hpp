#include "Debug.hpp"

int LightCounter = 1;


#define SunID 0

static Color SunLightValues = {15 , 15 , 15 , 255 };

void UpdateLightSources() {

    if (SetSunFlag){
        OmegaTechData.GameLights[SunID] = CreateLight(LIGHT_DIRECTIONAL, SetSunPos ,  {SetSunPos.x , SetSunPos.y - 1.0f, SetSunPos.z} , SunLightValues , OmegaTechData.Lights);
        SetSunFlag = false;
    }

    float CameraPos[3] = {OmegaTechData.MainCamera.position.x,
                          OmegaTechData.MainCamera.position.y,
                          OmegaTechData.MainCamera.position.z};


    OmegaTechData.GameLights[SunID].position = SetSunPos;
    OmegaTechData.GameLights[SunID].target = {SetSunPos.x , SetSunPos.y - 1.0f, SetSunPos.z};

    SetShaderValue(OmegaTechData.Lights,
                    OmegaTechData.Lights.locs[SHADER_LOC_VECTOR_VIEW],
                    CameraPos,
                    SHADER_UNIFORM_VEC3);


    for (int i = 0; i < MAX_LIGHTS - 1; i++)UpdateLightValues(OmegaTechData.Lights, OmegaTechData.GameLights[i]);

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