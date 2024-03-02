
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <utility>
#include "raylib.h"

using namespace std;

wstring LoadFile(const char *Path)
{
    wstring Data;
    wifstream OutFile;
    OutFile.open(Path);
    OutFile >> Data;
    return Data;
}

float ToFloat(wstring Data)
{
    string s(Data.begin(), Data.end());
    float Out = stof(s);
    return Out;
}


int GetWDLSize(wstring WData , wstring Extra)
{
    int Out = 0;
    wstring Data = WData + Extra;
    for (int i = 0; i <= WData.size() + Extra.size(); i++)
    {
        if (Data[i] == ':')
        {
            Out++;
        }
    }
    return Out;
}



// Weird Optimizations
static wstring ReadValueOut = L"";

auto WReadValue(wstring Data, int Start, int End)
{ // Reads Value from (Start to End)
    ReadValueOut = L"";
    for (int i = Start; i <= End; i++)
    {
        ReadValueOut += Data[i];
    }
    return ReadValueOut;
}

static int SVPlaceCounter = 0;
static int SVStart = 0;
static int SVEnd = 0;

auto WSplitValue(wstring Data, int Place)
{
    SVPlaceCounter = 0;
    SVStart = 0;
    SVEnd = 0;

    if (Place == 0)
    {
        for (int i = 0; i <= Data.size(); i++)
        {
            if (Data[i] == L':' || i == Data.size())
            {
                SVEnd = i - 1;
                break;
            }
        }
    }
    else
    {
        Place = Place;
        SVPlaceCounter = 1;
        for (int i = 0; i <= Data.size(); i++)
        {
            if (SVPlaceCounter == Place)
            {
                SVStart = i + 1;
                for (int x = i + 1; x <= Data.size(); x++)
                {
                    if (Data[x] == L':' || x == Data.size())
                    {
                        SVEnd = x - 1;
                        break;
                    }
                }
            }
            if (Data[i] == L':')
            {
                SVPlaceCounter++;
            }
        }
    }

    return WReadValue(Data, SVStart, SVEnd);
}

int PullConfigValue(const char *Path, int ValueIndex) {
    ifstream configFile(Path);

    string line;
    getline(configFile, line);

    configFile.close();

    stringstream ss(line);
    string token;
    int index = 0;
    while (getline(ss, token, '|')) {
        if (index == ValueIndex) {
            try {
                int intValue = stoi(token);
                return intValue;
            } catch (const invalid_argument &ia) {
                cerr << "Error: InvalidIntegerValueInTheConfigurationFile" << endl;
                return -1;
            }
        }
        index++;
    }

    cerr << "Error: ValueNotFoundAtIndex" << ValueIndex << endl;
    return -1;
}

bool GetCollision(int x, int y , int w, int h, int x2 , int y2 , int w2 , int h2){
    if( y+h <= y2 )
    {
        return false;
    }

    if( y >= y2+h2 )
    {
        return false;
    }

    if( x+w <= x2 )
    {
        return false;
    }

    if( x >= x2+w2 )
    {
        return false;
    }

    return true;

}

class Input{
    public:
        bool InteractPressed;
        bool InteractDown;
        bool TextButton;

        bool ControllerUsed = false;

        void UpdateInputs(){

            if (IsGamepadAvailable(0)){
                ControllerUsed = true;
            }

            if (GetMouseDelta().x != 0 || GetMouseDelta().y != 0){
                ControllerUsed = false;
            }

            if (ControllerUsed){
                if (IsGamepadButtonDown(0 , GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)){
                    InteractDown = true;
                }
                else {
                    InteractDown = false;
                }
                if (IsGamepadButtonPressed(0 , GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)){
                    InteractPressed = true;
                }
                else {
                    InteractPressed = false;
                }
                if (IsGamepadButtonPressed(0 , GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
                    TextButton = true;
                }
                else {
                    TextButton = false;
                }
            }
            else {
                if (IsMouseButtonDown(0)){
                    InteractDown = true;
                }
                else {
                    InteractDown = false;
                }

                if (IsMouseButtonPressed(0)){
                    InteractPressed = true;
                }
                else {
                    InteractPressed = false;
                }

                if (IsKeyPressed(KEY_SPACE)){
                    TextButton = true;
                }
                else {
                    TextButton = false;
                }
            }
        }
};

static Input OmegaInputController;

#ifdef __linux__

void Rumble(int duration){
    
}

#endif
