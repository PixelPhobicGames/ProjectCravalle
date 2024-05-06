

#include <iostream>
#include <string>
#include <sstream>

#include <fstream>
#include <stdlib.h>
#include <cstring>

#include "../../TextSystem.hpp"
#include "../Language/Language.hpp"
#include "../../CineFlow/CineFlow.hpp"

using namespace std;

#define MaxVaribles 100 
#define MaxTextures 25
#define MaxSounds 25
#define MaxPermanentVaribles 40
#define MaxJumpPoints 45
#define MaxArraySize 1
#define MaxTFlag 100
#define MaxArrays 1

static int VirtualMouseX = 0;
static int VirtualMouseY = 0;

static int SceneIDMirror = 0;

static bool MapClipping = true;
static bool UsingClipBox = false;
static int PlatformHeight = 0;
static bool FoundPlatform = false;
static float OriginY = 0;
static bool ParasiteRunning = false;

static bool CodeLogging = false;

static int VaribleCounter = 0;
static int TextureCounter = 0;
static int SFXCounter = 0;
static int PMemCounter = 0;
static int JumpPointCounter = 0;
static int ArrayCounter = 0;

static wstring ExtraWDLInstructions = L"";

static bool SetSceneFlag = false;
static int SetSceneId = 0;

static bool SetCameraFlag = false;
static Vector3 SetCameraPos = {0,0,0};

static bool SetSunFlag = false;
static Vector3 SunPos = {0,0,0};

static bool ScriptCollisionMessage = false;

static bool GrassEnabled = false;

static RenderTexture ParasiteTarget;
static bool UsingParasiteDraw = false;


typedef struct Memory{
    string Name;
    string Value;
    int IValue;
}Memory;


static Memory VaribleMemory[MaxVaribles];

typedef struct PMemory{
    string Name;
    int IValue;
}PMemory;

static PMemory PermanentMemory[MaxPermanentVaribles];

typedef struct TexMemory{
    string Name;
    Texture2D TextureValue;
}TexMemory;

static TexMemory TextureMemory[MaxTextures];


typedef struct SFXMemory{
    string Name;
    Sound SoundValue;
}SFXMemory;

static SFXMemory SoundMemory[MaxSounds];

typedef struct ArrMemory{
    string Name;
    int Array[MaxArraySize];
    int Size;
}ArrMemory;


static ArrMemory ArrayMemory[MaxArrays];

typedef struct JumpPoint{
    string Name;
    int LineNumber;
}JumpPoint;

static JumpPoint JumpPoints[MaxJumpPoints];

typedef struct Flags{
    bool Value;
}Flags;

static Flags ToggleFlags[MaxTFlag];


typedef struct ParasiteScriptData{
   istringstream ProgramData;
   
   string Line[1000];
   
   int LineCounter;
   int ProgramSize;

   bool CompareFlag;

   bool EqualFlag;
   bool GreaterFlag;
   bool LesserFlag;
   bool NotEqualFlag;
   bool ContinueFlag;
   bool ElseFlag;
   bool ErrorFlag;

   int ReturnLine;
   int TextSize;
   

}ParasiteScriptData;

static ParasiteScriptData ParasiteScriptCoreData;

static Pack GlobalPackData;



