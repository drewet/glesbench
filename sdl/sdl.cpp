#include "SDL.h"
#include "PDL.h"

#define USE_PDL

//
// Globals
//
SDL_Surface*        g_pSurface;

//
// CreateSDL
//
bool CreateSDL(unsigned Width, unsigned Height)
{
    puts("Creating SDL...");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
#ifdef USE_PDL
    PDL_Init(0);
#endif

    // Tell it to use OpenGL version 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

    // Set the video mode to full screen with OpenGL-ES support
    // use zero for width/height to use maximum resolution
    g_pSurface = SDL_SetVideoMode(Width, Height, 0, SDL_OPENGL);

    /*
    PDL_Err err;
    char buffer[256];

    int pdkVersion = PDL_GetPDKVersion();
    printf("PDL_GetPDKVersion: %d\n", pdkVersion);

    if (pdkVersion >= 200)
        printf("PDL_GetHardwareID: %d\n", PDL_GetHardwareID());

    PDL_OSVersion osVersion;

    if (PDL_NOERROR == PDL_GetOSVersion(&osVersion))
    {
        printf("PDL_GetOSVersion: %s (%d.%d.%d)\n",
            osVersion.versionStr,
            osVersion.majorVersion,
            osVersion.minorVersion,
            osVersion.revision);
    }

    PDL_ScreenMetrics metrics;

    if (PDL_NOERROR == PDL_GetScreenMetrics(&metrics))
    {
        printf("PDL_GetScreenMetrics: %d x %d, DPI %d x %d, Aspect Ratio: %f\n",
            metrics.horizontalPixels,
            metrics.verticalPixels,
            metrics.horizontalDPI,
            metrics.verticalDPI,
            metrics.aspectRatio);
    }

    if (PDL_NOERROR == PDL_GetUniqueID(buffer, 256))
        printf("PDL_GetUniqueID: %s\n", buffer);

    if (PDL_NOERROR == PDL_GetDeviceName(buffer, 256))
        printf("PDL_GetDeviceName: %s\n", buffer);

    if (PDL_NOERROR == PDL_GetLanguage(buffer, 256))
        printf("PDL_GetLanguage: %s\n", buffer);

    if (pdkVersion >= 200)
    {
        if (PDL_NOERROR == PDL_GetRegionCountryName(buffer, 256))
            printf("PDL_GetRegionCountryName: %s\n", buffer);

        if (PDL_NOERROR == PDL_GetRegionCountryCode(buffer, 256))
            printf("PDL_GetRegionCountryCode: %s\n", buffer);
    }
    */
}

//
// DestroySDL
//
void DestroySDL()
{
#ifdef USE_PDL
    PDL_Quit();
#endif
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
}
