#include "Misc.hpp"

RenderTexture2D ScenePreview;
Camera3D SceneCamera = {0};


int SteelCameraMode = 3;

wstring WDLData;


#define MaxMapSize 4096 * 2

#define MaxCachedModels MaxMapSize

#define ModelCount 21

static float TerrainHeightMap[MaxMapSize][MaxMapSize];

class Skybox{
    public:

        Model SkyboxModel;
        Shader CubemapShader;

        void InitSkybox(){
            Mesh Cube = GenMeshCube(1.0f, 1.0f, 1.0f);
            SkyboxModel = LoadModelFromMesh(Cube);
            SkyboxModel.materials[0].shader = LoadShader(".OTEData/AppData/Shaders/skybox.vs", ".OTEData/AppData/Shaders/skybox.fs");
            bool UseHDR = false;

            int materialMapCubemap = MATERIAL_MAP_CUBEMAP;
            int doGamma = UseHDR ? 1 : 0;
            int vFlipped = UseHDR ? 1 : 0;

            // Modify the problematic lines
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "environmentMap"), &materialMapCubemap, SHADER_UNIFORM_INT);
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "doGamma"), &doGamma, SHADER_UNIFORM_INT);
            SetShaderValue(SkyboxModel.materials[0].shader, GetShaderLocation(SkyboxModel.materials[0].shader, "vflipped"), &vFlipped, SHADER_UNIFORM_INT);

            CubemapShader = LoadShader(".OTEData/AppData/Shaders/cubemap.vs", ".OTEData/AppData/Shaders/cubemap.fs");

            int equirectangularMapValue = 0;
            SetShaderValue(CubemapShader, GetShaderLocation(CubemapShader, "equirectangularMap"), &equirectangularMapValue, SHADER_UNIFORM_INT);


            Image img = LoadImage(".OTEData/AppData/Shaders/skybox.png");
            SkyboxModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);    // CUBEMAP_LAYOUT_PANORAMA
            UnloadImage(img);
        }

        void Draw(){
            rlDisableBackfaceCulling();
            rlDisableDepthMask();

                DrawModel(SkyboxModel, (Vector3){0, 0, 0}, 1.0f, WHITE);

            rlEnableBackfaceCulling();
            rlEnableDepthMask();
        }
};

static Skybox OTSkybox;


class WDLModels{
    public:
        Model Object;
        Texture2D ModelTexture;
        bool Avaliable;
};

static WDLModels WDLModelData[ModelCount];
static Model HeightMapModel;

Texture2D HeightMapGenImagePreview;
Texture2D HeightMapImagePreview;

class GameData {
  public:
    float X;
    float Y;
    float Z;
    float R;
    float S;
    int ModelId;
    bool Collision;
    bool AlwaysRender;

    void Init() {
        X = 0;
        Y = 0;
        Z = 0;
        R = 0;
        S = 0;
        ModelId = 0;
        Collision = false;
        AlwaysRender = false;
    }
};

static int CachedModelCounter = 0;
static GameData CachedModels[MaxCachedModels];

class CollisionData {
  public:
    float X;
    float Y;
    float Z;
    float W;
    float H;
    float L;

    void Init() {
        X = 0;
        Y = 0;
        Z = 0;
        W = 0;
        H = 0;
        L = 0;
    }
};

static int CachedCollisionCounter = 0;
static CollisionData CachedCollision[MaxCachedModels];

void InitMiniWDL(){
    ScenePreview = LoadRenderTexture(720, 600);
    SceneCamera.position = (Vector3){ 0.2f, 0.4f, 0.2f }; 
    SceneCamera.target = (Vector3){ 0.185f, 0.4f, 0.0f };  
    SceneCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };       
    SceneCamera.fovy = 45.0f;                                
}

string PullAvailableModels() {
    string Base = "";

    for (int i = 1; i <= ModelCount - 1; i++) {
        if (WDLModelData[i].Avaliable) {
            Base += "Model " + to_string(i) + ";";
        }
    }

    if (Base.empty()){
        Base = "No Models;";
    }
    else {
        Base = ReadValue(Base , 0 , Base.size() - 2);
    }

    return Base;
}

static int ScriptTimer = 0;
static float X, Y, Z, S, Rotation, W, H, L;
bool NextCollision = false;
bool NextAlwaysRender = false;
wstring OtherWDLData = L"";

wstring WorkingWDLData = L"";

