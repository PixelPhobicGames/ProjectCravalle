#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"


#define SHADOWMAP_RESOLUTION 1024 * 2


RenderTexture2D LoadShadowmapRenderTexture(int width, int height)
{
    RenderTexture2D target = { 0 };
    target.id = rlLoadFramebuffer(NULL , NULL); // Load an empty framebuffer
    target.texture.width = width;
    target.texture.height = height;

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;
        target.depth.mipmaps = 1;

        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        rlDisableFramebuffer();
    }

    return target;
}


class LightingSystem {
    public:
        Camera3D ShadowCamera;
        Shader ShadowShader;

        RenderTexture2D ShadowMap;

        Vector3 LightDirection;

        float Offset;

        int lightDirLoc;
        int lightVPLoc;
        int shadowMapLoc;

        void Init(){
            ShadowShader = LoadShader("Lights/Shadowmap.vs" , "Lights/Shadowmap.fs" );
            ShadowShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(ShadowShader, "viewPos");
            Color lightColor = WHITE;
            Vector4 lightColorNormalized = ColorNormalize(lightColor);
            lightDirLoc = GetShaderLocation(ShadowShader, "lightDir");
            int lightColLoc = GetShaderLocation(ShadowShader, "lightColor");
            SetShaderValue(ShadowShader, lightDirLoc, &LightDirection, SHADER_UNIFORM_VEC3);
            SetShaderValue(ShadowShader, lightColLoc, &lightColorNormalized, SHADER_UNIFORM_VEC4);

            int ambientLoc = GetShaderLocation(ShadowShader, "ambient");
            float ambient[4] = {0.01f, 0.01f, 0.01f, .10f};

            SetShaderValue(ShadowShader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);
            lightVPLoc = GetShaderLocation(ShadowShader, "lightVP");
            shadowMapLoc = GetShaderLocation(ShadowShader, "shadowMap");

            int shadowMapResolution = SHADOWMAP_RESOLUTION;
            SetShaderValue(ShadowShader, GetShaderLocation(ShadowShader, "shadowMapResolution"), &shadowMapResolution, SHADER_UNIFORM_INT);


            ShadowCamera = (Camera3D){ 0 };

            ShadowCamera.position = Vector3Scale(LightDirection, Offset);
            ShadowCamera.target = Vector3Zero();

            ShadowCamera.projection = CAMERA_ORTHOGRAPHIC;
            ShadowCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            ShadowCamera.fovy = 60.0f;

            ShadowMap = LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
        }

        void RenderShadows(Camera3D Cam, void (*DrawScene)() ){

            Vector3 cameraPos = Cam.position;
            SetShaderValue(ShadowShader, ShadowShader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);

            LightDirection = Vector3Normalize(LightDirection);
            ShadowCamera.position = Vector3Scale(LightDirection, Offset);
            SetShaderValue(ShadowShader, lightDirLoc, &LightDirection, SHADER_UNIFORM_VEC3);

            Matrix lightView;
            Matrix lightProj;
            BeginTextureMode(ShadowMap);
                ClearBackground(WHITE);
                BeginMode3D(ShadowCamera);
                    lightView = rlGetMatrixModelview();
                    lightProj = rlGetMatrixProjection();

                    DrawScene();

                EndMode3D();
            EndTextureMode();

            Matrix lightViewProj = MatrixMultiply(lightView, lightProj);

            ClearBackground(RAYWHITE);

            SetShaderValueMatrix(ShadowShader, lightVPLoc, lightViewProj);

            rlEnableShader(ShadowShader.id);
            int slot = 10; // Can be anything 0 to 15, but 0 will probably be taken up
            rlActiveTextureSlot(10);
            rlEnableTexture(ShadowMap.depth.id);
            rlSetUniform(shadowMapLoc, &slot, SHADER_UNIFORM_INT, 1);
        }

        void PositionMainLight(Vector3 Pos, float Off){
            LightDirection = Vector3Normalize(Pos);
            Offset = Off;

        }
};

static LightingSystem OTLightingSystem;