#ifdef _WIN32
#include <windows.h>
#endif

#if defined(LG_TV)
#   include <GLES2/gl2.h>
#   include <EGL/egl.h>
#elif defined(USE_SDL)
#   include <GLES2/gl2.h>
typedef char GLchar; // Some GLES headers do not typedef this
#else
#   include <GL/glew.h>
#   include <GL/glx.h>
#endif
