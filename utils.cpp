#include <stdio.h>
#include <string.h>

#include "ogl_es.h"

#ifdef USE_SDL
#include "SDL.h"
#else
#include <sys/timeb.h>
#endif

//
// glHandleErrors
//
void glHandleErrors(const char *pStr)
{
    const char *Codes[] =
    {
        "GL_INVALID_ENUM",
        "GL_INVALID_VALUE",
        "GL_INVALID_OPERATION",
        "GL_STACK_OVERFLOW",
        "GL_STACK_UNDERFLOW",
        "GL_OUT_OF_MEMORY"
    };

    GLenum Error = glGetError();
    if (Error != GL_NO_ERROR)
        printf("GL error: %s at line %s\n", Codes[Error - 0x0500], pStr);
}

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
