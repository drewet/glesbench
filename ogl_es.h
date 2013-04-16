#ifdef _WIN32
#include <windows.h>
#endif

#ifdef USE_EGL
// LG TV
#   include <GLES2/gl2.h>
#   include <EGL/egl.h>
#elif defined(USE_SDL)
// HT Touchpad
#   include <GLES2/gl2.h>
typedef char GLchar; // Some GLES headers do not typedef this
#else // X11
// Desktop Linux
#   include <GL/glew.h>
#   include <GL/glx.h>
#endif

#include <stdlib.h> // for NULL

#define ATTRIB_OFFSET(n) \
    (GLvoid *)((char *)NULL + (n))
