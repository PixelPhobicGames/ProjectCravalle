#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

// Fast Rendering Implementation 

#define MAX_MATERIAL_MAPS       12 
#define DEFAULT_ASPECT_RATIO 16.0f/9.0f
#define DEFAULT_NEAR_PLANE 0.1f
#define DEFAULT_FAR_PLANE 1000.0f


typedef struct Plane {
    Vector3 normal;
    float distance;
} Plane;

Vector3 Vector3FromFloatArray(float* array)
{
    return (Vector3){ array[0], array[1], array[2] };
}

bool CheckPointInFrustum(Vector3 point, Camera camera, float aspect, float nearPlane, float farPlane)
{
    float halfVSide = farPlane * tanf(camera.fovy * 0.5f * DEG2RAD);
    float halfHSide = halfVSide * aspect;

    Vector3 forward = Vector3Subtract(camera.target, camera.position);
    forward = Vector3Normalize(forward);
    Vector3 right = Vector3CrossProduct(forward, camera.up);
    right = Vector3Normalize(right);
    Vector3 up = Vector3CrossProduct(right, forward);

    Vector3 nearCenter = Vector3Add(camera.position, Vector3Scale(forward, nearPlane));
    Vector3 farCenter = Vector3Add(camera.position, Vector3Scale(forward, farPlane));

    Plane planes[6];

    planes[0].normal = forward;
    planes[0].distance = Vector3DotProduct(forward, nearCenter);

    planes[1].normal = Vector3Negate(forward);
    planes[1].distance = -Vector3DotProduct(forward, farCenter);

    planes[2].normal = Vector3CrossProduct(up, Vector3Add(forward, Vector3Scale(right, halfHSide)));
    planes[2].distance = Vector3DotProduct(planes[2].normal, farCenter);

    planes[3].normal = Vector3CrossProduct(Vector3Add(forward, Vector3Scale(Vector3Negate(right), halfHSide)), up);
    planes[3].distance = Vector3DotProduct(planes[3].normal, farCenter);

    planes[4].normal = Vector3CrossProduct(right, Vector3Add(forward, Vector3Scale(up, halfVSide)));
    planes[4].distance = Vector3DotProduct(planes[4].normal, farCenter);

    planes[5].normal = Vector3CrossProduct(Vector3Add(forward, Vector3Scale(Vector3Negate(up), halfVSide)), right);
    planes[5].distance = Vector3DotProduct(planes[5].normal, farCenter);

    for (int i = 0; i < 6; i++)
    {
        if (Vector3DotProduct(planes[i].normal, point) + planes[i].distance < 0)
        {
            return false;
        }
    }

    return true;
}

