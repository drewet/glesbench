#include <stdio.h>
#include <string.h>

#include "../ogl_es.h"

#define _USE_MATH_DEFINES
#ifdef _WIN32
#include <xnamath.h>
#else
#include "../xna/gccxnamath.h"
#endif

#include "vsh.h"
#include "fsh.h"
#include "mesh.h"
#include "../shader/shader.h"
#include "../hud/fraps.h"
#include "../hud/chart.h"
#include "../hud/bff.h"
#include "../hud/bg.h"

namespace bunny {
#include "models/bunny.h"
}

namespace knot {
#include "models/knot.h"
}

namespace sphere {
#include "models/sphere.h"
}

void BeginFrame();
void EndFrame();
float GetElapsedMilliseconds();

#define SAFE_DELETE(p) \
    if (p) {delete p; p = NULL;}

enum
{
    MAX_POINT_LIGHTS = 8
};

struct POINT_LIGHT_SOURCE
{
    XMFLOAT3 Center;
    float AttenuationRadius;
    XMFLOAT3 DiffuseColor;
    XMFLOAT3 SpecularColor;
    float SpecularPower;
    bool bMoveable;
    bool bClockwiseOrbit;
    float OrbitRadius;
    float OrbitRoll;
    float Velocity;

    float OrbitAngle;
    XMFLOAT3 Position;
};

struct WVP_COLOR_UNIFORMS
{
    GLint Mwvp;
    GLint FillColor;
};

struct PHONG_UNIFORMS
{
    GLint Mpivot;
    GLint Mworld;
    GLint Mwvp;
    GLint Light[MAX_POINT_LIGHTS];
    GLint Count;
    GLint Kd[MAX_POINT_LIGHTS];
    GLint Ks[MAX_POINT_LIGHTS];
    GLint ViewPos;
};

//
// Globals
//
CMesh*                  g_pBunny;
CMesh*                  g_pSphere;

CBffFont*               g_pFont;
CBffFont*               g_pTitleFont;
CFraps*                 g_pFraps;
CBackground*            g_pBackground;

GLuint                  g_WvpAndColorProgram;
WVP_COLOR_UNIFORMS      g_WvpAndColorUniforms;
GLuint                  g_PhongProgram;
PHONG_UNIFORMS          g_PhongUniforms;

float                   g_ElapsedTime = 0.0f;
POINT_LIGHT_SOURCE      g_PointLights[MAX_POINT_LIGHTS];

float                   g_Distance = -3.3f;
float                   g_SpinX;
float                   g_SpinY;

XMMATRIX                g_View = XMMatrixIdentity();
XMMATRIX                g_Proj = XMMatrixIdentity();

GLuint                  g_LightCount = 2;

