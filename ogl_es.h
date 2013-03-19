#define USE_GLES

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef USE_GLES
#include <GLES2/gl2.h>
typedef char GLchar; // Some GLES headers do not typedef this
#else
#include <GL/glew.h>
#include <GL/glx.h>
#endif
