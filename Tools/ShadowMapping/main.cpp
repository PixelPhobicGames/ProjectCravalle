#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <iostream>


using namespace std;

#define WIDTH 800
#define HEIGHT 600

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

Camera3D camera = {0};
Camera3D camera_shadow_map = {0};
Vector3 cube_position = {0.0f, 1.0f, 0.0f};
Model BasePlane = {0};
Model torus = {0};
Model column = {0};
RenderTexture2D render_texture = {0};
Vector3 phase = {0};
Shader shader = {0};
Shader shader_default = {0};
Light light_1 = {0};
Model ShadowPlane;

float phaseLight = .0f;

#define Clear  CLITERAL(Color){ 0 , 0 , 0 , 100} 


void BindLight(){

}

void InitShader(){
    shader = LoadShader("Lights/Lighting.vs", "Lights/Lighting.fs");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    int ambientLoc = GetShaderLocation(shader, "ambient");
    float FArray[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    SetShaderValue(shader, ambientLoc, FArray, SHADER_UNIFORM_VEC4);
    light_1 = CreateLight(LIGHT_POINT, (Vector3){ 200.0f, 500.f, 200.0f}, { 200.0f, 32.f, 200.0f}, WHITE, shader);
    UpdateLightValues(shader, light_1);
}

void InitCamera(){
    camera.fovy = 45.0f;
    camera.target = (Vector3){.0f, .0f, .0f};
    camera.position = (Vector3){0.0f, 10.0f, 10.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};

/*
200
500.14
200

199.943
80.1653
193.077
*/
    camera_shadow_map.fovy = 45.0f;
    camera_shadow_map.target = (Vector3){ 199.943 ,80.1653 , 193.077};
    camera_shadow_map.position = (Vector3){ 200.000f, 500.14f, 200.0f};
    camera_shadow_map.up = (Vector3){0.0f, 1.0f,0.0f};
}

void FlipTextureVertical(Texture2D *texture) {

    //UpdateTexture(*texture, image.data);

    // Unload the temporary image data
    //UnloadImage(image);
}

auto RenderShadow(Model Object, Vector3 Position , float Scale){
    BindLight();

    UpdateLightValues(shader, light_1);

    UpdateCamera(&camera, CAMERA_FREE);

    BeginTextureMode(render_texture);{
        BeginMode3D(camera_shadow_map);
        {
            DrawModel(Object , Position , Scale , BLACK);
        }
        EndMode3D();
    }EndTextureMode();

}


int main(void)
{

    InitWindow(1280, 720, "Shadow Mapping");
    SetTargetFPS(60);

    InitShader();
    InitCamera();


    torus = LoadModel("Assets/Tree.obj");
    torus.materials[0].shader = shader;
    torus.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("Assets/Tree.png");

    Mesh plane_mesh = GenMeshHeightmap(LoadImage("Assets/HeightMap.png"), (Vector3){ 400.f, 32, 400.f});
    BasePlane = LoadModelFromMesh(plane_mesh);
    BasePlane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("Assets/HeightMapTexture.png");

    render_texture = LoadRenderTexture(900, 900);

    ShadowPlane = LoadModelFromMesh(plane_mesh);
    ShadowPlane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = render_texture.texture;

    DisableCursor();

    BeginTextureMode(render_texture);
    ClearBackground(Clear);
    EndTextureMode();

    RenderShadow(torus, {100.0f, -1 , 100.0f}, 1.0f);
    RenderShadow(torus, {150.0f, -1 , 100.0f}, 1.0f);

    Image Temp = LoadImageFromTexture(render_texture.texture);   
    ImageFlipVertical(&Temp);     
    UpdateTexture(render_texture.texture, Temp.data);



    

    while (!WindowShouldClose())
    {

        BeginBlendMode(BLEND_ALPHA);


        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawFPS(10, 10);

            if (IsKeyDown(KEY_TAB)){
                UpdateCamera(&camera_shadow_map , CAMERA_FREE);

                BeginMode3D(camera_shadow_map);
                    DrawPoint3D(light_1.position , YELLOW);

                    DrawModel(torus,{100.0f, -1 , 100.0f}, 1.0f, WHITE);
                    DrawModel(torus,{150.0f, -1 , 100.0f}, 1.0f, WHITE);
                    DrawModel(BasePlane, (Vector3){0, -1.f, 0}, 1.f, WHITE);
                    DrawModel(ShadowPlane, (Vector3){0, -1.f, 0}, 1.f, BLACK);
                EndMode3D();


                if (IsKeyPressed(KEY_ENTER)){
                    cout << camera_shadow_map.position.x << "\n";
                    cout << camera_shadow_map.position.y << "\n";
                    cout << camera_shadow_map.position.z << "\n";

                    cout << camera_shadow_map.target.x << "\n";
                    cout << camera_shadow_map.target.y << "\n";
                    cout << camera_shadow_map.target.z << "\n";
                }
                
            }
            else {
                BeginMode3D(camera);
                {
                    UpdateCamera(&camera , CAMERA_FREE);

                    DrawPoint3D(light_1.position , YELLOW);

                    DrawModel(torus,{100.0f, -1 , 100.0f}, 1.0f, WHITE);
                    DrawModel(torus,{150.0f, -1 , 100.0f}, 1.0f, WHITE);

                    DrawModel(BasePlane, (Vector3){0, -1.f, 0}, 1.f, WHITE);

                    DrawModel(ShadowPlane, (Vector3){0, -1.f, 0}, 1.f, Clear);
                }
                EndMode3D();
            }

            if (IsKeyDown(KEY_UP)){
                light_1.position.y += 1.0f;
            }
            if (IsKeyDown(KEY_DOWN)){
                light_1.position.y -= 1.0f;
            }
        }
        EndDrawing();
        EndBlendMode();
    }

    UnloadModel(torus);
    CloseWindow();

    return 0;
}