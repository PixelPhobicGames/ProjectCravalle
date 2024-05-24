#include "Shadow.hpp"



Camera3D camera = {0};


Model Tree;
Model Car;
Model Ground;

void DrawScene(){
    DrawModel(Tree, { 0 , 0 , 0 } , 1.0f , WHITE);

    DrawModel(Car, { -10 , 0 , -10 } , 1.0f , WHITE);

    DrawModel(Ground , { 0 , 0 , 0 } , 1.0f , WHITE);
}

int main(){

    InitWindow(1280 , 720, "Lighting Test");

    SetTargetFPS(60);

    OTLightingSystem.Init();



    camera.fovy = 45.0f;
    camera.target = (Vector3){.0f, .0f, .0f};
    camera.position = (Vector3){0.0f, 10.0f / 8, 10.0f};
    camera.up = (Vector3){0.0f, 1.0f / 8, 0.0f};

    Tree = LoadModel("Assets/Tree.obj");
    Tree.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("Assets/Tree.png");
    Tree.materials[0].shader  = OTLightingSystem.ShadowShader;

    Car = LoadModel("Assets/Car.obj");
    Car.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("Assets/Car.png");
    Car.materials[0].shader  =  OTLightingSystem.ShadowShader;

    Ground = LoadModelFromMesh(GenMeshCube(800.0f, 1.0f, 800.0f));
    Ground.materials[0].shader  =  OTLightingSystem.ShadowShader;


    RenderTexture2D Target = LoadRenderTexture(1280 , 720);

    OTLightingSystem.PositionMainLight({ 0.35f, -1.0f, -0.35f }, -15);


    while (!WindowShouldClose())
    {

        OTLightingSystem.RenderShadows(camera, DrawScene);

        BeginTextureMode(Target);

        ClearBackground(BLACK);

            BeginMode3D(camera);
            
                UpdateCamera(&camera , CAMERA_ORBITAL);


                DrawScene();

            
            EndMode3D();

        EndTextureMode();

        BeginDrawing();

            ClearBackground(BLACK);

            DrawTexturePro(Target.texture , {0 , 0 , Target.texture.width , -Target.texture.height} , {0 , 0 , 1280 , 720} , {0,0 } , 0.0f , WHITE);


        EndDrawing();
    }

    CloseWindow();

    
}