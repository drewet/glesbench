#include <stdio.h>
#include <sys/timeb.h>

#include "../ogl_es.h"

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
    static timeb Prev, Curr;

    if (bFirst)
    {
        ftime(&Prev);
        bFirst = false;
        return 0.0f;
    }

    ftime(&Curr);

    float ElapsedTime;

    ElapsedTime = (Curr.time - Prev.time) * 1000.0f;
    ElapsedTime += (float)(Curr.millitm - Prev.millitm);

    if (ElapsedTime > 0.0f)
        Prev = Curr;

    return ElapsedTime;
}