//
// LoadShaders
//
bool LoadShaders()
{
    puts("Load shaders");
    GLuint vsh, fsh;
    bool bLinked;

    //
    // Transform'n'Color program
    //
    vsh = LoadGLSLShader(GL_VERTEX_SHADER, g_pVshTransform);
    fsh = LoadGLSLShader(GL_FRAGMENT_SHADER, g_pFshFill);
    if ((0 == vsh) || (0 == fsh))
        return false;
    g_WvpAndColorProgram = glCreateProgram();
    glAttachShader(g_WvpAndColorProgram, vsh);
    glAttachShader(g_WvpAndColorProgram, fsh);
    glBindAttribLocation(g_WvpAndColorProgram, 0, "position");
    bLinked = LinkGLSLProgram(g_WvpAndColorProgram);
    glDeleteShader(fsh);
    glDeleteShader(vsh);
    if (!bLinked)
    {
        glDeleteProgram(g_WvpAndColorProgram);
        return false;
    }

    g_WvpAndColorUniforms.Mwvp = GetUniformLocation(g_WvpAndColorProgram, "Mwvp");
    g_WvpAndColorUniforms.FillColor = GetUniformLocation(g_WvpAndColorProgram, "color");

    //
    // Phong lighting program
    //
    vsh = LoadGLSLShader(GL_VERTEX_SHADER, g_pVshPhong);
    fsh = LoadGLSLShaderFromFile(GL_FRAGMENT_SHADER, "phong.psh");
    if ((0 == vsh) || (0 == fsh))
        return false;
    g_PhongProgram = glCreateProgram();
    glAttachShader(g_PhongProgram, vsh);
    glAttachShader(g_PhongProgram, fsh);
    glBindAttribLocation(g_PhongProgram, 0, "position");
    glBindAttribLocation(g_PhongProgram, 1, "normal");
    bLinked = LinkGLSLProgram(g_PhongProgram);
    glDeleteShader(fsh);
    glDeleteShader(vsh);
    if (!bLinked)
    {
        glDeleteProgram(g_PhongProgram);
        return false;
    }

    g_PhongUniforms.Mpivot = GetUniformLocation(g_PhongProgram, "Mpivot");
    g_PhongUniforms.Mworld = GetUniformLocation(g_PhongProgram, "Mworld");
    g_PhongUniforms.Mwvp = GetUniformLocation(g_PhongProgram, "Mwvp");
    
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        char name[256];

        sprintf(name, "light_and_radius[%d]", i);
        g_PhongUniforms.Light[i] = GetUniformLocation(g_PhongProgram, name);
        sprintf(name, "Kd[%d]", i);
        g_PhongUniforms.Kd[i] = GetUniformLocation(g_PhongProgram, name);
        sprintf(name, "Ks[%d]", i);
        g_PhongUniforms.Ks[i] = GetUniformLocation(g_PhongProgram, name);
    }

    g_PhongUniforms.Count = GetUniformLocation(g_PhongProgram, "count");
    g_PhongUniforms.ViewPos = GetUniformLocation(g_PhongProgram, "view");

    return true;
}

