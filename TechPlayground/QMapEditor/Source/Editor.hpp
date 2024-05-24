#include "QMaps.hpp"

Ray GetScreenToWorldRayEx(Vector2 position, Camera camera, int width, int height)
{
    Ray ray = { 0 };

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f*position.x)/(float)width - 1.0f;
    float y = 1.0f - (2.0f*position.y)/(float)height;
    float z = 1.0f;

    // Store values in a vector
    Vector3 deviceCoords = { x, y, z };

    // Calculate view matrix from camera look at
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    Matrix matProj = MatrixIdentity();

    if (camera.projection == CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        matProj = MatrixPerspective(camera.fovy*DEG2RAD, ((double)width/(double)height),  0.01 , 1000.0);
    }
    else if (camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        double aspect = (double)width/(double)height;
        double top = camera.fovy/2.0;
        double right = top*aspect;

        // Calculate projection matrix from orthographic
        matProj = MatrixOrtho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Unproject far/near points
    Vector3 nearPoint = Vector3Unproject((Vector3){ deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
    Vector3 farPoint = Vector3Unproject((Vector3){ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);

    // Unproject the mouse cursor in the near plane
    // We need this as the source position because orthographic projects,
    // compared to perspective doesn't have a convergence point,
    // meaning that the "eye" of the camera is more like a plane than a point
    Vector3 cameraPlanePointerPos = Vector3Unproject((Vector3){ deviceCoords.x, deviceCoords.y, -1.0f }, matProj, matView);

    // Calculate normalized direction vector
    Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));

    if (camera.projection == CAMERA_PERSPECTIVE) ray.position = camera.position;
    else if (camera.projection == CAMERA_ORTHOGRAPHIC) ray.position = cameraPlanePointerPos;

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}

Ray GetScreenToWorldRay(Vector2 position, Camera camera)
{
    Ray ray = GetScreenToWorldRayEx(position, camera, GetScreenWidth(), GetScreenHeight());

    return ray;
}


class Editor {
    public:
        enum EditorMode {PointFaceCreation , QuadFaceCreation, FaceDeletion};

        EditorMode CurrentMode = PointFaceCreation;

        Vector3 PFCPoint = {0 , 0 , 0};

        int PointCounter = 1;
        int EditorTextureID = 1;

        float MDistance = 1.0f;

        Vector3 Point1 = {0 , 0 , 0};
        Vector3 Point2 = {0 , 0 , 0};
        Vector3 Point3 = {0 , 0 , 0};
        Vector3 Point4 = {0 , 0 , 0};

        Ray DeletionRay;


        void UpdateEdior(Camera WorldCam){
            switch (CurrentMode)
            {
                case PointFaceCreation:
                    DrawCube(PFCPoint , 0.15, 0.15 , 0.15 , GREEN);

                    DrawLine3D(PFCPoint, {PFCPoint.x , PFCPoint.y , PFCPoint.z - 100}, GREEN);
                    DrawLine3D(PFCPoint, {PFCPoint.x - 100, PFCPoint.y , PFCPoint.z}, BLUE);
                    DrawLine3D(PFCPoint, {PFCPoint.x, PFCPoint.y - 100, PFCPoint.z}, RED);

                    DrawLine3D(PFCPoint, {PFCPoint.x , PFCPoint.y , PFCPoint.z + 100}, GREEN);
                    DrawLine3D(PFCPoint, {PFCPoint.x + 100, PFCPoint.y , PFCPoint.z}, BLUE);
                    DrawLine3D(PFCPoint, {PFCPoint.x, PFCPoint.y + 100, PFCPoint.z}, RED);

                    DrawCube(Point1 , 0.15, 0.15 , 0.15 , PURPLE);
                    DrawCube(Point2 , 0.15, 0.15 , 0.15 , PURPLE);
                    DrawCube(Point3 , 0.15, 0.15 , 0.15 , PURPLE);
                    DrawCube(Point4 , 0.15, 0.15 , 0.15 , PURPLE);

                    if (IsKeyPressed(KEY_BACKSPACE)){
                        Point1 = {0 , 0 , 0};
                        Point2 = {0 , 0 , 0};
                        Point3 = {0 , 0 , 0};
                        Point4 = {0 , 0 , 0};
                        PointCounter = 1;
                    }

                    if (IsKeyPressed(KEY_U)){
                        PFCPoint.y += MDistance;
                    }
                    if (IsKeyPressed(KEY_J)){
                        PFCPoint.y -= MDistance;
                    }

                    if (IsKeyPressed(KEY_K)){
                        PFCPoint.x -= MDistance;
                    }
                    if (IsKeyPressed(KEY_H)){
                        PFCPoint.x += MDistance;
                    }

                    if (IsKeyPressed(KEY_I)){
                        PFCPoint.z += MDistance;
                    }
                    if (IsKeyPressed(KEY_Y)){
                        PFCPoint.z -= MDistance;
                    }

                    if (IsKeyPressed(KEY_X)){
                        MDistance = 1.0f;
                    }
                    if (IsKeyPressed(KEY_Z)){
                        MDistance = .50f;
                    }
                    if (IsKeyPressed(KEY_C)){
                        MDistance = 2.0f;
                    }

                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                        PFCPoint.y  -= int (GetMouseDelta().y / 5.0f);
                        PFCPoint.x  -= int (GetMouseDelta().x / 5.0f);
                    }


                    if (IsKeyPressed(KEY_ENTER)){

                        switch (PointCounter)
                        {
                            case 1:
                                Point1 = PFCPoint;
                                break;
                            case 2:
                                Point2 = PFCPoint;
                                break;
                            case 3:
                                Point3 = PFCPoint;
                                break;
                            case 4:
                                Point4 = PFCPoint;

                                wstring OldData = LoadFile("Working/World1.qmap");

                                wstring NewData = L"";

                                NewData += L"WA:";
                                NewData += to_wstring(Point1.x) + L":";
                                NewData += to_wstring(Point1.y) + L":";
                                NewData += to_wstring(Point1.z) + L":";

                                NewData += to_wstring(Point2.x) + L":";
                                NewData += to_wstring(Point2.y) + L":";
                                NewData += to_wstring(Point2.z) + L":";

                                NewData += to_wstring(Point3.x) + L":";
                                NewData += to_wstring(Point3.y) + L":";
                                NewData += to_wstring(Point3.z) + L":";

                                NewData += to_wstring(Point4.x) + L":";
                                NewData += to_wstring(Point4.y) + L":";
                                NewData += to_wstring(Point4.z) + L":";

                                NewData += to_wstring(1) + L":";
                                NewData += to_wstring(EditorTextureID) + L":";

                                wofstream OutFile;
                                OutFile.open("Working/World1.qmap");
                                OutFile << OldData + NewData;
                                OutFile.close();

                                QMapSystem.LoadQMap("Working/World1.qmap");

                                Point1 = {0 , 0 , 0};
                                Point2 = {0 , 0 , 0};
                                Point3 = {0 , 0 , 0};
                                Point4 = {0 , 0 , 0};

                                PointCounter = 0;
                                

                                break;
                        }

                        PointCounter ++;
                    }
                    break;

                case QuadFaceCreation:
                    DrawCube(PFCPoint , 0.15, 0.15 , 0.15 , GREEN);

                    DrawLine3D(PFCPoint, {PFCPoint.x , PFCPoint.y , PFCPoint.z - 100}, GREEN);
                    DrawLine3D(PFCPoint, {PFCPoint.x - 100, PFCPoint.y , PFCPoint.z}, BLUE);
                    DrawLine3D(PFCPoint, {PFCPoint.x, PFCPoint.y - 100, PFCPoint.z}, RED);

                    DrawLine3D(PFCPoint, {PFCPoint.x , PFCPoint.y , PFCPoint.z + 100}, GREEN);
                    DrawLine3D(PFCPoint, {PFCPoint.x + 100, PFCPoint.y , PFCPoint.z}, BLUE);
                    DrawLine3D(PFCPoint, {PFCPoint.x, PFCPoint.y + 100, PFCPoint.z}, RED);

                    DrawCube(Point1 , 0.15, 0.15 , 0.15 , PURPLE);
                    DrawCube(Point2 , 0.15, 0.15 , 0.15 , PURPLE);

                    if (IsKeyPressed(KEY_U)){
                        PFCPoint.y += MDistance;
                    }
                    if (IsKeyPressed(KEY_J)){
                        PFCPoint.y -= MDistance;
                    }

                    if (IsKeyPressed(KEY_K)){
                        PFCPoint.x -= MDistance;
                    }
                    if (IsKeyPressed(KEY_H)){
                        PFCPoint.x += MDistance;
                    }

                    if (IsKeyPressed(KEY_I)){
                        PFCPoint.z += MDistance;
                    }
                    if (IsKeyPressed(KEY_Y)){
                        PFCPoint.z -= MDistance;
                    }

                    if (IsKeyPressed(KEY_X)){
                        MDistance = 1.0f;
                    }
                    if (IsKeyPressed(KEY_Z)){
                        MDistance = .50f;
                    }
                    if (IsKeyPressed(KEY_C)){
                        MDistance = 2.0f;
                    }

                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                        PFCPoint.y  -= int (GetMouseDelta().y / 5.0f);
                        PFCPoint.x  -= int (GetMouseDelta().x / 5.0f);
                    }

                    if (IsKeyPressed(KEY_BACKSPACE)){
                        Point1 = {0 , 0 , 0};
                        Point2 = {0 , 0 , 0};
                        Point3 = {0 , 0 , 0};
                        Point4 = {0 , 0 , 0};
                        PointCounter = 1;
                    }


                    if (IsKeyPressed(KEY_ENTER)){

                        switch (PointCounter)
                        {
                            case 1:
                                Point1 = PFCPoint;
                                break;
                            case 2:
                                Point4 = PFCPoint;

                                // Determine the plane of alignment based on the coordinates of Point1 and Point4
                                if (Point1.z == Point4.z) {
                                    // Alignment in the x-y plane

                                    // Calculate Point2
                                    Point2.x = Point4.x;
                                    Point2.y = Point1.y;
                                    Point2.z = Point1.z; // Same z

                                    // Calculate Point3
                                    Point3.x = Point1.x;
                                    Point3.y = Point4.y;
                                    Point3.z = Point1.z; // Same z

                                } else if (Point1.x == Point4.x) {
                                    // Alignment in the z-y plane

                                    // Calculate Point2
                                    Point2.x = Point1.x; // Same x
                                    Point2.y = Point1.y;
                                    Point2.z = Point4.z;

                                    // Calculate Point3
                                    Point3.x = Point1.x; // Same x
                                    Point3.y = Point4.y;
                                    Point3.z = Point1.z;

                                } else if (Point1.y == Point4.y) {
                                    // Alignment in the x-z plane

                                    // Calculate Point2
                                    Point2.x = Point4.x;
                                    Point2.y = Point1.y; // Same y
                                    Point2.z = Point1.z;

                                    // Calculate Point3
                                    Point3.x = Point1.x;
                                    Point3.y = Point1.y; // Same y
                                    Point3.z = Point4.z;

                                } else {
                                    // Alignment in the z-x plane

                                    // Calculate Point2
                                    Point2.x = Point1.x;
                                    Point2.y = Point4.y; // Same y
                                    Point2.z = Point1.z;

                                    // Calculate Point3
                                    Point3.x = Point1.x;
                                    Point3.y = Point1.y;
                                    Point3.z = Point4.z; // Same z
                                }


                                wstring OldData = LoadFile("Working/World1.qmap");

                                wstring NewData = L"";

                                NewData += L"WA:";

                                NewData += to_wstring(Point1.x) + L":";
                                NewData += to_wstring(Point1.y) + L":";
                                NewData += to_wstring(Point1.z) + L":";

                                NewData += to_wstring(Point2.x) + L":";
                                NewData += to_wstring(Point2.y) + L":";
                                NewData += to_wstring(Point2.z) + L":";

                                NewData += to_wstring(Point3.x) + L":";
                                NewData += to_wstring(Point3.y) + L":";
                                NewData += to_wstring(Point3.z) + L":";

                                NewData += to_wstring(Point4.x) + L":";
                                NewData += to_wstring(Point4.y) + L":";
                                NewData += to_wstring(Point4.z) + L":";

                                NewData += to_wstring(1) + L":";
                                NewData += to_wstring(EditorTextureID) + L":";

                                wofstream OutFile;
                                OutFile.open("Working/World1.qmap");
                                OutFile << OldData + NewData;
                                OutFile.close();

                                QMapSystem.LoadQMap("Working/World1.qmap");

                                Point1 = {0 , 0 , 0};
                                Point2 = {0 , 0 , 0};
                                Point3 = {0 , 0 , 0};
                                Point4 = {0 , 0 , 0};

                                PointCounter = 0;
                                

                                break;
                        }

                        PointCounter ++;
                    }

                    break;

                case FaceDeletion:

                    RayCollision collision = { 0 };

                    collision.distance = MAXFLOAT;
                    collision.hit = false;

                    DeletionRay = GetScreenToWorldRay(GetMousePosition(), WorldCam);

                    RayCollision groundHitInfo;

                    for (int i = 0 ; i <= QMapSystem.WallCounter; i++){
                        groundHitInfo = GetRayCollisionQuad(DeletionRay, QMapSystem.QMapWalls[i].Point1, QMapSystem.QMapWalls[i].Point2, QMapSystem.QMapWalls[i].Point3, QMapSystem.QMapWalls[i].Point4);

                        if ((groundHitInfo.hit) && (groundHitInfo.distance < collision.distance))
                        {
                            collision = groundHitInfo;
                            DrawCube(collision.point, 0.3f, 0.3f, 0.3f, PURPLE);

                            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                                int PopIndex = i;
                                wstring NWallData = L"";
                                wstring NFloorData = L"";

                                for (int x = 0; x <= QMapSystem.WallCounter; x ++){
                                    if (x != PopIndex){
                                        NWallData += L"WA:" 
                                        + to_wstring(QMapSystem.QMapWalls[x].Point1.x) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point1.y) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point1.z) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point2.x) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point2.y) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point2.z) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point3.x) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point3.y) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point3.z) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point4.x) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point4.y) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].Point4.z) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].CollisionEnabled) + L":"
                                        + to_wstring(QMapSystem.QMapWalls[x].TextureID) + L":";
                                    }
                                }

                                for (int x = 0; x <= QMapSystem.FloorCounter; x ++){
                                    if (QMapSystem.QMapFloors[x].TextureID != NULL){
                                        NFloorData += L"FL:" 
                                        + to_wstring(QMapSystem.QMapFloors[x].Point1.x) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point1.y) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point1.z) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point2.x) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point2.y) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point2.z) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point3.x) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point3.y) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point3.z) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point4.x) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point4.y) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].Point4.z) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].CollisionEnabled) + L":"
                                        + to_wstring(QMapSystem.QMapFloors[x].TextureID) + L":";
                                    }
                                }

                                QMapSystem.WallCounter --;

                                wofstream OutFile;
                                OutFile.open("Working/World1.qmap");
                                OutFile << NWallData + NFloorData;
                                OutFile.close();

                                QMapSystem.LoadQMap("Working/World1.qmap");
                            
                            }
                        }
                    }

                    break;
            }
        }
        
};

static Editor QMEditorCore;