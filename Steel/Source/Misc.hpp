#include "raylib.h"
#include "rlgl.h"
#include "PPGIO/PPGIO.hpp"

#include <string>
#include <filesystem>



using namespace std;


bool FullscreenMode = false;


int SelectorID = 1;



Image WorkingHMData;
Texture WorkingHM;
int WHMWidth;

Image ColourMask;


Color clampColor(Color c) {
    c.r = (c.r > 255) ? 255 : (c.r < 0) ? 0 : c.r;
    c.g = (c.g > 255) ? 255 : (c.g < 0) ? 0 : c.g;
    c.b = (c.b > 255) ? 255 : (c.b < 0) ? 0 : c.b;
    return c;
}

Image addImagesColor(Image dest, Image src) {
    Color *destpixels = LoadImageColors(dest);
    Color *srcpixels = LoadImageColors(src);

    int w = dest.width;

    Image Out = GenImageColor(w, w, BLACK);

    // Iterate over each pixel in the images
    for (int x = 0; x <= w - 1; x++) {
        for (int y = 0; y <= w - 1; y++) {
            // Add the color values together
            Color destPixel = destpixels[y * w + x];
            Color srcPixel = srcpixels[y * w + x];


            Color currentPixel;
            int Red = abs(destPixel.r + srcPixel.r);
            int Blue = abs(destPixel.g + srcPixel.g);
            int Green = abs(destPixel.b + srcPixel.b);

            currentPixel.r = Red;
            currentPixel.g = Green;
            currentPixel.b = Blue;
            currentPixel.a = 255;

            currentPixel = clampColor(currentPixel);

            // Draw pixel onto output image
            ImageDrawPixel(&Out, x, y, currentPixel);
        }
    }

    UnloadImageColors(destpixels);
    UnloadImageColors(srcpixels);

    return Out;
}


void InitHMEditor(){
    WHMWidth = PullConfigValue(".OTEData/Working/Config/HeightMap.conf" , 0);
    ColourMask = GenImageColor(WHMWidth , WHMWidth , BLACK);
    WorkingHMData = LoadImage(".OTEData/Working/Models/HeightMap.png");
    WorkingHM = LoadTextureFromImage(WorkingHMData);

    
}

Image TerrainGeneration;
Texture2D TerrainGenerationTexture;


void InitTG(){
    int Width = PullConfigValue(".OTEData/Working/Config/HeightMap.conf" , 0);

    TerrainGeneration = GenImageColor(Width , Width , BLACK);
    TerrainGenerationTexture = LoadTextureFromImage(TerrainGeneration);
}

int Depth = 0;

void CreateTerrain(float Value){
    int Width = PullConfigValue(".OTEData/Working/Config/HeightMap.conf" , 0);

    TerrainGeneration = GenImagePerlinNoise(Width, Width, GetRandomValue(0 , 1000), GetRandomValue(0 , 1000), Value); 

    ImageColorBrightness(&TerrainGeneration, Depth);

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