void ProcessWDL(const char* Path){
    OtherWDLData = L"";
    WorkingWDLData = L"";

    wstring WData = LoadFile(Path);

    WorkingWDLData = WData;

    CachedModelCounter = 0;
    CachedCollisionCounter = 0;

    bool NextCollision = false;

    for (int i = 0; i <= MaxCachedModels - 1; i++) {
        CachedModels[i].Init();
        CachedCollision[i].Init();
    }

    for (int i = 0; i <= GetWDLSize(WData, L""); i++) {
        if (CachedModelCounter == MaxCachedModels)
            break;

        wstring Instruction = WSplitValue(WData, i);

        if (WReadValue(Instruction, 0, 4) == L"Model" || WReadValue(Instruction, 0, 8) == L"HeightMap") {
            if (WReadValue(Instruction, 0, 8) != L"HeightMap") {
                CachedModels[CachedModelCounter].ModelId = int(ToFloat(WReadValue(Instruction, 5, 6)));
            } else {
                CachedModels[CachedModelCounter].ModelId = -1;
            }

            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));

            if (NextCollision) {
                CachedModels[CachedModelCounter].Collision = true;
                NextCollision = false;
            }
            if (NextAlwaysRender) {
                CachedModels[CachedModelCounter].AlwaysRender = true;
                NextAlwaysRender = false;
            }

            CachedModelCounter++;
        }

        if (WReadValue(Instruction, 0, 8) == L"Collision") {
            CachedModels[CachedModelCounter].ModelId = -2;
            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));
        }

        if (WReadValue(Instruction, 0, 11) == L"AdvCollision") {
            CachedCollision[CachedCollisionCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedCollision[CachedCollisionCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedCollision[CachedCollisionCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedCollision[CachedCollisionCounter].W = ToFloat(WSplitValue(WData, i + 6));
            CachedCollision[CachedCollisionCounter].H = ToFloat(WSplitValue(WData, i + 7));
            CachedCollision[CachedCollisionCounter].L = ToFloat(WSplitValue(WData, i + 8));
            CachedCollisionCounter++;
        }

        if (WReadValue(Instruction, 0, 5) == L"Script") {
            CachedModels[CachedModelCounter].ModelId = 200 + int(ToFloat(WReadValue(Instruction, 6, 6)));
            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));
            CachedModelCounter++;
        }

        if (WReadValue(Instruction, 0, 1) == L"NE") {
            OtherWDLData += WSplitValue(WData, i) + L":" + WSplitValue(WData, i + 1) + L":" +
                            WSplitValue(WData, i + 2) + L":" + WSplitValue(WData, i + 3) + L":" +
                            WSplitValue(WData, i + 4) + L":" + WSplitValue(WData, i + 5) + L":";
        }

        if (Instruction == L"C") {
            NextCollision = true;
        }
        if (Instruction == L"PERM") {
            NextAlwaysRender = true;
        }
    }
}

void GenHeights(Image heightmap, Vector3 size) {
#define GRAY_VALUE(c) ((float)(c.r + c.g + c.b) / 3.0f)

    int mapX = size.x;
    int mapZ = size.z;

    for (int z = 0; z < mapX - 1; z++) {
        for (int x = 0; x < mapZ - 1; x++) {
            TerrainHeightMap[z][x] = 0.0f;
        }
    }

    Color *pixels = LoadImageColors(heightmap);

    float Scale = heightmap.width / size.x;

    for (int z = 0; z < (mapZ)-1; z++) {
        for (int x = 0; x < (mapX)-1; x++) {
            float Index = ((x * Scale) + (z * Scale) * heightmap.width);
            TerrainHeightMap[z][x] = GRAY_VALUE(pixels[int(Index)]) / (255 / size.y);
        }
    }
}

void GenTerrain(){
    Image Heights = LoadImage(".OTEData/Working/Models/HeightMap.png");

    int Width = PullConfigValue(".OTEData/Working/Config/HeightMap.conf" , 0);
    int Height = PullConfigValue(".OTEData/Working/Config/HeightMap.conf", 1);
    GenHeights(Heights , {Width , Height , Width });

    Mesh Mesh1 = GenMeshHeightmap(Heights , {Width , Height , Width });

    HeightMapModel = LoadModelFromMesh(Mesh1);
    HeightMapModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = HeightMapImagePreview;
    HeightMapModel.meshes[0] = Mesh1;

}


