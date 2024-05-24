#include "PPGIO/PPGIO.hpp"
#include "rlgl.h"
#include "raymath.h"

#include <regex>

using namespace std;



/* QMap Technology - Liquid  2024 */


#define MaxQMapTextures 20

#define MaxFloors 100
#define MaxWalls 100

#define MaxQPlanes 10000


class Floor{
    public:
        int TextureID;
        bool CollisionEnabled;

        Vector3 Point1;
        Vector3 Point2;
        Vector3 Point3;
        Vector3 Point4;
};

class Wall{
    public:
        int TextureID;
        bool CollisionEnabled;

        Vector3 Point1;
        Vector3 Point2;
        Vector3 Point3;
        Vector3 Point4;
};

class QMap{
    public:        
        Wall QMapWalls[MaxWalls];
        Floor QMapFloors[MaxFloors];
        
        int WallCounter;
        int FloorCounter;

        Texture2D EmptyTexture;
        Texture2D QMapTextures[MaxQMapTextures];


        void LoadQMap(const char* Path){
            WallCounter = 0;
            FloorCounter = 0;

            wstring QMapData = LoadFile(Path);

            EmptyTexture = LoadTextureFromImage(GenImagePerlinNoise(100 , 100 , 0 , 0 , 10.0f));

            for (int i = 0; i <= MaxQMapTextures; i++){
                if (IsPathFile(TextFormat("Working/QMap/Q%i.png" , i))){
                    SetTextureWrap(QMapTextures[i], TEXTURE_WRAP_MIRROR_REPEAT);
                    QMapTextures[i] = LoadTexture(TextFormat("Working/QMap/Q%i.png" , i));
                }
            }

            for (int i = 0; i <= GetWDLSize(QMapData , L"") ; i++ ){
                wstring Instruction = WSplitValue(QMapData , i);

                if (Instruction == L"FL"){
                    QMapFloors[FloorCounter].Point1 = {ToFloat(WSplitValue(QMapData , i + 1)) , ToFloat(WSplitValue(QMapData , i + 2)) , ToFloat(WSplitValue(QMapData , i + 3))};
                    QMapFloors[FloorCounter].Point2 = {ToFloat(WSplitValue(QMapData , i + 4)) , ToFloat(WSplitValue(QMapData , i + 5)) , ToFloat(WSplitValue(QMapData , i + 6))};
                    QMapFloors[FloorCounter].Point3 = {ToFloat(WSplitValue(QMapData , i + 7)) , ToFloat(WSplitValue(QMapData , i + 8)) , ToFloat(WSplitValue(QMapData , i + 9))};
                    QMapFloors[FloorCounter].Point4 = {ToFloat(WSplitValue(QMapData , i + 10)) , ToFloat(WSplitValue(QMapData , i + 11)) , ToFloat(WSplitValue(QMapData , i + 12))};
                    QMapFloors[FloorCounter].CollisionEnabled = stoi(WSplitValue(QMapData , i + 13));
                    QMapFloors[FloorCounter].TextureID = stoi(WSplitValue(QMapData , i + 14));
                    FloorCounter ++;
                }
                if (Instruction == L"WA"){
                    QMapWalls[WallCounter].Point1 = {ToFloat(WSplitValue(QMapData , i + 1)) , ToFloat(WSplitValue(QMapData , i + 2)) , ToFloat(WSplitValue(QMapData , i + 3))};
                    QMapWalls[WallCounter].Point2 = {ToFloat(WSplitValue(QMapData , i + 4)) , ToFloat(WSplitValue(QMapData , i + 5)) , ToFloat(WSplitValue(QMapData , i + 6))};
                    QMapWalls[WallCounter].Point3 = {ToFloat(WSplitValue(QMapData , i + 7)) , ToFloat(WSplitValue(QMapData , i + 8)) , ToFloat(WSplitValue(QMapData , i + 9))};
                    QMapWalls[WallCounter].Point4 = {ToFloat(WSplitValue(QMapData , i + 10)) , ToFloat(WSplitValue(QMapData , i + 11)) , ToFloat(WSplitValue(QMapData , i + 12))};
                    QMapWalls[WallCounter].CollisionEnabled = stoi(WSplitValue(QMapData , i + 13));
                    QMapWalls[WallCounter].TextureID = stoi(WSplitValue(QMapData , i + 14));
                    WallCounter ++;
                }
            }

            cout << "Found " << FloorCounter << " Floors\n";
            cout << "Found " << WallCounter << " Walls\n";

        }

