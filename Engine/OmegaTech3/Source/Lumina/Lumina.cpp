
#include "Lumina.hpp"

RenderTexture2D LuminaTarget;
bool ExitLuminaFlag;
bool UseLumina;
Lumina LuminaData;

#define SpawnTile 3
#define KillTile 2
#define CollisionTile 1


void ReadCSVFile(string filename, int Array[MaxCollisionSize][MaxCollisionSize], int& rows, int& cols) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening the file!" << endl;
        return;
    }

    string line;
    rows = 0;
    cols = 0;

    while (getline(file, line) && rows < MaxCollisionSize) {
        istringstream ss(line);
        string token;
        cols = 0;

        while (getline(ss, token, ',') && cols < MaxCollisionSize) {
            int value = stoi(token);
            Array[rows][cols] = value;
            cols++;
        }

        rows++;
    }

    file.close();
}

void SetPlayer(){
    for (int x = 0 ; x <= LuminaData.LevelWidth ; x++){
        for (int y = 0 ; y <= LuminaData.LevelHeight ; y++){
            if (LuminaData.Collision[y][x] == 3){
                LuminaData.PlayerY = y * 16;
                LuminaData.PlayerX = x * 16;

            }
        }
    }
}

void InitLumina(const char* Path){
    LuminaTarget = LoadRenderTexture(320 * 2, 180 * 2);
    //LuminaData.SingleFrame1 = LoadTexture(TextFormat("%sData/Player/Frame1.png" , Path));
    //LuminaData.SingleFrame2 = LoadTexture(TextFormat("%sData/Player/Frame2.png" , Path));
    LuminaData.TileLayer = LoadTexture(TextFormat("%sData/Tiles.png" , Path));
    LuminaData.LevelWidth = 0;
    LuminaData.LevelHeight = 0;

    const char* filename = TextFormat("%sData/data.json" , Path); 
    FILE* file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file into a buffer
    char* jsonBuffer = (char*)malloc(fileSize + 1);
    fread(jsonBuffer, 1, fileSize, file);
    jsonBuffer[fileSize] = '\0'; // Null-terminate the buffer

    fclose(file);

    cJSON* root = cJSON_Parse(jsonBuffer);

    LuminaData.LevelWidth = (cJSON_GetObjectItem(root , "width")->valueint) / 16;
    LuminaData.LevelHeight = (cJSON_GetObjectItem(root , "height")->valueint) / 16;

    cJSON_Delete(root);
    free(jsonBuffer);

    ReadCSVFile(TextFormat("%sData/Collisions.csv" , Path) , LuminaData.Collision , LuminaData.LevelWidth , LuminaData.LevelHeight);

    LuminaData.LCamera = { 0 };

    LuminaData.LCamera.target = (Vector2){ 0 , 0};
    LuminaData.LCamera.offset = (Vector2){ LuminaTarget.texture.width / 2.0f, LuminaTarget.texture.height / 2.0f };

    LuminaData.LCamera.rotation = 0.0f;
    LuminaData.LCamera.zoom = 1.0f;

    SetPlayer();

    ExitLuminaFlag = false;
}

void LuminaDrawWorld(){

    BeginTextureMode(LuminaTarget);
    BeginDrawing();

    BeginMode2D(LuminaData.LCamera);

    LuminaData.LCamera.target = (Vector2){LuminaData.PlayerX + 16, LuminaData.PlayerY + 16};

    ClearBackground(BLACK);

    DrawTexture(LuminaData.TileLayer , 0, 0, WHITE);

    if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)){
        //DrawTexture(LuminaData.SingleFrame1 ,LuminaData.PlayerX, LuminaData.PlayerY, WHITE);
    }
    else {
        if (LuminaData.Ticker > 30){
            //DrawTexture(LuminaData.SingleFrame1 ,LuminaData.PlayerX, LuminaData.PlayerY, WHITE);
        }
        else {
            //DrawTexture(LuminaData.SingleFrame2 ,LuminaData.PlayerX, LuminaData.PlayerY, WHITE);
        }
    }

    if (LuminaData.Ticker != 60){
        LuminaData.Ticker ++;
    }
    else {
        LuminaData.Ticker = 0;
    }

    DrawRectangleLines(LuminaData.PlayerX, LuminaData.PlayerY, 32, 16 * 3 , WHITE);

    if(LuminaData.Collision[(LuminaData.PlayerY / 16) + 3][LuminaData.PlayerX / 16] != 1 && LuminaData.Collision[(LuminaData.PlayerY / 16) + 3][(LuminaData.PlayerX / 16) + 1] != 1){
        if (!LuminaData.PlayerIsJumping)LuminaData.PlayerY  += 4;
    }
    else {
        LuminaData.PlayerJumpCount = 20;
        LuminaData.PlayerIsJumping = false;

        if (LuminaData.PlayerY % 16 == 1){
            LuminaData.PlayerY --;
        }

        if (IsKeyDown(KEY_SPACE) && !LuminaData.PlayerIsJumping){
            LuminaData.PlayerIsJumping = true;
        }

    }

    LuminaData.LCamera.zoom += GetMouseWheelMove();

    if (LuminaData.Collision[(LuminaData.PlayerY / 16) + 3][(LuminaData.PlayerX / 16) + 1] == 3){
        ExitLuminaFlag = true;
    }

    if (LuminaData.Collision[(LuminaData.PlayerY / 16) + 2][(LuminaData.PlayerX / 16) + 2] == 1){
        LuminaData.PlayerX -= LuminaData.Speed;
    }
    if (LuminaData.Collision[(LuminaData.PlayerY / 16) + 2][(LuminaData.PlayerX / 16)] == 1){
        LuminaData.PlayerX += LuminaData.Speed;
    }

    if (LuminaData.Collision[(LuminaData.PlayerY / 16) + 2][(LuminaData.PlayerX / 16)] == KillTile || LuminaData.Collision[(LuminaData.PlayerY / 16) + 2][(LuminaData.PlayerX / 16) + 2] == KillTile){
        SetPlayer();
    }

    if (LuminaData.PlayerIsJumping) {
        if (LuminaData.PlayerJumpCount >= -10) {

            LuminaData.PlayerY += -(LuminaData.PlayerJumpCount * abs(LuminaData.PlayerJumpCount)) * .03;

            LuminaData.PlayerJumpCount -= 1;
        }
        else {
            LuminaData.PlayerJumpCount = 20;
            LuminaData.PlayerIsJumping = false;
        }
    }

    if (IsKeyDown(KEY_D)){
        LuminaData.PlayerX += LuminaData.Speed;
    }
    if (IsKeyDown(KEY_A)){
        LuminaData.PlayerX -= LuminaData.Speed;
    }

    if ((LuminaData.PlayerX + 16) <= (320 / 2)){
        LuminaData.PlayerX ++;
    }

    EndMode2D();

    EndDrawing();
    EndTextureMode();
    
}
