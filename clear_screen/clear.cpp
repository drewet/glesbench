#include "../hud/fraps.h"

void BeginFrame();
void EndFrame();

//
// Globals
//
CFraps*                 g_pFraps;

//
// Initialize
//
bool Initialize()
{
    puts("Initialize...");

#if !defined(USE_SDL) && !defined(USE_EGL)
    glewInit();
#endif

    g_pFraps = new CFraps();

    return true;
}

//
// Cleanup
//
void Cleanup()
{
    delete g_pFraps;
}

//
// Render
//
void Render(unsigned Width, unsigned Height)
{
    BeginFrame();

    glViewport(0, 0, Width, Height);
    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    g_pFraps->SetScreenSize(Width, Height);
    g_pFraps->Draw();

    EndFrame();

    g_pFraps->OnPresent();
}
