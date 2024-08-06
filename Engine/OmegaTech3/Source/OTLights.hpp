#include "Debug.hpp"

int LightCounter = 1;


#define SunID 0

static Color SunLightValues = {15 , 15 , 15 , 255 };

void UpdateLightSources() {

    if (SetSunFlag){
        OTCoreData.GameLights[SunID] = CreateLight(LIGHT_POINT, SetSunPos ,  {SetSunPos.x , SetSunPos.y - 1.0f, SetSunPos.z} , SunLightValues , OTCoreData.Lights);
        SetSunFlag = false;
    }

    float CameraPos[3] = {OTCoreData.RenderCamera.position.x,
                          OTCoreData.RenderCamera.position.y,
                          OTCoreData.RenderCamera.position.z};


    OTCoreData.GameLights[SunID].position = SetSunPos;
    OTCoreData.GameLights[SunID].target = {SetSunPos.x , SetSunPos.y - 1.0f, SetSunPos.z};

    SetShaderValue(OTCoreData.Lights,
                    OTCoreData.Lights.locs[SHADER_LOC_VECTOR_VIEW],
                    CameraPos,
                    SHADER_UNIFORM_VEC3);


    for (int i = 0; i < MAX_LIGHTS - 1; i++)UpdateLightValues(OTCoreData.Lights, OTCoreData.GameLights[i]);

    int AmbientLoc = GetShaderLocation(OTCoreData.Lights, "ambient");
    SetShaderValue(OTCoreData.Lights, AmbientLoc, AmbientLightValues, SHADER_UNIFORM_VEC4);
}

void ClearLights() {
    LightCounter = 1;
    OTCoreData.GameLights[MAX_LIGHTS] = {0};
}

void PutLight(Vector3 Position, Vector3 Target, Color LColor) {
    OTCoreData.GameLights[LightCounter] = CreateLight(LIGHT_POINT, Position ,  Target , LColor , OTCoreData.Lights);

    if (LightCounter != MAX_LIGHTS){
        LightCounter ++;
    }
    else { // Cycle Back if no more Light Room. 
        LightCounter = 1;
    }
}

void DrawLights() {
    for (int i = 1; i < MAX_LIGHTS; i++) {
        if (OTCoreData.GameLights[i].enabled)
            DrawSphereEx(OTCoreData.GameLights[i].position, 0.2f, 8, 8, OTCoreData.GameLights[i].color);
        else
            DrawSphereWires(OTCoreData.GameLights[i].position,
                            0.2f,
                            8,
                            8,
                            ColorAlpha(OTCoreData.GameLights[i].color, 0.3f));
    }
}