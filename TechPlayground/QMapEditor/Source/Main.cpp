#include "Editor.hpp"



int main(){

    InitWindow(1280 , 720, "QMaps Test");

    SetTargetFPS(60);

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.2f, 4.4f, 0.2f };    // Camera position
    camera.target = (Vector3){ 0.185f, 0.4f, 0.0f };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;    

    QMapSystem.LoadQMap("Working/World1.qmap");

    HideCursor();
    DisableCursor();


    while (!WindowShouldClose())
    {

        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera);

                if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsCursorHidden() ){
                    UpdateCamera(&camera , CAMERA_FREE);
                }

                DrawGrid(100 , 1);

                QMapSystem.RenderQMap();


                QMEditorCore.UpdateEdior(camera);



            EndMode3D();

            if (IsKeyPressed(KEY_ONE)){
                QMEditorCore.CurrentMode = QMEditorCore.PointFaceCreation;
            }

            if (IsKeyPressed(KEY_TWO)){
                QMEditorCore.CurrentMode = QMEditorCore.QuadFaceCreation;
            }

            if (IsKeyPressed(KEY_THREE)){
                QMEditorCore.CurrentMode = QMEditorCore.FaceDeletion;
            }


            if (QMEditorCore.CurrentMode == 0){
                DrawText("Point Face Creation", 1280 - 300 , 10 , 15 , WHITE);
                DrawText(TextFormat("Current Point: %i" , QMEditorCore.PointCounter) , 10 , 10 , 15 , RED);
                DrawText(TextFormat("Texture ID: %i" , QMEditorCore.EditorTextureID) , 10 , 25 , 15 , RED);
                DrawText(TextFormat("Grid Distance: %f" , QMEditorCore.MDistance) , 10 , 40 , 15 , RED);

                if (IsTextureReady(QMapSystem.QMapTextures[QMEditorCore.EditorTextureID])){
                    DrawTexturePro(QMapSystem.QMapTextures[QMEditorCore.EditorTextureID] , {0 , 0 , QMapSystem.QMapTextures[QMEditorCore.EditorTextureID].width , QMapSystem.QMapTextures[QMEditorCore.EditorTextureID].height} , { 1280 - 95 , 5 , 90 , 90}, {0 ,0},0.0f, WHITE );
                }

                if (IsKeyPressed(KEY_UP)){
                QMEditorCore.EditorTextureID ++;
                }
                if (IsKeyPressed(KEY_DOWN)){
                QMEditorCore.EditorTextureID --;
                }
            }
            if (QMEditorCore.CurrentMode == 1){
                DrawText("Duo Face Creation", 1280 - 300 , 10 , 15 , WHITE);
                
                DrawText(TextFormat("Current Point: %i" , QMEditorCore.PointCounter) , 10 , 10 , 15 , RED);
                DrawText(TextFormat("Texture ID: %i" , QMEditorCore.EditorTextureID) , 10 , 25 , 15 , RED);
                DrawText(TextFormat("Grid Distance: %f" , QMEditorCore.MDistance) , 10 , 40 , 15 , RED);

                if (IsTextureReady(QMapSystem.QMapTextures[QMEditorCore.EditorTextureID])){
                    DrawTexturePro(QMapSystem.QMapTextures[QMEditorCore.EditorTextureID] , {0 , 0 , QMapSystem.QMapTextures[QMEditorCore.EditorTextureID].width , QMapSystem.QMapTextures[QMEditorCore.EditorTextureID].height} , { 1280 - 95 , 5 , 90 , 90}, {0 ,0},0.0f, WHITE );
                }

                if (IsKeyPressed(KEY_UP)){
                    QMEditorCore.EditorTextureID ++;
                }
                if (IsKeyPressed(KEY_DOWN)){
                    QMEditorCore.EditorTextureID --;
                }
            }

            if (QMEditorCore.CurrentMode == 2){
                DrawText("Face Deletion", 1280 - 300 , 10 , 15 , WHITE);

                if (IsKeyPressed(KEY_LEFT_SHIFT)){
                    if (IsCursorHidden()){
                        EnableCursor();
                        ShowCursor();
                    }
                    else {
                        HideCursor();
                        DisableCursor(); 
                    }
                }
            }

        EndDrawing();
    }

    CloseWindow();

    
}