void LoadData(){
    HeightMapGenImagePreview = LoadTexture(".OTEData/Working/Models/HeightMap.png");
    HeightMapImagePreview = LoadTexture(".OTEData/Working/Models/HeightMapTexture.png");

    for (int i = 1; i <= ModelCount-1; i ++){
        if (IsPathFile(TextFormat(".OTEData/Working/Models/Model%i.obj" , i))){
            WDLModelData[i].Object = LoadModel(TextFormat(".OTEData/Working/Models/Model%i.obj" , i));
            WDLModelData[i].ModelTexture = LoadTexture(TextFormat(".OTEData/Working/Models/Model%iTexture.png" , i));
            SetTextureFilter(WDLModelData[i].ModelTexture, TEXTURE_FILTER_TRILINEAR);
            WDLModelData[i].Object.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModelData[i].ModelTexture;
            WDLModelData[i].Avaliable = true;
        }
        else {
            WDLModelData[i].Avaliable = false;
            cout << "Model " << i << " Does not exists \n";
        }
    }
    
    GenTerrain();
    OTSkybox.InitSkybox();

    ProcessWDL(".OTEData/Working/World.wdl");

}

void DrawWDL(){

    OTSkybox.Draw();


    for (int i = 0; i <= CachedCollisionCounter; i++) {
        X = CachedCollision[i].X;
        Y = CachedCollision[i].Y;
        Z = CachedCollision[i].Z;
        W = CachedCollision[i].W;
        H = CachedCollision[i].H;
        L = CachedCollision[i].L;

        DrawCubeWires((Vector3){X, Y, Z}, W, H, L , RED);
    }


    for (int i = 0; i <= CachedModelCounter; i++) {
        X = CachedModels[i].X;
        Y = CachedModels[i].Y;
        Z = CachedModels[i].Z;
        S = CachedModels[i].S;
        Rotation = CachedModels[i].R;

        if (CachedModels[i].ModelId == -2 || CachedModels[i].ModelId == - 1 || CachedModels[i].ModelId >= 200){
            if (CachedModels[i].ModelId == -2){
                DrawCubeWires((Vector3){X, Y, Z}, S, S , S , RED);
            }
            if (CachedModels[i].ModelId == -1){
                DrawModelEx(HeightMapModel, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, WHITE);
            }
            if (CachedModels[i].ModelId >= 200) {
                DrawCubeWires({X, Y, Z} , S, S, S , YELLOW);
            }
        }
        else {
            DrawModelEx(WDLModelData[CachedModels[i].ModelId].Object , {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, WHITE);
        }

    }
}

bool RenderWindowFocused = false;

float EditorX = 0.0f;
float EditorY = 0.0f;
float EditorZ = 0.0f;

float EditorW = 0.0f;
float EditorH = 0.0f;
float EditorL = 0.0f;

float EditorScale = 0.0f;
float EditorRotation = 0.0f;

int EditorMID = 2;

bool EditorActive = false;

int ModelSelectorIndex = 0;

void UpdateSceneCamera(){
    if (IsKeyDown(KEY_SPACE)){
        for (int i = 0 ; i <= 6 ; i ++){
            UpdateCamera(&SceneCamera , SteelCameraMode);
        }
    }
    else {
        UpdateCamera(&SceneCamera , SteelCameraMode);
    }
}

void RenderWDL(){

    if (RenderWindowFocused){

        BeginTextureMode(ScenePreview);
        ClearBackground(BLACK);

        BeginMode3D(SceneCamera);

        DrawWDL();


        if (SteelCameraMode == 3){
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
                GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 ||
                GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) != 0) {
                SceneCamera.position.y = 8 +
                                                        TerrainHeightMap[int(SceneCamera.position.z)]
                                                                        [int(SceneCamera.position.x)];
            }
        }


        if (IsKeyPressed(KEY_LEFT_SHIFT)){

            EditorX = SceneCamera.position.x;
            EditorY = SceneCamera.position.y;
            EditorZ = SceneCamera.position.z;

            EditorW = SceneCamera.position.x + 5.0f;
            EditorH = SceneCamera.position.y + 5.0f;
            EditorL = SceneCamera.position.z + 5.0f;

            EditorActive = true;
        }

        if (EditorActive){
            
            EditorMID = ModelSelectorIndex;

            if (EditorMID >= 1 && EditorMID <= 20 || EditorMID >= 201 || EditorMID == -2 || EditorMID == -3){
                if (EditorMID >= 1 && EditorMID <= 20)DrawModelEx(WDLModelData[EditorMID].Object , {EditorX , EditorY , EditorZ} , {0 , 1 , 0} , EditorRotation , {EditorScale , EditorScale , EditorScale} , WHITE);
                if (EditorMID >= 201)DrawCubeWires({EditorX , EditorY , EditorZ} , EditorScale , EditorScale , EditorScale , YELLOW);
                if (EditorMID == -2)DrawCubeWires({EditorX , EditorY , EditorZ} , EditorScale , EditorScale , EditorScale , RED);
                if (EditorMID == -3)DrawCubeWires({EditorX , EditorY , EditorZ} , EditorW, EditorH , EditorL , RED);

                if (EditorMID == -3){
                    if (IsMouseButtonDown(1)){
                        EditorW += GetMouseDelta().x / 8;
                        EditorH += GetMouseDelta().y / 8;
                        EditorL -= (GetMouseWheelMove() * 2);
                    }
                }
                else {
                
                

                    if (IsMouseButtonDown(1)){
                        EditorScale -= (GetMouseWheelMove() * .25);

                    }

                    if (IsKeyPressed(KEY_O)) {
                        EditorRotation += 45.0f;
                    }

                    if (IsKeyPressed(KEY_L)) {
                        EditorRotation -= 45.0f;
                    }
                }

                if (IsMouseButtonDown(0)){
                    EditorX += GetMouseDelta().x / 8;
                    EditorY += GetMouseDelta().y / 8;
                    EditorZ -= (GetMouseWheelMove() * 2);
                }
                

                if (!IsMouseButtonDown(1) && !IsMouseButtonDown(0)){
                    UpdateSceneCamera();
                }

                if (IsKeyPressed(KEY_ENTER)){
                    if (EditorMID >= 1 && EditorMID <= 20){
                        WorkingWDLData += L"Model"+to_wstring(EditorMID) + L":" + to_wstring(EditorX) + L":" + to_wstring(EditorY) + L":" +
                                to_wstring(EditorZ) + L":" + to_wstring(EditorScale) + L":" +
                                to_wstring(EditorRotation) + L":";
                    }
                    if (EditorMID >= 201){
                                        WorkingWDLData += L"Script"+to_wstring(EditorMID - 200) + L":" + to_wstring(EditorX) + L":" + to_wstring(EditorY) + L":" +
                        to_wstring(EditorZ) + L":" + to_wstring(EditorScale) + L":" +
                        to_wstring(EditorRotation) + L":";
                    }
                    if (EditorMID == -2){
                                        WorkingWDLData += L"Collision"+to_wstring(EditorMID - 200) + L":" + to_wstring(EditorX) + L":" + to_wstring(EditorY) + L":" +
                        to_wstring(EditorZ) + L":" + to_wstring(EditorScale) + L":" +
                        to_wstring(EditorRotation) + L":";
                    }

                    if (EditorMID == -3){
                        WorkingWDLData += L"AdvCollision:" + to_wstring(EditorX) + L":" + to_wstring(EditorY) + L":" +
                        to_wstring(EditorZ) + L":" + to_wstring(W) + L":" +
                        to_wstring(H) + L":" + to_wstring(L) + L":";
                    }

                    EditorActive = false;

                    wofstream OutFile;
                    OutFile.open(".OTEData/Working/World.wdl");
                    OutFile << WorkingWDLData;
                    OutFile.close();

                    ProcessWDL(".OTEData/Working/World.wdl");
                }

            }
            

        }
        else {
            UpdateSceneCamera();
        }

        EndMode3D();

        if (EditorActive){

            if (EditorMID >= 1 && EditorMID <= 20 || EditorMID >= 201 || EditorMID == -2){

                if (IsMouseButtonDown(1)){
                    DrawText(TextFormat("Object Scale: %i" , int(EditorScale) ) , 5 , 5, 10 , WHITE);
                }
                else {
                    DrawText(TextFormat("Cam Pos: %i %i %i" , int(SceneCamera.position.x) , int(SceneCamera.position.y) , int(SceneCamera.position.z)) , 5 , 5, 10 , WHITE);
                }
                

            }
        }

        DrawText(TextFormat("MID: %i" , int(EditorMID) ) , 5 , 15, 10 , WHITE);

        

        
        EndTextureMode();

        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_G)){
            ShowCursor();
            EnableCursor();
            FullscreenMode = false;
            RenderWindowFocused = false;
        }

    }
    else {
    }
    
}
