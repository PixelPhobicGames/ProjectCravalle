#include "Core.hpp"

/* OT:2: PixelPhobicGames 2023 */

int main() {
    
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(1280, 720, "OT2");
    SetTargetFPS(60);

    InitAudioDevice();

    if (!IsAudioDeviceReady()) {
        CloseAudioDevice();
    }

    // PlayLogo(); // Turned off by default

    OmegaTechInit();
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
        }
        else {
        #endif

            if (!ConsoleToggle) {
                if (UsingCineFlow) {
                    UpdateCamera(&CineFlowData.CFCamera, CAMERA_FREE);
                    UpdateCineFlow();
                } else {
                    OmegaPlayer.OldX = OmegaTechData.MainCamera.position.x;
                    OmegaPlayer.OldY = OmegaTechData.MainCamera.position.y;
                    OmegaPlayer.OldZ = OmegaTechData.MainCamera.position.z;

                    OmegaTechData.CameraSpeed = 1;

                    if (IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0 , GAMEPAD_BUTTON_RIGHT_FACE_DOWN))OmegaTechData.CameraSpeed = 2;

                    if (!OmegaInputController.InteractDown) {
                        for (int i = 0; i <= OmegaTechData.CameraSpeed; i++) {
                            UpdateCamera(&OmegaTechData.MainCamera, CAMERA_FIRST_PERSON);
                        }
                    }

                    OmegaInputController.UpdateInputs();
                }
            }

            UpdateFade();
            UpdateLightSources();
            DrawWorld();
            BeginDrawing();

            ClearBackground(BLACK);


                BeginShaderMode(OmegaTechData.Bloom);
                DrawTexturePro(Target.texture,
                            (Rectangle){0, 0, Target.texture.width, -Target.texture.height},
                            (Rectangle){0, 0, float(GetScreenWidth()), float(GetScreenHeight())},
                            (Vector2){0, 0},
                            0.f,
                            FadeColor);

                if (ScriptCollisionMessage) {
                    DisplayInteractText();
                }

                if (OmegaTechData.Ticker == 33 || OmegaInputController.InteractPressed || OmegaInputController.TextButton) {
                    ScriptCollisionMessage = false;
                }

                EndShaderMode();

                if (!UsingCineFlow) {
                    if (UIToggle)UpdateUI();
                    UpdateCustomUI(OmegaTechData.LevelIndex);
                }

                if (IsKeyPressed(KEY_TAB)){
                    if (UIToggle){
                        UIToggle = false;
                    }
                    else {
                        UIToggle = true;
                    }
                }

                if (FPSEnabled) {
                    DrawFPS(0, 0);
                }

                if (!UsingCineFlow)
                    OmegaTechTextSystem.Update();

                if (ConsoleToggle) {
                    DrawConsole();
                }
            
                EndDrawing();

        #ifdef UseLumina
        }
        #endif
        

        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();

        if (IsWindowResized()){
            SetWindowSize(1280 , 720);
        }

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

        SceneIDMirror = OmegaTechData.LevelIndex;
    }

    CloseWindow();
}