#include "raylib.h"
#include "PPGIO/PPGIO.hpp"

#include <string>
#include <filesystem>



using namespace std;


bool FullscreenMode = false;


int SelectorID = 1;

Image TerrainGeneration;
Texture2D TerrainGenerationTexture;


void InitTG(){

    int Width = PullConfigValue(".OTEData/Working/Config/HeightMap.conf" , 0);

    TerrainGeneration = GenImageColor(Width , Width , BLACK);
    TerrainGenerationTexture = LoadTextureFromImage(TerrainGeneration);

}

void CreateTerrain(float Value){
    int Width = PullConfigValue(".OTEData/Working/Config/HeightMap.conf" , 0);
    TerrainGeneration = GenImagePerlinNoise(Width , Width , GetRandomValue(0 , 1000), GetRandomValue(0 , 1000), Value); 

    TerrainGenerationTexture = LoadTextureFromImage(TerrainGeneration);
}


int CountSemicolons(const std::string& inputString) {
    int count = 0;
    for (char ch : inputString) {
        if (ch == ';') {
            count++;
        }
    }
    return count;
}

int FindSingleNumber(const std::string& inputString) {
    int result = 0;
    bool foundDigit = false;

    for (char ch : inputString) {
        if (isdigit(ch)) {
            result = result * 10 + (ch - '0');
            foundDigit = true;
        } else {
            if (foundDigit) {
                break;
            }
        }
    }

    return result;
}

int GuiModelSelector(string List){
    GuiPanel((Rectangle){ 32, 80, 136, 560 }, NULL);

    for (int i = 0; i <= CountSemicolons(List); i ++){
        if (GuiButton({32, 80 + (i + 5)+ i * 30, 136, 30} , SplitValueEx(List , ';' , i).c_str())){
            SelectorID = FindSingleNumber(SplitValueEx(List , ';' , i));
        }
    }

    return SelectorID;
}


void SetupNewProject(){
    #ifdef __linux__
        system("cp -r .OTEData/BaseProject/* .OTEData/Working");
    #elif _WIN32
        system("xcopy /s .OTEData\\BaseProject\\* .OTEData\\Working\\");
    #endif
}



bool IsFolderEmpty(const std::string& path) {
    for (const auto& entry : filesystem::directory_iterator(path)) {
        return false;
    }
    return true;
}

bool SearchString(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

