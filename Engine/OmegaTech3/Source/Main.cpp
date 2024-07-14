#include "Core.hpp"

/* OT 3: PixelPhobicGames 2024*/

int main() {

    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(1280, 720, "OT3");
    SetTargetFPS(60);

    InitAudioDevice();

    if (!IsAudioDeviceReady()) {
        CloseAudioDevice();
    }

    // PlayLogo(); // Turned off by default

    OTInit();
    PlayHomeScreen();

    LoadWorld();

    HideCursor();
    DisableCursor();

    while (!WindowShouldClose()) {

#ifdef UseLumina

        if (!UseLumina) {
            LuminaDrawWorld();

            BeginDrawing();

            ClearBackground(BLACK);

            DrawTexturePro(LuminaTarget.texture,
                           (Rectangle){0, 0, LuminaTarget.texture.width, -LuminaTarget.texture.height},
                           (Rectangle){0, 0, float(GetScreenWidth()), float(GetScreenHeight())},
                           (Vector2){0, 0},
                           0.f,
                           FadeColor);

            EndDrawing();

            if (ExitLuminaFlag) {
                UseLumina = false;
                OmegaTechData.LevelIndex++;
                LoadWorld();
                ExitLuminaFlag = false;
            }
        } else {
#endif

            if (!ConsoleToggle) {
                if (UsingCineFlow) {
                    UpdateCamera(&CineFlowData.CFCamera, CAMERA_FREE);
                    UpdateCineFlow();
                } else {
                    OTPlayer.OldX = OTCoreData.RenderCamera.position.x;
                    OTPlayer.OldY = OTCoreData.RenderCamera.position.y;
                    OTPlayer.OldZ = OTCoreData.RenderCamera.position.z;

                    OTCoreData.CameraSpeed = 1;

                    if (IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                        OTCoreData.CameraSpeed = 2;

                    if (!OTInputController.InteractDown) {
                        for (int i = 0; i <= OTCoreData.CameraSpeed; i++) {
                            UpdateCamera(&OTCoreData.RenderCamera, CAMERA_FIRST_PERSON);
                        }
                    }

                    OTInputController.UpdateInputs();
                }
            }

            UpdateFade();
            UpdateLightSources();
            DrawWorld();

            BeginDrawing();
            ClearBackground(BLACK);

            DrawTexturePro(OTCoreData.RenderTarget.texture,
                           (Rectangle){0, 0, OTCoreData.RenderTarget.texture.width, -OTCoreData.RenderTarget.texture.height},
                           (Rectangle){0, 0, float(GetScreenWidth()), float(GetScreenHeight())},
                           (Vector2){0, 0},
                           0.f,
                           FadeColor);

            if (!UsingCineFlow) {
                if (UIToggle){
                    UpdateUI();
                    UpdateCustomUI(OTCoreData.LevelIndex);
                }
            }

            EndBlendMode();
            
            if (IsKeyPressed(KEY_TAB)) UIToggle = !UIToggle;
            if (FPSEnabled) DrawFPS(0, 0);
            if (!UsingCineFlow) OTTextSystem.Update();
            if (ConsoleToggle) OTDebugConsole.Draw();
            

            EndDrawing();

#ifdef UseLumina
        }
#endif

        if (IsKeyPressed(KEY_F3)) {
            if (ConsoleToggle) {
                ConsoleToggle = false;
                HideCursor();
                DisableCursor();
            } else {
                ConsoleToggle = true;
                ShowCursor();
                EnableCursor();
            }
        }

        SceneIDMirror = OTCoreData.LevelIndex;
    }

    UnloadGame();

    CloseWindow();
}