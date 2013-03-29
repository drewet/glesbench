#include <stdio.h>
#include <memory.h>

#include <vector>

#include <X11/Xlib.h>
#include <GL/glx.h>

void PrintExtensions(const char *pExtensions);

//
// Globals
//
Display*            g_pDisplay              = NULL;
Window              g_Window                = 0;

XVisualInfo*        g_pVisual               = NULL;

GLXContext          g_Context               = 0;
//GLXFBConfig*        g_pFBConfig             = NULL;      // GLX 1.3
//GLXWindow           g_Drawable              = 0;         // GLX 1.3

//
// CreateAppWindow
//
static Window CreateAppWindow(const XVisualInfo *Visual, unsigned Width, unsigned Height)
{
    Colormap cm;
    XSetWindowAttributes Attribs;

    cm = XCreateColormap(
        g_pDisplay,
        RootWindow(g_pDisplay, Visual->screen),
        Visual->visual,
        AllocNone);

    memset(&Attribs, 0, sizeof(XSetWindowAttributes));
    Attribs.colormap = cm;
    Attribs.event_mask = (KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask);

    Window Wnd = XCreateWindow(
        g_pDisplay,
        RootWindow(g_pDisplay, Visual->screen),
        0, 0,
        Width, Height,
        0,
        Visual->depth,
        InputOutput,
        Visual->visual,
        CWColormap | CWEventMask,
        &Attribs);

    return Wnd;
}

//
// CreateOpenGL
//
bool CreateOpenGL(unsigned Width, unsigned Height, int MSAASamples)
{
    g_pDisplay = XOpenDisplay(NULL);
    if (!g_pDisplay)
    {
        puts("Failed to open display.");
        return false;
    }

    int Error;
    int Event;

    glXQueryExtension(g_pDisplay, &Error, &Event);

    int MajorNumber;
    int MinorNumber;

    glXQueryVersion(g_pDisplay, &MajorNumber, &MinorNumber);
    printf("GLX version: %d.%d\n", MajorNumber, MinorNumber);

    std::vector<int> Attribs;

    Attribs.push_back(GLX_USE_GL);
    Attribs.push_back(GLX_RGBA);
    Attribs.push_back(GLX_LEVEL);
    Attribs.push_back(0);
    Attribs.push_back(GLX_DOUBLEBUFFER);

    Attribs.push_back(GLX_AUX_BUFFERS);
    Attribs.push_back(0);
    Attribs.push_back(GLX_BUFFER_SIZE);
    Attribs.push_back(32);
    Attribs.push_back(GLX_RED_SIZE);
    Attribs.push_back(8);
    Attribs.push_back(GLX_GREEN_SIZE);
    Attribs.push_back(8);
    Attribs.push_back(GLX_BLUE_SIZE);
    Attribs.push_back(8);
    Attribs.push_back(GLX_ALPHA_SIZE);
    Attribs.push_back(8);
    Attribs.push_back(GLX_DEPTH_SIZE);
    Attribs.push_back(24);
    Attribs.push_back(GLX_STENCIL_SIZE);
    Attribs.push_back(8);
    if (MSAASamples > 1)
    {
        puts("Requesting MSAA config...");
        Attribs.push_back(GLX_SAMPLE_BUFFERS);
        Attribs.push_back(1);
        Attribs.push_back(GLX_SAMPLES);
        Attribs.push_back(MSAASamples);
    }
    Attribs.push_back(0);

    g_pVisual = glXChooseVisual(g_pDisplay, DefaultScreen(g_pDisplay), &Attribs[0]);
    if (!g_pVisual)
    {
        puts("No conforming visual exists.");
        return false;
    }

    g_Context = glXCreateContext(g_pDisplay, g_pVisual, NULL, True);
    if (!g_Context)
    {
        puts("Failed to create direct rendering context." );
        return false;
    }

    g_Window = CreateAppWindow(g_pVisual, Width, Height);

    if (!glXMakeCurrent(g_pDisplay, g_Window, g_Context))
    {
        puts("Failed to make rendering context current.");
        return false;
    }

    if (MSAASamples > 1)
    {
        glEnable(GL_MULTISAMPLE);
    }

    printf("\n");
    printf("GL_RENDERER: %s\n", (const char *)glGetString(GL_RENDERER));
    printf("GL_VERSION: %s\n", (const char *)glGetString(GL_VERSION));
    printf("GL_VENDOR: %s\n", (const char *)glGetString(GL_VENDOR));
    PrintExtensions((const char *)glGetString(GL_EXTENSIONS));

    return true;
}

//
// DestroyOpenGL
//
void DestroyOpenGL()
{
    glXDestroyContext(g_pDisplay, g_Context);

    XDestroyWindow(g_pDisplay, g_Window);

    if (g_pVisual)
        XFree(g_pVisual);
    /*
    else if (g_FBConfig)
        XFree(g_FBConfig);
    */
}

//
// BeginFrame
//
void BeginFrame()
{
    glXMakeCurrent(g_pDisplay, g_Window, g_Context);
}

//
// EndFrame
//
void EndFrame()
{
    glXSwapBuffers(g_pDisplay, g_Window);

    static unsigned Count = 0;

    if (++Count % 200 == 0)
        printf("Heartbeat : %d frames passed...\n", Count);
}
