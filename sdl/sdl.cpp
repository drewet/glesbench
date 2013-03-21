#include "SDL.h"

//
// Globals
//
SDL_Surface*        g_pSurface;

//
// CreateSDL
// Note: PDL is needed for x86 SDL implementation (otherwise OpenGL context will not be created)
// PDL doesn't needed on touchpad
//
bool CreateSDL(unsigned Width, unsigned Height)
{
    puts("Creating SDL...");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

#ifdef LG_TV
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#else
    // This is from Palm PDK:
    // Tell it to use OpenGL version 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
#endif

    // Set the video mode to full screen with OpenGL-ES support
    // use zero for width/height to use maximum resolution
    g_pSurface = SDL_SetVideoMode(Width, Height, 0, SDL_OPENGL);

    return true;
}

//
// DestroySDL
//
void DestroySDL()
{
    SDL_Quit();
}

//
// BeginFrame
//
void BeginFrame()
{
    // Do nothing
}

//
// EndFrame
//
void EndFrame()
{
    SDL_GL_SwapBuffers();

    static unsigned Count = 0;

    if (++Count % 200 == 0)
        printf("Heartbeat : %d frames passed...\n", Count);
}
