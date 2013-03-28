#include <stdio.h>
#include <string.h>

#include "ogl_es.h"

#ifdef USE_SDL
#include "SDL.h"
#else
#include <sys/timeb.h>
#endif

#define COMPILETIMESTRING(x) return #x;

//
// GetElapsedMilliseconds
//
float GetElapsedMilliseconds()
{
    static bool bFirst = true;
    float ElapsedTime;
#ifdef USE_SDL
    static Uint32 Prev, Curr;

    if (bFirst)
    {
        Prev = SDL_GetTicks();
        bFirst = false;
        return 0.0f;
    }

    Curr = SDL_GetTicks();

    ElapsedTime = (float)(Curr - Prev);
    if (ElapsedTime > 0.0f)
        Prev = Curr;
#else
    static timeb Prev, Curr;

    if (bFirst)
    {
        ftime(&Prev);
        bFirst = false;
        return 0.0f;
    }

    ftime(&Curr);

    ElapsedTime = (Curr.time - Prev.time) * 1000.0f;
    ElapsedTime += (float)(Curr.millitm - Prev.millitm);
    if (ElapsedTime > 0.0f)
        Prev = Curr;
#endif // !USE_SDL

    return ElapsedTime;
}

//
// PrintExtensions
//
void PrintExtensions(const char *pExtensions)
{
    char *s = new char[strlen(pExtensions) + 1];

    if (s)
    {
        const char *tok;

        strcpy(s, pExtensions);
        tok = strtok(s, " ");
        while (tok)
        {
            puts(tok);
            tok = strtok(NULL, " ");
        }
    }

    delete[] s;
}

//
// StringifyGLError
//
const char *StringifyGLError(GLenum Error)
{
    switch (Error)
    {
    COMPILETIMESTRING(GL_NO_ERROR)
    COMPILETIMESTRING(GL_INVALID_ENUM)
    COMPILETIMESTRING(GL_INVALID_VALUE)
    COMPILETIMESTRING(GL_INVALID_OPERATION)
    COMPILETIMESTRING(GL_STACK_OVERFLOW)
    COMPILETIMESTRING(GL_STACK_UNDERFLOW)
    COMPILETIMESTRING(GL_OUT_OF_MEMORY)
    }

    return "n/a";
}

#ifdef __egl_h_

//
// StringifyEGLError
//
const char *StringifyEGLError(EGLint Error)
{
    switch (Error)
    {
    COMPILETIMESTRING(EGL_SUCCESS)
    COMPILETIMESTRING(EGL_NOT_INITIALIZED)
    COMPILETIMESTRING(EGL_BAD_ACCESS)
    COMPILETIMESTRING(EGL_BAD_ALLOC)
    COMPILETIMESTRING(EGL_BAD_ATTRIBUTE)
    COMPILETIMESTRING(EGL_BAD_CONFIG)
    COMPILETIMESTRING(EGL_BAD_CONTEXT)
    COMPILETIMESTRING(EGL_BAD_CURRENT_SURFACE)
    COMPILETIMESTRING(EGL_BAD_DISPLAY)
    COMPILETIMESTRING(EGL_BAD_MATCH)
    COMPILETIMESTRING(EGL_BAD_NATIVE_PIXMAP)
    COMPILETIMESTRING(EGL_BAD_NATIVE_WINDOW)
    COMPILETIMESTRING(EGL_BAD_PARAMETER)
    COMPILETIMESTRING(EGL_BAD_SURFACE)
    COMPILETIMESTRING(EGL_CONTEXT_LOST)
    }

    return "n/a";
}

#endif // __egl_h_