//
// SetupLights
//
void SetupLights()
{
    memset(g_PointLights, 0, sizeof(g_PointLights));

    POINT_LIGHT_SOURCE *l;

    //
    // Static
    //

    l = &g_PointLights[0];
    l->Center = XMFLOAT3(-1.0f, 1.0f, 0.0f);
    l->AttenuationRadius = 2.0f;
    l->DiffuseColor = XMFLOAT3(0.0f, 0.5f, 1.0f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = false;
    l->bClockwiseOrbit = false;
    l->OrbitRadius = 0.0f;
    l->OrbitRoll = 0.0f;
    l->Velocity = 0.0f;

    l = &g_PointLights[1];
    l->Center = XMFLOAT3(1.0f, 1.5f, 1.0f);
    l->AttenuationRadius = 5.0f;
    l->DiffuseColor = XMFLOAT3(0.75f, 0.75f, 0.75f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = false;
    l->bClockwiseOrbit = false;
    l->OrbitRadius = 0.0f;
    l->OrbitRoll = 0.0f;
    l->Velocity = 0.0f;

    l = &g_PointLights[2];
    l->Center = XMFLOAT3(0.5f, -1.0f, 1.0f);
    l->AttenuationRadius = 2.0f;
    l->DiffuseColor = XMFLOAT3(0.0f, 0.25f, 1.0f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = false;
    l->bClockwiseOrbit = false;
    l->OrbitRadius = 0.0f;
    l->OrbitRoll = 0.0f;
    l->Velocity = 0.0f;

    //
    // Moveable
    //

    l = &g_PointLights[3];
    l->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
    l->AttenuationRadius = 6.0f;
    l->DiffuseColor = XMFLOAT3(126.0f/255.0f, 237.0f/255.0f, 199.0f/255.0f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = true;
    l->bClockwiseOrbit = false;
    l->OrbitRadius = 2.5f;
    l->OrbitRoll = 0.0f;
    l->Velocity = 6.0f;

    l = &g_PointLights[4];
    l->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
    l->AttenuationRadius = 2.0f;
    l->DiffuseColor = XMFLOAT3(1.0f, 0.0f, 0.0f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = true;
    l->bClockwiseOrbit = false;
    l->OrbitRadius = 1.25f;
    l->OrbitRoll = 65.0f;
    l->Velocity = 3.0f;

    l = &g_PointLights[5];
    l->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
    l->AttenuationRadius = 2.0f;
    l->DiffuseColor = XMFLOAT3(0.0f, 0.8f, 0.7f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = true;
    l->bClockwiseOrbit = true;
    l->OrbitRadius = 1.5f;
    l->OrbitRoll = 120.0f;
    l->Velocity = 20.0f;

    l = &g_PointLights[6];
    l->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
    l->AttenuationRadius = 3.0f;
    l->DiffuseColor = XMFLOAT3(0.25f, 1.0f, 0.25f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = true;
    l->bClockwiseOrbit = false;
    l->OrbitRadius = 2.0f;
    l->OrbitRoll = 10.0f;
    l->Velocity = 8.0f;

    l = &g_PointLights[7];
    l->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
    l->AttenuationRadius = 3.0f;
    l->DiffuseColor = XMFLOAT3(0.0f, 0.0f, 1.0f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = true;
    l->bClockwiseOrbit = true;
    l->OrbitRadius = 2.0f;
    l->OrbitRoll = -20.0f;
    l->Velocity = 6.0f;
}

//
// Initialize
//
bool Initialize()
{
    puts("Initialize...");

#if !defined(USE_SDL) && !defined(USE_EGL)
    glewInit();
#endif

    g_pBunny = new CMesh((const VERTEX *)bunny::g_Vertices, sizeof(bunny::g_Vertices),
        bunny::g_Indices, sizeof(bunny::g_Indices) / sizeof(bunny::g_Indices[0]));

    g_pSphere = new CMesh((const VERTEX *)sphere::g_Vertices, sizeof(sphere::g_Vertices),
        sphere::g_Indices, sizeof(sphere::g_Indices) / sizeof(sphere::g_Indices[0]));

    g_pFont = new CBffFont("fixedsys.bff");
    g_pTitleFont = new CBffFont("arialn.bff");
    g_pTitleFont->SetScale(0.8f);
    g_pTitleFont->SetColor(XMFLOAT3(0.75f, 0.75f, 0.75f));
    g_pFraps = new CFraps();
    g_pBackground = new CBackground();

    bool bShaders = LoadShaders();
    assert(bShaders);

    SetupLights();

    // Setup render states once
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    return true;
}

//
// Cleanup
//
void Cleanup()
{
    SAFE_DELETE(g_pBackground);
    SAFE_DELETE(g_pFraps);
    SAFE_DELETE(g_pTitleFont);
    SAFE_DELETE(g_pFont);

    SAFE_DELETE(g_pSphere);
    SAFE_DELETE(g_pBunny);

    glDeleteProgram(g_PhongProgram);
    glDeleteProgram(g_WvpAndColorProgram);
}

//
// CalcLightPosition
//
void CalcLightPosition(POINT_LIGHT_SOURCE *pLight)
{
    if (pLight->bMoveable)
    {
        float Angle = (g_ElapsedTime / 100.0f * pLight->Velocity);

        if (pLight->bClockwiseOrbit)
            pLight->OrbitAngle -= Angle;
        else
            pLight->OrbitAngle += Angle; 

        float Rad = XMConvertToRadians(pLight->OrbitAngle);

        // Position on orbit
        pLight->Position = pLight->Center;
        pLight->Position.x += sinf(Rad) * pLight->OrbitRadius;
        pLight->Position.z += cosf(Rad) * pLight->OrbitRadius;

        XMMATRIX Roll = XMMatrixRotationZ(XMConvertToRadians(pLight->OrbitRoll));
        XMVECTOR V = XMLoadFloat3(&pLight->Position);
        
        // Orbit roll angle
        V = XMVector3Transform(V, Roll);
        XMStoreFloat3(&pLight->Position, V);
    }
    else
    {
        pLight->Position = pLight->Center;
    }
}

//
// UpdateBunnyConstantsVS
//
void UpdateBunnyConstantsVS()
{
    XMMATRIX SpinX = XMMatrixRotationX(XMConvertToRadians(g_SpinY));
    XMMATRIX SpinY = XMMatrixRotationY(XMConvertToRadians(g_SpinX));
    XMMATRIX Pivot = XMMatrixMultiply(SpinY, SpinX);
    XMMATRIX Trans = XMMatrixTranslation(0.0f, 0.1f, 0.0f); // Lift slightly up
    XMMATRIX World = XMMatrixMultiply(Pivot, Trans);
    XMMATRIX WorldView = XMMatrixMultiply(World, g_View);
    XMMATRIX WorldViewProj = XMMatrixMultiply(WorldView, g_Proj);
    
    glUniformMatrix4fv(g_PhongUniforms.Mpivot, 1, GL_FALSE, (const GLfloat *)&Pivot);
    glUniformMatrix4fv(g_PhongUniforms.Mworld, 1, GL_FALSE, (const GLfloat *)&World);
    glUniformMatrix4fv(g_PhongUniforms.Mwvp, 1, GL_FALSE, (const GLfloat *)&WorldViewProj);
}

//
// UpdateBunnyConstantsPS
//
void UpdateBunnyConstantsPS()
{
    for (int i = 0; i < g_LightCount; ++i)
    {
        const POINT_LIGHT_SOURCE *pLight = &g_PointLights[i];
        XMFLOAT4 LightAndRadius(
            pLight->Position.x,
            pLight->Position.y,
            pLight->Position.z,
            pLight->AttenuationRadius);

        glUniform4fv(g_PhongUniforms.Light[i], 1, (const GLfloat *)&LightAndRadius);
        glUniform3fv(g_PhongUniforms.Kd[i], 1, (const GLfloat *)&pLight->DiffuseColor);
        glUniform3fv(g_PhongUniforms.Ks[i], 1, (const GLfloat *)&pLight->SpecularColor);
    }

    glUniform1i(g_PhongUniforms.Count, g_LightCount);
    glUniform3f(g_PhongUniforms.ViewPos, 0.0f, 0.0f, -g_Distance);
}

//
// UpdateSphereConstantsVS
//
void UpdateSphereConstantsVS(XMFLOAT3 Position)
{
    // Sphere radius is 1 by default - too large for us, 
    // scale as appropriate 
    const float SCALE = 1.0f / 50.0f;
    const XMMATRIX Scale = XMMatrixScaling(SCALE, SCALE, SCALE);

    XMMATRIX Spin90 = XMMatrixRotationX(XMConvertToRadians(90.0f)); // Need to rotate here because of initial mesh rotation
    XMMATRIX Trans = XMMatrixTranslation(Position.x, Position.y, Position.z);
    XMMATRIX World = XMMatrixMultiply(Spin90, Trans);
    XMMATRIX ScaledWorld = XMMatrixMultiply(Scale, World);
    XMMATRIX WorldView = XMMatrixMultiply(ScaledWorld, g_View);
    XMMATRIX WorldViewProj = XMMatrixMultiply(WorldView, g_Proj);

    glUniformMatrix4fv(g_WvpAndColorUniforms.Mwvp, 1, GL_FALSE, (const GLfloat *)&WorldViewProj);
}

//
// UpdateKnotConstantsVS
//
void UpdateKnotConstantsVS(XMFLOAT3 Position)
{
    XMMATRIX World = XMMatrixTranslation(Position.x, Position.y, Position.z);
    XMMATRIX WorldView = XMMatrixMultiply(World, g_View);
    XMMATRIX WorldViewProj = XMMatrixMultiply(WorldView, g_Proj);

    glUniformMatrix4fv(g_WvpAndColorUniforms.Mwvp, 1, GL_FALSE, (const GLfloat *)&WorldViewProj);
}

//
// UpdateFillColor
//
void UpdateFillColor(XMFLOAT3 FillColor)
{
    glUniform3fv(g_WvpAndColorUniforms.FillColor, 1, (const GLfloat *)&FillColor);
}

//
// DrawBunny
// Creates fragment shader load with phong lighting
//
void DrawBunny()
{
    glUseProgram(g_PhongProgram);

    UpdateBunnyConstantsVS();
    UpdateBunnyConstantsPS();

    g_pBunny->Draw();
}

//
// DrawLights
//
void DrawLights()
{
    glUseProgram(g_WvpAndColorProgram);

    for (int i = 0; i < g_LightCount; ++i)
    {
        POINT_LIGHT_SOURCE *pLight = &g_PointLights[i];
         
        UpdateSphereConstantsVS(pLight->Position);
        UpdateFillColor(pLight->DiffuseColor);

        g_pSphere->Draw();
    }
}

//
// DrawHUD
//
void DrawHUD(unsigned Width, unsigned Height)
{
    g_pFraps->SetScreenSize(Width, Height);
    g_pFont->SetScreenSize(Width, Height);
    g_pTitleFont->SetScreenSize(Width, Height);

    static const char *pRenderer = NULL;
    static const char *pVendor = NULL;
    static const char *pVersion = NULL;
    static const char *pEGLVersion = NULL;

    if (!pRenderer)
    {
        // Call different glGet*() only once
        pRenderer = (const char *)glGetString(GL_RENDERER);
        pVendor = (const char *)glGetString(GL_VENDOR);
        pVersion = (const char *)glGetString(GL_VERSION);
#ifdef USE_EGL
        extern EGLDisplay g_Display;
        pEGLVersion = eglQueryString(g_Display, EGL_VERSION);
#endif
    }

    int x = 10;
    int y = Height - 25;

    g_pFont->SetColor(XMFLOAT3(0.0f, 1.0f, 1.0f));
    g_pFont->DrawString(x, y, "RENDERER: %s", pRenderer);
    y -= 20;
    g_pFont->DrawString(x, y, "VENDOR: %s", pVendor);
    y -= 20;
    g_pFont->DrawString(x, y, "VERSION: %s", pVersion);
#ifdef USE_EGL
    y -= 20;
    g_pFont->DrawString(x, y, "EGL VERSION: %s", pEGLVersion);
#endif
    y -= 60;

    extern GLuint g_MSAASamples;

    g_pFont->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
    g_pFont->DrawString(x, y, "FRAMEBUFFER RESOLUTION: %d x %d", Width, Height);
    y -= 20;
    g_pFont->DrawString(x, y, "MSAA LEVEL: %d", g_MSAASamples);
    y -= 60;

    unsigned VertexCount = sizeof(bunny::g_Vertices) / sizeof(bunny::g_Vertices[0]);
    unsigned PolyCount = sizeof(bunny::g_Indices) / sizeof(bunny::g_Indices[0]) / 3;

    g_pFont->SetColor(XMFLOAT3(0.0f, 1.0f, 0.0f));
    g_pFont->DrawString(x, y, "VERTEX COUNT: %d", VertexCount);
    y -= 20;
    g_pFont->DrawString(x, y, "POLYGON COUNT: %d", PolyCount);
    y -= 20;
    g_pFont->DrawString(x, y, "POINT LIGHTS: %d", g_LightCount);
    y -= 20;

    const char *pTitleString = "OPENGL ES 2.0 BENCHMARK";

    float w = g_pTitleFont->CalcStringWidth(pTitleString);
    g_pTitleFont->DrawString((int)((Width - w) / 2.0f), 0, pTitleString);

    g_pFraps->Draw();
}

//
// Render
//
void Render(unsigned Width, unsigned Height)
{
    g_ElapsedTime = GetElapsedMilliseconds();

    BeginFrame();

    glViewport(0, 0, Width, Height);
    glClear(GL_DEPTH_BUFFER_BIT); // No need to clear color buffer, because we draw background image
    
    g_View = XMMatrixTranslation(0.0f, 0.0f, g_Distance);
    g_Proj = XMMatrixPerspectiveFovRH(XMConvertToRadians(45.0f),
        Width / (float)Height, 0.1f, 100.0f);

    // Bunny rotation
    g_SpinX += g_ElapsedTime / 50.0f;

    // Setup light positions
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        POINT_LIGHT_SOURCE *pLight = &g_PointLights[i]; 
        CalcLightPosition(pLight);
    }

    g_pBackground->SetScreenSize(Width, Height);
    g_pBackground->Draw();

    DrawBunny();
    DrawLights();
    DrawHUD(Width, Height);

    EndFrame();

    g_pFraps->OnPresent();
}
