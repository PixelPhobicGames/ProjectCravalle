
#include <cmath>
#include <algorithm>
#include <vector>
#include <cctype>
#include <cstring>


#include "Style.h"
#include "MiniWDL.hpp"

#define MAX_FILEPATH_RECORDED   4096
#define MAX_FILEPATH_SIZE       2048

int main(){
	InitWindow(1104, 720, "Steel Editor");
	SetTargetFPS(60);

	InitMiniWDL();

	bool ProjectButton = false;
    bool SaveButton = false;
    bool UndoButton = false;
    bool FullScreenToggle = false;
    bool FPToggle = true;
    bool FreeToggle = true;

	bool ScriptToggle = false;
	bool CollisionToggle = false;


	bool TerrainToggle = false;

	bool ProjectOptionsToggle = false;
	bool DataExportToggle = false;

	bool TerrainGenToggle = false;
    bool ValueBOx003EditMode = false;
    int ValueBOx003Value = 0;

    bool DENameEditMode = false;
    char DEName[128] = "Name";
    float DEProgress = 0.0f;
	
	bool ModelLoaderToggle = false;
	bool ModelLoaderEditMode = false;
    int ModelLoaderIndex = 0;

	string MLTexturePath;
	string MLModelPath;

	int filePathCounter = 0;
    char *filePaths[MAX_FILEPATH_RECORDED] = { 0 }; 

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++)
    {
        filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
    }

    bool TWidthMode = false;
    int TWidthValue = 800;
    bool THeightMode = false;
    int THeightValue = 32;
	
    int ListView014Active = -1;
    bool ScriptButton = false;
    bool CollisionButton = false;

	int Alert = 0;
	string AlertText = "";

	if (IsFolderEmpty(".OTEData/Working/")){
		SetupNewProject();
		Alert = 120;
		AlertText = "New Data Created. Past Project Not Found";
	}

	InitTG();

	LoadData();

	while (!WindowShouldClose())
	{
		RenderWDL();
		BeginDrawing();

		ClearBackground(BLACK);

		if (!FullscreenMode){
			GuiPanel((Rectangle){ 0, 0, 1104, 720 }, NULL);

			if (GuiButton((Rectangle){ 8, 0, 96, 24 }, "Project")){
				if (ProjectOptionsToggle)ProjectOptionsToggle = false;
				else {
					ProjectOptionsToggle = true;
				}
			}

			if (GuiButton((Rectangle){ 112, 0, 120, 24 }, "Save" ) || IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)){
				cout << "Saving \n";

				wofstream OutFile;
				OutFile.open(".OTEData/Working/World.wdl");
				OutFile << WorkingWDLData;
				OutFile.close();
			}

			if (GuiButton((Rectangle){ 240, 0, 120, 24 }, "Undo")){
				LoadData();
			}

			if (GuiButton((Rectangle){ 496, 0, 120, 24 }, "Scripts")){
				if (ScriptToggle)ScriptToggle = false;
				else {
					ScriptToggle = true;
				}
			}

			if (GuiButton((Rectangle){ 624, 0, 120, 24 }, "Collision Objects")){
				if (CollisionToggle)CollisionToggle = false;
				else {
					CollisionToggle = true;
				}
			}

			if (GuiButton((Rectangle){ 624 + 128, 0, 120, 24 }, "Help")){
				OpenURL(".OTEData/AppData/EditorHelp.pdf");
			}

			if (GuiButton((Rectangle){ 368, 0, 120, 24 }, "Terrain")){
				if (TerrainToggle)TerrainToggle = false;
				else { TerrainToggle = true;}
			}

			GuiPanel((Rectangle){ 24, 72, 168, 600 }, NULL);
			GuiLabel((Rectangle){ 24, 48, 120, 24 }, "Model List");
			GuiLabel((Rectangle){ 40, 648, 152, 24 }, "Press Tab to import");

			ModelSelectorIndex = GuiModelSelector(PullAvailableModels());

			if (IsKeyPressed(KEY_TAB)){
				ModelLoaderToggle = true;
			}                      



			// Scene Preview
			GuiLabel((Rectangle){ 288, 48, 120, 24 }, "Scene -(Shft - G) Exit");

			GuiPanel((Rectangle){ 288, 72, 720, 600 }, NULL);

			if(GuiButton((Rectangle){ 288, 72, 720, 600 }, "Click to Start Scene") && !TerrainToggle && !ModelLoaderToggle && !ProjectOptionsToggle && !ScriptToggle && !CollisionToggle){
				HideCursor();
				DisableCursor();
				RenderWindowFocused = true;
			}

			DrawTexturePro(ScenePreview.texture , {0 , 0 , ScenePreview.texture.width , -ScenePreview.texture.height} , {288, 72 , 720, 600} , {0, 0} , 0.0f , WHITE);


			if (GuiButton((Rectangle){ 296 - 8, 648 + 26, 88, 24 }, "First Person")){
				SteelCameraMode = 3;
			}

			if (GuiButton((Rectangle){ 392 - 8, 648 + 26, 88, 24 }, "Free Mode")){
				SteelCameraMode = 1;
			}

			if (GuiButton((Rectangle){ 912 + 100, 648, 88, 24 }, "FullScreen")){
				if (FullscreenMode == false){
					HideCursor();
					DisableCursor();
					RenderWindowFocused = true;
					FullscreenMode = true;
				}
				else {
					FullscreenMode = false;
				}
			}

			if (ModelLoaderToggle && !TerrainToggle && !ProjectOptionsToggle){
				if (GuiWindowBox((Rectangle){ 216, 96, 528, 368 }, "Model Loader")){
					ModelLoaderToggle = false;
				}

				if (GuiSpinner((Rectangle){ 608, 384, 120, 24 }, "Model Index", &ModelLoaderIndex, 1, 20, ModelLoaderEditMode)) ModelLoaderEditMode = !ModelLoaderEditMode;
				GuiPanel((Rectangle){ 240, 144, 488, 216 }, NULL);

				for (int i = 0; i <= filePathCounter; i++){
					string Path = filePaths[i];
					if (SearchString(Path , "png")){
						DrawText(filePaths[i] , 248, 260 , 8, RED);
						MLTexturePath = Path;
					}
					if (SearchString(Path , "obj")){
						DrawText(filePaths[i] , 248, 188 , 8, RED);
						MLModelPath = Path;
					}
				}

				GuiLabel((Rectangle){ 248, 240, 248 - 216 + 528, 24 }, "Texture Path:");
				GuiLabel((Rectangle){ 248, 168, 120, 24 }, "Model Path:");
				GuiPanel((Rectangle){ 240, 368, 288, 88 }, NULL);

				if (GuiButton((Rectangle){ 608, 424, 120, 24 }, "Load Object")){
					#ifdef __linux__
						system(TextFormat("echo "" >> .OTEData/Working/Models/Model%i.obj" , ModelLoaderIndex));
						system(TextFormat("echo "" >> .OTEData/Working/Models/Model%iTexture.png" , ModelLoaderIndex));
						system(TextFormat("mv '%s' .OTEData/Working/Models/Model%i.obj" , MLModelPath.c_str() , ModelLoaderIndex));
						system(TextFormat("mv '%s' .OTEData/Working/Models/Model%iTexture.png" , MLTexturePath.c_str() , ModelLoaderIndex));
					#elif _WIN32
						system(TextFormat("echo. >> .OTEData\\Working\\Models\\Model%i.obj" , ModelLoaderIndex));
						system(TextFormat("echo. >> .OTEData\\Working\\Models\\Model%iTexture.png" , ModelLoaderIndex));
						system(TextFormat("move \"%s\" .OTEData\\Working\\Models\\Model%i.obj" , MLModelPath.c_str() , ModelLoaderIndex));
						system(TextFormat("move \"%s\" .OTEData\\Working\\Models\\Model%iTexture.png" , MLTexturePath.c_str() , ModelLoaderIndex));
					#endif

					MLTexturePath = "";
					MLModelPath = "";

					filePathCounter = 0;
					filePaths[MAX_FILEPATH_RECORDED] = { 0 }; 

					for (int i = 0; i < MAX_FILEPATH_RECORDED; i++)
					{
						filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
					}

					LoadData();
					ModelLoaderToggle = false;
				}

				GuiLabel((Rectangle){ 248, 384, 280, 24 }, "Note: You must drop Models and Textures ");
				GuiLabel((Rectangle){ 248, 408, 280, 16 }, "Seperatly, you can use PNG files and OBJ Files");

				if (IsFileDropped())
				{
					FilePathList droppedFiles = LoadDroppedFiles();

					for (int i = 0, offset = filePathCounter; i < (int)droppedFiles.count; i++)
					{
						if (filePathCounter < (MAX_FILEPATH_RECORDED - 1))
						{
							TextCopy(filePaths[offset + i], droppedFiles.paths[i]);
							filePathCounter++;

							cout << droppedFiles.paths[i] << "\n";
						}
					}

					UnloadDroppedFiles(droppedFiles); 
				}

			}

			if (TerrainToggle && !ProjectOptionsToggle ){
				GuiPanel((Rectangle){ 416, 24, 304, 360 }, NULL);

				if (GuiButton((Rectangle){ 592, 352, 120, 24 }, "Generate Terrain")){
					wstring ConfData = L"";
					ConfData += to_wstring(TWidthValue);
					ConfData += L"|";
					ConfData += to_wstring(THeightValue);
					ConfData += L"|";

					wofstream OutFile;
					OutFile.open(".OTEData/Working/Config/HeightMap.conf");
					OutFile << ConfData;
					OutFile.close();

					GenTerrain();

				}

				GuiPanel((Rectangle){ 432, 72, 120, 96 }, NULL);
				DrawTexturePro(HeightMapGenImagePreview , {0 , 0 , HeightMapGenImagePreview.width , HeightMapGenImagePreview.height} , {432, 72, 120, 96} , {0,0} , 0.0f , WHITE);
				GuiLabel((Rectangle){ 576, 112, 120, 24 }, "Height Map Image");

				GuiPanel((Rectangle){ 432, 192, 120, 96 }, NULL);
				DrawTexturePro(HeightMapImagePreview , {0 , 0 , HeightMapImagePreview.width , HeightMapImagePreview.height} , {432, 192, 120, 96} , {0,0} , 0.0f , WHITE);
				GuiLabel((Rectangle){ 576, 232, 120, 24 }, "Applied Texture");

				GuiLabel((Rectangle){ 432, 168, 120, 24 }, "Drag & Drop");
				GuiLabel((Rectangle){ 512, 40, 104, 24 }, "Terrain Generator");
				GuiPanel((Rectangle){ 432, 312, 144, 72 }, NULL);
				if (GuiValueBox((Rectangle){ 528, 320, 40, 24 }, "Width", &TWidthValue, 1, 1000, TWidthMode)) TWidthMode = !TWidthMode;
				if (GuiValueBox((Rectangle){ 528, 352, 40, 24 }, "Height", &THeightValue, 1, 1000, THeightMode)) THeightMode = !THeightMode;
				GuiLine((Rectangle){ 416, 56, 304, 16 }, NULL);
			}

			if (ScriptToggle && !CollisionToggle){
				if (GuiWindowBox((Rectangle){ 496, 24, 208, 464 }, "Scripts")){
					ScriptToggle = false;
				}

                GuiPanel((Rectangle){ 504, 56, 192, 424 }, NULL);

				for (int i = 1 ; i <= 10 ; i ++){
					if (GuiButton((Rectangle){ 512, 64 + 5 + (i * 24), 176, 24 }, TextFormat("Script %i" , i))){
						SelectorID = 200 + i;
					}
				}

			}

			if (CollisionToggle){
				if (GuiWindowBox((Rectangle){ 624, 24, 200, 136 }, "Collision")){
					CollisionToggle = false;
				}
            	if (GuiButton((Rectangle){ 632, 64, 176, 24 }, "Simple Collision"))SelectorID = -2;
                if (GuiButton((Rectangle){ 632, 120, 176, 24 }, "Advanced Collision"))SelectorID = -3;
			}


			if (ProjectOptionsToggle) {
				if (GuiWindowBox((Rectangle){ 48, 24, 336, 416 }, "Project Options")){
					ProjectOptionsToggle = false;
				}

				if (GuiButton((Rectangle){ 72, 64, 288, 32 }, "Create New Project")){

					#ifdef __linux__ 
						system("rm -r .OTEData/Working");
						system("mkdir .OTEData/Working ");
					#elif _WIN32
						system("rmdir /s /q .OTEData\\Working");
						system("mkdir .OTEData\\Working ");
					#endif

					SetupNewProject();
					LoadData();
				}

                if (GuiButton((Rectangle){ 72, 112, 288, 32 }, "Export")){
					if (DataExportToggle)DataExportToggle= false;
					else {
						DataExportToggle = true;
					}
				}

                if (GuiButton((Rectangle){ 72, 112, 288, 32 }, "Export")){
					if (DataExportToggle)DataExportToggle= false;
					else {
						DataExportToggle = true;
					}
				}

                if (GuiButton((Rectangle){ 72, 160, 288, 32 }, "Terrain Gen")){
					if (TerrainGenToggle)TerrainGenToggle= false;
					else {
						TerrainGenToggle = true;
					}
				}

				if (TerrainGenToggle){
					if (GuiWindowBox((Rectangle){ 312, 144, 504, 264 }, "Terrain Generator")){
						TerrainGenToggle = false;
					}
					
					GuiLabel((Rectangle){ 336, 168, 120, 24 }, "Noise Map");

					if (GuiValueBox((Rectangle){ 672, 336, 120, 24 }, "Noise Value (0 - 1000)", &ValueBOx003Value, 0, 1000, ValueBOx003EditMode)){
						ValueBOx003EditMode = !ValueBOx003EditMode;
						float Value = ValueBOx003Value / 100.0f;

						CreateTerrain(Value);
					}

					if (GuiButton((Rectangle){ 672, 368, 120, 24 }, "Save")){
						ExportImage(TerrainGeneration, ".OTEData/Working/Models/HeightMap.png"); 
					}

					GuiPanel((Rectangle){ 336, 192, 192, 168 }, NULL);

					DrawTexturePro(TerrainGenerationTexture , {0 , 0 , TerrainGenerationTexture.width , TerrainGenerationTexture.height } , {336, 192, 192, 168} , {0,0} , 0.0f , WHITE);


				}


				if (DataExportToggle && !TerrainGenToggle){
					if (GuiWindowBox((Rectangle){ 448, 192, 400, 224 }, "Data Export")){
						DataExportToggle = false;
					}

					if (GuiTextBox((Rectangle){ 456, 248, 384, 24 }, DEName, 128, DENameEditMode)) DENameEditMode = !DENameEditMode;
					GuiLabel((Rectangle){ 456, 224, 120, 24 }, "Project Name");
					GuiProgressBar((Rectangle){ 464, 376, 120, 16 }, NULL, NULL, DEProgress, 0, 1);

					if (GuiButton((Rectangle){ 712, 376, 120, 24 }, "Export")){
						#ifdef __linux__ 
							system("mkdir Exports");
							system(TextFormat("mkdir Exports/%s/" , DEName));
							system(TextFormat("cp -r .OTEData/Working/* Exports/%s/ " , DEName));
							DEProgress = 1.0f;
						#elif _WIN32
							system("mkdir Exports");
							system(TextFormat("mkdir Exports\\%s\\", DEName));
							system(TextFormat("xcopy /s .OTEData\\Working\\* Exports\\%s\\", DEName));
							DEProgress = 1.0f;
						#endif
					}
				}
			}


			if (Alert != 0){
				GuiWindowBox((Rectangle){ 336, 136, 552, 224 }, "System Alert");
				GuiLabel((Rectangle){ 512, 160, 384, 200 }, AlertText.c_str());
				Alert --;
			}
		}

		if (FullscreenMode){
			DrawTexturePro(ScenePreview.texture , {0 , 0 , ScenePreview.texture.width , -ScenePreview.texture.height} , {0, 0 , 1104, 720} , {0, 0} , 0.0f , WHITE);
		}


		EndDrawing();
	}

	CloseWindow();
}