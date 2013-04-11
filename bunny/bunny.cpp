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
#include "../shader/shader.h"
#include "../hud/fraps.h"
#include "../hud/chart.h"
#include "../hud/bff.h"

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
    MAX_POINT_LIGHTS = 5//8
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
    GLint Radius[MAX_POINT_LIGHTS];
    GLint Kd[MAX_POINT_LIGHTS];
    GLint Ks[MAX_POINT_LIGHTS];
    GLint ViewPos;
};

//
// Globals
//
GLuint                  g_BunnyVB;
GLuint                  g_BunnyIB;
GLuint                  g_KnotVB;
GLuint                  g_KnotIB;
GLuint                  g_SphereVB;
GLuint                  g_SphereIB;

GLuint                  g_WvpAndColorProgram;
WVP_COLOR_UNIFORMS      g_WvpAndColorUniforms;
GLuint                  g_PhongProgram;
PHONG_UNIFORMS          g_PhongUniforms;

float                   g_ElapsedTime = 0.0f;
POINT_LIGHT_SOURCE      g_PointLights[MAX_POINT_LIGHTS];

CBffFont*               g_pFont;
CFraps*                 g_pFraps;
//CChart*                 g_pChart;

float                   g_Distance = -2.7f;
float                   g_SpinX;
float                   g_SpinY;

XMMATRIX                g_View = XMMatrixIdentity();
XMMATRIX                g_Proj = XMMatrixIdentity();

//
// LoadBunnyMesh
//
void LoadBunnyMesh()
{
    glGenBuffers(1, &g_BunnyVB);
    glBindBuffer(GL_ARRAY_BUFFER, g_BunnyVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bunny::g_Vertices), bunny::g_Vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &g_BunnyIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_BunnyIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bunny::g_Indices), bunny::g_Indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//
// LoadKnotMesh
//
void LoadKnotMesh()
{
    glGenBuffers(1, &g_KnotVB);
    glBindBuffer(GL_ARRAY_BUFFER, g_KnotVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(knot::g_Vertices), knot::g_Vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &g_KnotIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_KnotIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(knot::g_Indices), knot::g_Indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//
// LoadSphereMesh
//
void LoadSphereMesh()
{
    glGenBuffers(1, &g_SphereVB);
    glBindBuffer(GL_ARRAY_BUFFER, g_SphereVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere::g_Vertices), sphere::g_Vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &g_SphereIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_SphereIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere::g_Indices), sphere::g_Indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

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

        sprintf(name, "light[%d]", i);
        g_PhongUniforms.Light[i] = GetUniformLocation(g_PhongProgram, name);
        sprintf(name, "radius[%d]", i);
        g_PhongUniforms.Radius[i] = GetUniformLocation(g_PhongProgram, name);
        sprintf(name, "Kd[%d]", i);
        g_PhongUniforms.Kd[i] = GetUniformLocation(g_PhongProgram, name);
        sprintf(name, "Ks[%d]", i);
        g_PhongUniforms.Ks[i] = GetUniformLocation(g_PhongProgram, name);
    }

    g_PhongUniforms.ViewPos = GetUniformLocation(g_PhongProgram, "view");

    return true;
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

    LoadBunnyMesh();
    LoadKnotMesh();
    LoadSphereMesh();
    bool bShaders = LoadShaders();
    assert(bShaders);

    memset(g_PointLights, 0, sizeof(g_PointLights));

    POINT_LIGHT_SOURCE *l;

    //
    // Static
    //

    l = &g_PointLights[0];
    l->Center = XMFLOAT3(-1.0f, 1.0f, 0.0f);
    l->AttenuationRadius = 2.0f;
    l->DiffuseColor = XMFLOAT3(1.0f, 0.5f, 0.0f);
    l->SpecularColor = XMFLOAT3(l->DiffuseColor.x/2.0f, l->DiffuseColor.y/2.0f, l->DiffuseColor.z/2.0f);
    l->bMoveable = false;
    l->bClockwiseOrbit = false;
    l->OrbitRadius = 0.0f;
    l->OrbitRoll = 0.0f;
    l->Velocity = 0.0f;

    l = &g_PointLights[1];
    l->Center = XMFLOAT3(1.0f, 1.5f, 1.0f);
    l->AttenuationRadius = 5.0f;
    l->DiffuseColor = XMFLOAT3(0.0f, 0.5f, 0.25f);
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
/*
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
*/
    // Setup render states once
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    g_pFont = new CBffFont("fixedsys.bff");
    g_pFraps = new CFraps();
    //g_pChart = new CChart();
    //g_pChart->AddFpsValue(0.0f);

    return true;
}

