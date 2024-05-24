#include "QMaps.hpp"



int main(){

    InitWindow(1280 , 720, "QMaps Test");

    SetTargetFPS(60);

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.2f, 4.4f, 0.2f };    // Camera position
    camera.target = (Vector3){ 0.185f, 0.4f, 0.0f };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;    


    QMapSystem.LoadQMap("Example/World1.qmap");

    HideCursor();
    DisableCursor();


    while (!WindowShouldClose())
    {

        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera);

                UpdateCamera(&camera , CAMERA_FREE);
                

                DrawGrid(100 , 1);

                QMapSystem.RenderQMap();

            EndMode3D();

        EndDrawing();
    }

    CloseWindow();
    
}