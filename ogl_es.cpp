#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <unistd.h>

#include <vector>

#include <X11/Xlib.h>

#include <GL/glew.h>
#include <GL/glx.h>

//
// Globals
//
Display*            g_Display               = NULL;
Window              g_Window                = 0;

XVisualInfo*        g_Visual                = NULL;

GLXContext          g_Context               = 0;
GLXFBConfig*        g_FBConfig              = NULL;      // GLX 1.3
GLXWindow           g_Drawable              = 0;         // GLX 1.3

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
// PrintDesc
//
static void PrintDesc()
{
    printf("GL_RENDERER: %s\n", (const char *)glGetString(GL_RENDERER));
    printf("GL_VERSION: %s\n", (const char *)glGetString(GL_VERSION));
    printf("GL_VENDOR: %s\n", (const char *)glGetString(GL_VENDOR));
    //printf("GL_EXTENSIONS: %s\n", (const char *)glGetString(GL_EXTENSIONS));
}

//
// CreateOpenGL
//
bool CreateOpenGL(unsigned Width, unsigned Height)
{
    g_Display = XOpenDisplay(NULL);
    if (!g_Display)
    {
        puts("Failed to open display.");
        return false;
    }

    int Error;
    int Event;

    glXQueryExtension(g_Display, &Error, &Event);

    int MajorNumber;
    int MinorNumber;

    glXQueryVersion(g_Display, &MajorNumber, &MinorNumber);
    printf("GLX version: %d.%d\n", MajorNumber, MinorNumber);

    bool bNewGLX = false;//true;
    std::vector<int> Attribs;

    if (!bNewGLX)
    {
        Attribs.push_back(GLX_USE_GL);
        Attribs.push_back(GLX_RGBA);
    }
    else
    {
        Attribs.push_back(GLX_DRAWABLE_TYPE);
        Attribs.push_back(GLX_WINDOW_BIT);
        Attribs.push_back(GLX_RENDER_TYPE);
        Attribs.push_back(GLX_RGBA_BIT);
        Attribs.push_back(GLX_X_RENDERABLE);
        Attribs.push_back(GLX_DONT_CARE);
    }
    Attribs.push_back(GLX_LEVEL);
    Attribs.push_back(0);
    Attribs.push_back(GLX_DOUBLEBUFFER);
    if (bNewGLX)
        Attribs.push_back(GL_TRUE);

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
    Attribs.push_back(0);

    puts("Choose visual...");
    if (bNewGLX)
    {
        int NumConfigs = 0;

        g_FBConfig = glXChooseFBConfig(g_Display, DefaultScreen(g_Display), &Attribs[0], &NumConfigs);
        if (!g_FBConfig)
        {
            puts("No conforming FB config exists.");
            return false;
        }
    }
    else
    {
        g_Visual = glXChooseVisual(g_Display, DefaultScreen(g_Display), &Attribs[0] );
        if (!g_Visual)
        {
            puts("No conforming visual exists.");
            return false;
        }
    }

    if (bNewGLX)
    {
        XVisualInfo *Visual = glXGetVisualFromFBConfig(g_Display, g_FBConfig[0]);
        if (!Visual)
        {
            puts("Failed to obtain visual from FB config.");
            return false;
        }

        g_Window = CreateAppWindow(Visual, Width, Height);

        g_Drawable = glXCreateWindow(g_Display, g_FBConfig[0], g_Window, NULL);
        if (!g_Drawable)
        {
            puts("Failed to create drawable." );
            return false;
        }

        g_Context = glXCreateNewContext(g_Display, g_FBConfig[0], GLX_RGBA_TYPE, NULL, True);
        if (!g_Context)
        {
            puts("Failed to create direct rendering context.");
            return false;
        }
    }
    else
    {
        g_Context = glXCreateContext(g_Display, g_Visual, NULL, True);
        if (!g_Context)
        {
            puts("Failed to create direct rendering context." );
            return false;
        }

        g_Window = CreateAppWindow(g_Visual, Width, Height);
    }

    printf("glXMakeCurrent(%p, %d, %p)\n", g_Display, (unsigned)g_Window, g_Context);
    if (!glXMakeCurrent(g_Display, g_Window, g_Context))
    {
        puts("Failed to make rendering context current.");
        return false;
    }

    PrintDesc();
    return true;
}

//
// DestroyOpenGL
//
void DestroyOpenGL()
{
    glXDestroyContext(g_Display, g_Context);
    //if (bNewGLX)
        //glXDestroyWindow(g_Display, g_Drawable);

    XDestroyWindow(g_Display, g_Window);

    if (g_Visual)
        XFree(g_Visual);
    else if (g_FBConfig)
        XFree(g_FBConfig);
}

//
// BeginFrame
//
void BeginFrame()
{
    glXMakeCurrent(g_Display, g_Window, g_Context);
}

//
// EndFrame
//
void EndFrame()
{
    glXSwapBuffers(g_Display, g_Window);
}
