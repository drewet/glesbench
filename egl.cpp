#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <unistd.h>

#include "ogl_es.h"

//
// Globals
//
EGLNativeDisplayType    g_Display;
EGLNativeWindowType     g_Window;
EGLDisplay              g_EglDisplay;
EGLSurface              g_EglSurface;
EGLContext              g_EglContext;

//
// CreateAppWindow
//
static Window CreateAppWindow(const XVisualInfo *Visual, unsigned Width, unsigned Height)
{
    Colormap cm;
    XSetWindowAttributes Attribs;

    cm = XCreateColormap(
        g_Display,
        RootWindow(g_Display, Visual->screen),
        Visual->visual,
        AllocNone
        );

    memset(&Attribs, 0, sizeof(XSetWindowAttributes));
    Attribs.colormap = cm;
    Attribs.event_mask = (KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask);

    Window Wnd = XCreateWindow(
        g_Display,
        RootWindow(g_Display, Visual->screen),
        0, 0,
        Width, Height,
        0,
        Visual->depth,
        InputOutput,
        Visual->visual,
        CWColormap | CWEventMask,
        &Attribs
        );

    return Wnd;
}

//
// CreateOpenGL
//
bool CreateOpenGL(unsigned Width, unsigned Height)
{
    puts("XOpenDisplay...");
    g_Display = (EGLNativeDisplayType)XOpenDisplay(NULL);
    if (!g_Display)
    {
        puts("Failed to open display.");
        return false;
    }

    puts("eglGetDisplay...");
    g_EglDisplay = eglGetDisplay(g_Display);
    if (EGL_NO_DISPLAY == g_EglDisplay)
    {
        puts("Failed to get EGL display.");
        return false;
    }

    EGLint MajorNumber = 0;
    EGLint MinorNumber = 0;

    // Initialize the display
    puts("eglInitialize...");
    EGLBoolean bSuccess = eglInitialize(g_EglDisplay, &MajorNumber, &MinorNumber);
    if (!bSuccess)
    {
        puts("eglInitialize() failed!\n");
        return false;
    }

    printf("EGL version: %d.%d\n", MajorNumber, MinorNumber);

    // Obtain the first configuration with a depth buffer
    EGLint Attribs[] =
    {
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };

    EGLint NumConfig = 0;
    EGLConfig EglConfig = 0;

    puts("eglChooseConfig...");
    bSuccess = eglChooseConfig(g_EglDisplay, Attribs, &EglConfig, 1, &NumConfig);
    if (!bSuccess)
    {
        puts("Failed to find valid EGL config.");
        return false;
    }

    // Get the native visual id
    int NativeVisualID;

    puts("eglGetConfigAttrib...");
    if (!eglGetConfigAttrib(g_EglDisplay, EglConfig, EGL_NATIVE_VISUAL_ID, &NativeVisualID))
    {
        puts("Failed to get EGL_NATIVE_VISUAL_ID.");
        return false;
    }

    XVisualInfo Template;
    XVisualInfo *pVisual = NULL;
    int NumItems;

    // Construct visual info from id
    Template.visualid = NativeVisualID;
    puts("XGetVisualInfo...");
    pVisual = XGetVisualInfo(g_Display, VisualIDMask, &Template, &NumItems);
    if (!pVisual)
        return false;

    g_Window = CreateAppWindow(pVisual, Width, Height);

    puts("eglCreateWindowSurface...");
    g_EglSurface = eglCreateWindowSurface(g_EglDisplay, EglConfig, g_Window, NULL);
    if (EGL_NO_SURFACE == g_EglSurface)
    {
        puts("Failed to create window surface.");
        return false;
    }

    puts("eglCreateContext...");
    g_EglContext = eglCreateContext(g_EglDisplay, EglConfig, EGL_NO_CONTEXT, NULL);
    if (EGL_NO_CONTEXT == g_EglContext)
    {
        puts("Failed to create EGL context.");
        return false;
    }

    printf("eglMakeCurrent(%p, %p, %p, %p)\n", g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);
    bSuccess = eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);
    if(!bSuccess)
    {
        puts("Failed to make rendering context current.");
        return false;
    }

    return true;
}

//
// DestroyOpenGL
//
void DestroyOpenGL()
{
    eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(g_EglDisplay, g_EglContext);
    eglDestroySurface(g_EglDisplay, g_EglSurface);
    eglTerminate(g_EglDisplay);

    XDestroyWindow(g_Display, g_Window);
}

//
// BeginFrame
//
void BeginFrame()
{
    eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);
}

//
// EndFrame
//
void EndFrame()
{
    eglSwapBuffers(g_EglDisplay, g_EglSurface);
}
