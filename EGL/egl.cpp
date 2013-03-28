#include <stdlib.h>
#include <stdio.h>

#include "../ogl_es.h"

const char *StringifyEGLError(EGLint Error);
void PrintExtensions(const char *pExtensions);

//
// Globals
//
EGLDisplay          g_Display               = NULL;
EGLSurface          g_Surface               = NULL;
EGLContext          g_Context               = NULL;

//
// CreateOpenGL
//
bool CreateOpenGL(int *pWidth, int *pHeight)
{
    g_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EGL_NO_DISPLAY == g_Display)
    {
        puts("Failed to retrieve EGL display connection.");
        return false;
    }

    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint Error = eglGetError();
    if (Error != EGL_SUCCESS) {
        printf("eglBindAPI(EGL_OPENGL_ES_API) failed: %s\n", StringifyEGLError(Error));
        return false;
    }

    EGLint MajorNumber = 0;
    EGLint MinorNumber = 0;

    if (!eglInitialize(g_Display, &MajorNumber, &MinorNumber))
    {
        puts("eglInitialize failed.");
        return false;
    }

    printf("EGL version: %d.%d\n", MajorNumber, MinorNumber);

    const EGLint Attribs[] =
    {
        EGL_RED_SIZE,               8,
        EGL_GREEN_SIZE,             8,
        EGL_BLUE_SIZE,              8,
        EGL_ALPHA_SIZE,             8,
        EGL_DEPTH_SIZE,             24,
        EGL_STENCIL_SIZE,           8,
        EGL_SURFACE_TYPE,           EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,        EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLConfig Config;
    EGLint NumConfigs = 0;

    if (!eglChooseConfig(g_Display, Attribs, &Config, 1, &NumConfigs) || (0 == NumConfigs))
    {
        puts("eglChooseConfig failed.");
        return false;
    }

    printf("Number of framebuffer configurations that matches criteria: %d\n", NumConfigs);

    g_Surface = eglCreateWindowSurface(g_Display, Config, (NativeWindowType)0, NULL);
    if (EGL_NO_SURFACE == g_Surface)
    {
        puts("eglCreateWindowSurface() failed.");
        return false;
    }

    EGLint Width = 0;
    EGLint Height = 0;
    EGLint HorizontalResolution = 0;
    EGLint VerticalResolution = 0;

    eglQuerySurface(g_Display, g_Surface, EGL_WIDTH, &Width);
    eglQuerySurface(g_Display, g_Surface, EGL_HEIGHT, &Height);
    eglQuerySurface(g_Display, g_Surface, EGL_VERTICAL_RESOLUTION, &VerticalResolution);
    eglQuerySurface(g_Display, g_Surface, EGL_HORIZONTAL_RESOLUTION, &HorizontalResolution);

    printf("Width: %d\nHeight: %d\nVRes: %d\nHRes: %d\n", Width, Height, VerticalResolution, HorizontalResolution);
    *pWidth = Width;
    *pHeight = Height;

    const EGLint ContextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    g_Context = eglCreateContext(g_Display, Config, EGL_NO_CONTEXT, ContextAttribs);
    if (EGL_NO_CONTEXT == g_Context)
    {
        EGLint Error = eglGetError();
        printf("eglCreateContext failed: %s\n", StringifyEGLError(Error));
        return false;
    }

    if (!eglMakeCurrent(g_Display, g_Surface, g_Surface, g_Context))
    {
        puts("Failed to make rendering context current.");
        return false;
    }

    printf("EGL_VENDOR: %s\n", eglQueryString(g_Display, EGL_VENDOR));
    printf("EGL_VERSION: %s\n", eglQueryString(g_Display, EGL_VERSION));
    PrintExtensions(eglQueryString(g_Display, EGL_EXTENSIONS));
    PrintExtensions((const char *)glGetString(GL_EXTENSIONS));

    return true;
}

//
// DestroyOpenGL
//
void DestroyOpenGL()
{
    eglMakeCurrent(g_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(g_Display, g_Surface);
    eglDestroyContext(g_Display, g_Context);
    eglTerminate(g_Display);
}

//
// BeginFrame
//
void BeginFrame()
{
    eglMakeCurrent(g_Display, g_Surface, g_Surface, g_Context);
}

//
// EndFrame
//
void EndFrame()
{
    eglSwapBuffers(g_Display, g_Surface);
}