        void RenderWDL() {

        }


        void RenderQMap(){
            Vector2 texCoords[4] = {
                { 0.0f, 0.0f },  
                { 1.0f, 0.0f }, 
                { 0.0f, 1.0f },  
                { 1.0f, 1.0f }  
            };

            for (int i = 0 ; i <= FloorCounter; i++){
                DrawPoint3D(QMapFloors[i].Point1 , RED);
                DrawPoint3D(QMapFloors[i].Point2 , RED);
                DrawPoint3D(QMapFloors[i].Point3 , RED);
                DrawPoint3D(QMapFloors[i].Point4 , RED);

                DrawLine3D(QMapFloors[i].Point1 , QMapFloors[i].Point2, WHITE);
                DrawLine3D(QMapFloors[i].Point1 , QMapFloors[i].Point3 , WHITE);
                DrawLine3D(QMapFloors[i].Point3 , QMapFloors[i].Point4 , WHITE);
                DrawLine3D(QMapFloors[i].Point2 , QMapFloors[i].Point4 , WHITE);
            }

            for (int i = 0 ; i <= WallCounter; i++){
                DrawPoint3D(QMapWalls[i].Point1 , RED);
                DrawPoint3D(QMapWalls[i].Point2 , RED);
                DrawPoint3D(QMapWalls[i].Point3 , RED);
                DrawPoint3D(QMapWalls[i].Point4 , RED);

                DrawLine3D(QMapWalls[i].Point1 , QMapWalls[i].Point2, WHITE);
                DrawLine3D(QMapWalls[i].Point1 , QMapWalls[i].Point3 , WHITE);
                DrawLine3D(QMapWalls[i].Point3 , QMapWalls[i].Point4 , WHITE);
                DrawLine3D(QMapWalls[i].Point2 , QMapWalls[i].Point4 , WHITE);
                
                rlPushMatrix();

                rlBegin(RL_QUADS);

                

                if (IsTextureReady(QMapTextures[QMapWalls[i].TextureID]) && QMapWalls[i].TextureID <= MaxQMapTextures){
                    rlSetTexture(QMapTextures[QMapWalls[i].TextureID].id);
                } else {
                    rlSetTexture(EmptyTexture.id);
                }

                // Calculate dimensions of the quad
                float width = Vector3Distance(QMapWalls[i].Point1, QMapWalls[i].Point2);
                float height = Vector3Distance(QMapWalls[i].Point1, QMapWalls[i].Point3);

                // Define the texture size in world units (e.g., 1x1 unit square)
                float textureSize = 1.0f;

                // Calculate texture coordinates based on the quad size and texture size
                float texCoordRight = width / textureSize;
                float texCoordTop = height / textureSize;
                // Draw front face
                rlTexCoord2f(0.0f, 0.0f);
                rlVertex3f(QMapWalls[i].Point1.x, QMapWalls[i].Point1.y, QMapWalls[i].Point1.z);

                rlTexCoord2f(texCoordRight, 0.0f);
                rlVertex3f(QMapWalls[i].Point2.x, QMapWalls[i].Point2.y, QMapWalls[i].Point2.z);

                rlTexCoord2f(texCoordRight, texCoordTop);
                rlVertex3f(QMapWalls[i].Point4.x, QMapWalls[i].Point4.y, QMapWalls[i].Point4.z);

                rlTexCoord2f(0.0f, texCoordTop);
                rlVertex3f(QMapWalls[i].Point3.x, QMapWalls[i].Point3.y, QMapWalls[i].Point3.z);

                // Draw back face (reversed order)
                rlTexCoord2f(0.0f, 0.0f);
                rlVertex3f(QMapWalls[i].Point3.x, QMapWalls[i].Point3.y, QMapWalls[i].Point3.z);

                rlTexCoord2f(texCoordRight, 0.0f);
                rlVertex3f(QMapWalls[i].Point4.x, QMapWalls[i].Point4.y, QMapWalls[i].Point4.z);

                rlTexCoord2f(texCoordRight, texCoordTop);
                rlVertex3f(QMapWalls[i].Point2.x, QMapWalls[i].Point2.y, QMapWalls[i].Point2.z);

                rlTexCoord2f(0.0f, texCoordTop);
                rlVertex3f(QMapWalls[i].Point1.x, QMapWalls[i].Point1.y, QMapWalls[i].Point1.z);

                rlSetTexture(0);
                rlEnd();

                rlPopMatrix();
            }
        }
};

static QMap QMapSystem;