//
// Cleanup
//
void Cleanup()
{
    //SAFE_DELETE(g_pChart);
    SAFE_DELETE(g_pFraps);
    SAFE_DELETE(g_pFont);

    glDeleteProgram(g_PhongProgram);
    glDeleteProgram(g_WvpAndColorProgram);

    glDeleteBuffers(1, &g_SphereIB);
    glDeleteBuffers(1, &g_SphereVB);
    glDeleteBuffers(1, &g_KnotIB);
    glDeleteBuffers(1, &g_KnotVB);
    glDeleteBuffers(1, &g_BunnyIB);
    glDeleteBuffers(1, &g_BunnyVB);
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
    XMMATRIX Trans = XMMatrixTranslation(0.0f, 0.15f, 0.0f); // Lift slightly up
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
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        const POINT_LIGHT_SOURCE *pLight = &g_PointLights[i];

        glUniform3fv(g_PhongUniforms.Light[i], 1, (const GLfloat *)&pLight->Position);
        glUniform1f(g_PhongUniforms.Radius[i], pLight->AttenuationRadius);
        glUniform3fv(g_PhongUniforms.Kd[i], 1, (const GLfloat *)&pLight->DiffuseColor);
        glUniform3fv(g_PhongUniforms.Ks[i], 1, (const GLfloat *)&pLight->SpecularColor);
    }

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
// DrawKnots
// Creates vertex shader load by drawing high-poly meshes
//
void DrawKnots()
{
    GLsizei Stride = 0; // Tightly packed
    GLsizei Count = sizeof(knot::g_Indices) / sizeof(knot::g_Indices[0]);

    glBindBuffer(GL_ARRAY_BUFFER, g_KnotVB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_KnotIB);

    glUseProgram(g_WvpAndColorProgram);
    
    const float Z = -4.0f;
    float Y;

    Y = -1.0f;
    for (int i = 0; i < 3; ++i)
    {
        UpdateKnotConstantsVS(XMFLOAT3(-5.0f, Y, Z));
        UpdateFillColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
        
        glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_SHORT, 0);
        Y += 1.0f;
    }

    Y = -1.0f;
    for (int i = 0; i < 3; ++i)
    {
        UpdateKnotConstantsVS(XMFLOAT3(5.0f, Y, Z));
        UpdateFillColor(XMFLOAT3(1.0f, 1.0f, 0.0f));

        glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_SHORT, 0);
        Y += 1.0f;
    }

    glDisableVertexAttribArray(0);
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

    GLsizei Stride = sizeof(GLfloat) * 6;
    GLsizei Count = sizeof(bunny::g_Indices) / sizeof(bunny::g_Indices[0]);

    glBindBuffer(GL_ARRAY_BUFFER, g_BunnyVB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0)); // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(sizeof(GL_FLOAT) * 3)); // Normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_BunnyIB);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

//
// DrawLights
//
void DrawLights()
{
    GLsizei Stride = 0; // Tightly packed
    GLsizei Count = sizeof(sphere::g_Indices) / sizeof(sphere::g_Indices[0]);

    glBindBuffer(GL_ARRAY_BUFFER, g_SphereVB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_SphereIB);

    glUseProgram(g_WvpAndColorProgram);

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        POINT_LIGHT_SOURCE *pLight = &g_PointLights[i];
         
        UpdateSphereConstantsVS(pLight->Position);
        UpdateFillColor(pLight->DiffuseColor);

        glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_SHORT, 0);
    }

    glDisableVertexAttribArray(0);
}

//
// DrawHUD
//
void DrawHUD(unsigned Width, unsigned Height)
{
#ifdef USE_EGL
    extern EGLDisplay g_Display;
#endif
    extern GLuint g_MSAASamples;

    g_pFraps->SetScreenSize(Width, Height);
    g_pFraps->Draw();

    g_pFont->SetScreenSize(Width, Height);

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
    g_pFont->DrawString(x, y, "POINT LIGHTS: %d", MAX_POINT_LIGHTS);
    y -= 20;

    g_pFont->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
    g_pFont->DrawString(Width - 190, 0, "OPENGL ES 2.0 BENCHMARK");
}

//
// Render
//
void Render(unsigned Width, unsigned Height)
{
    g_ElapsedTime = GetElapsedMilliseconds();

    BeginFrame();

    glViewport(0, 0, Width, Height);
    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    g_View = XMMatrixTranslation(0.0f, 0.0f, g_Distance);
    g_Proj = XMMatrixPerspectiveFovRH(XMConvertToRadians(60.0f),
        Width / (float)Height, 0.1f, 100.0f);

    // Bunny rotation
    g_SpinX += g_ElapsedTime / 50.0f;

    // Setup light positions
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        POINT_LIGHT_SOURCE *pLight = &g_PointLights[i]; 
        CalcLightPosition(pLight);
    }

    DrawBunny();
    DrawLights();
    DrawHUD(Width, Height);

    EndFrame();

    g_pFraps->OnPresent();
}
