#include <GL/glew.h>

void BeginFrame();
void EndFrame();

//
// Initialize
//
bool Initialize()
{
    return true;
}

//
// Cleanup
//
void Cleanup()
{
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

    EndFrame();
}