void FDrawMesh(Mesh mesh, Material material, Matrix transform, Camera3D cam)
{
    rlEnableShader(material.shader.id);

    // Send required data to shader (matrices, values)
    //-----------------------------------------------------
    // Upload to shader material.colDiffuse
    if (material.shader.locs[SHADER_LOC_COLOR_DIFFUSE] != -1)
    {
        float values[4] = {
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.r / 255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.g / 255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.b / 255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.a / 255.0f
        };

        rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_DIFFUSE], values, SHADER_UNIFORM_VEC4, 1);
    }

    // Upload to shader material.colSpecular (if location available)
    if (material.shader.locs[SHADER_LOC_COLOR_SPECULAR] != -1)
    {
        float values[4] = {
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.r / 255.0f,
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.g / 255.0f,
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.b / 255.0f,
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.a / 255.0f
        };

        rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_SPECULAR], values, SHADER_UNIFORM_VEC4, 1);
    }

    // Get a copy of current matrices to work with
    Matrix matModel = MatrixIdentity();
    Matrix matView = rlGetMatrixModelview();
    Matrix matModelView = MatrixIdentity();
    Matrix matProjection = rlGetMatrixProjection();

    // Upload view and projection matrices (if locations available)
    if (material.shader.locs[SHADER_LOC_MATRIX_VIEW] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_VIEW], matView);
    if (material.shader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);

    // Accumulate several model transformations
    matModel = MatrixMultiply(transform, rlGetMatrixTransform());

    // Model transformation matrix is sent to shader uniform location: SHADER_LOC_MATRIX_MODEL
    if (material.shader.locs[SHADER_LOC_MATRIX_MODEL] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MODEL], matModel);

    // Get model-view matrix
    matModelView = MatrixMultiply(matModel, matView);

    // Upload model normal matrix (if locations available)
    if (material.shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));

    // Bind active texture maps (if available)
    for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id > 0)
        {
            // Select current shader texture slot
            rlActiveTextureSlot(i);

            // Enable texture for active slot
            if ((i == MATERIAL_MAP_IRRADIANCE) ||
                (i == MATERIAL_MAP_PREFILTER) ||
                (i == MATERIAL_MAP_CUBEMAP)) rlEnableTextureCubemap(material.maps[i].texture.id);
            else rlEnableTexture(material.maps[i].texture.id);

            rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + i], &i, SHADER_UNIFORM_INT, 1);
        }
    }

    // Try binding vertex array objects (VAO) or use VBOs if not possible
    if (!rlEnableVertexArray(mesh.vaoId))
    {
        // Bind mesh VBO data and set vertex attributes
        rlEnableVertexBuffer(mesh.vboId[0]);
        rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION]);

        rlEnableVertexBuffer(mesh.vboId[1]);
        rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01], 2, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

        if (material.shader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
        {
            rlEnableVertexBuffer(mesh.vboId[2]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL], 3, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL]);
        }

        if (material.shader.locs[SHADER_LOC_VERTEX_COLOR] != -1)
        {
            if (mesh.vboId[3] != 0)
            {
                rlEnableVertexBuffer(mesh.vboId[3]);
                rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR], 4, RL_UNSIGNED_BYTE, 1, 0, 0);
                rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
            else
            {
                float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                rlSetVertexAttributeDefault(material.shader.locs[SHADER_LOC_VERTEX_COLOR], value, SHADER_ATTRIB_VEC4, 4);
                rlDisableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
        }

        if (material.shader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
        {
            rlEnableVertexBuffer(mesh.vboId[4]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT], 4, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT]);
        }

        if (material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
        {
            rlEnableVertexBuffer(mesh.vboId[5]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
        }

        if (mesh.indices != NULL) rlEnableVertexBufferElement(mesh.vboId[6]);
    }

    int eyeCount = 1;
    if (rlIsStereoRenderEnabled()) eyeCount = 2;

    for (int eye = 0; eye < eyeCount; eye++)
    {
        // Calculate model-view-projection matrix (MVP)
        Matrix matModelViewProjection = MatrixIdentity();
        if (eyeCount == 1) matModelViewProjection = MatrixMultiply(matModelView, matProjection);
        else
        {
            rlViewport(eye * rlGetFramebufferWidth() / 2, 0, rlGetFramebufferWidth() / 2, rlGetFramebufferHeight());
            matModelViewProjection = MatrixMultiply(MatrixMultiply(matModelView, rlGetMatrixViewOffsetStereo(eye)), rlGetMatrixProjectionStereo(eye));
        }

        // Send combined model-view-projection matrix to shader
        rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

        // Draw visible triangles

        float aspect = (float)rlGetFramebufferWidth() / (float)rlGetFramebufferHeight();
        float nearPlane = 0.1f;
        float farPlane = 1000.0f; 

        if (mesh.indices != NULL)
        {
            for (int i = 0; i < mesh.triangleCount; i++)
            {
                Vector3 v1 = Vector3FromFloatArray(&mesh.vertices[mesh.indices[i * 3] * 3]);
                Vector3 v2 = Vector3FromFloatArray(&mesh.vertices[mesh.indices[i * 3 + 1] * 3]);
                Vector3 v3 = Vector3FromFloatArray(&mesh.vertices[mesh.indices[i * 3 + 2] * 3]);

                if (CheckPointInFrustum(v1, cam, aspect, nearPlane, farPlane) ||
                    CheckPointInFrustum(v2, cam, aspect, nearPlane, farPlane) || CheckPointInFrustum(v3, cam, aspect, nearPlane, farPlane))
                {
                    // Draw the triangle
                    rlDrawVertexArrayElements(i * 3, 3, 0);
                }
            }
        }
        else
        {
            rlDrawVertexArray(0, mesh.vertexCount);
        }
    }

    // Unbind all bound texture maps
    for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id > 0)
        {
            rlActiveTextureSlot(i);

            if ((i == MATERIAL_MAP_IRRADIANCE) ||
                (i == MATERIAL_MAP_PREFILTER) ||
                (i == MATERIAL_MAP_CUBEMAP)) rlDisableTextureCubemap();
            else rlDisableTexture();
        }
    }

    // Disable all possible vertex array objects (or VBOs)
    rlDisableVertexArray();
    rlDisableVertexBuffer();
    rlDisableVertexBufferElement();

    // Disable shader program
    rlDisableShader();

    // Restore rlgl internal modelview and projection matrices
    rlSetMatrixModelview(matView);
    rlSetMatrixProjection(matProjection);
}

void FDrawModelEx(Camera cam , Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint)
{

    Matrix matScale = MatrixScale(scale.x, scale.y, scale.z);
    Matrix matRotation = MatrixRotate(rotationAxis, rotationAngle*DEG2RAD);
    Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);

    Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);


    model.transform = MatrixMultiply(model.transform, matTransform);

    for (int i = 0; i < model.meshCount; i++)
    {
        Color color = model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color;

        Color colorTint = WHITE;
        colorTint.r = (unsigned char)(((int)color.r*(int)tint.r)/255);
        colorTint.g = (unsigned char)(((int)color.g*(int)tint.g)/255);
        colorTint.b = (unsigned char)(((int)color.b*(int)tint.b)/255);
        colorTint.a = (unsigned char)(((int)color.a*(int)tint.a)/255);

        model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = colorTint;

        FDrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], model.transform, cam);

        model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = color;
    }
}

void FDrawModel(Camera cam, Model model, Vector3 position, float scale, Color tint)
{
    Vector3 vScale = { scale, scale, scale };
    Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };

    FDrawModelEx(cam, model, position, rotationAxis, 0.0f, vScale, tint);
}
