#ifdef _WIN32
#include <windows.h>
#endif

#ifdef USE_SDL // Define this for SDL build
#include <GLES2/gl2.h>
typedef char GLchar; // Some GLES headers do not typedef this
#else
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glx.h>
#endif // _WIN32
#endif // USE_SDL
