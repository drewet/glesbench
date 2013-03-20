#ifndef __SHADER_H__
#define __SHADER_H__

#include "../ogl_es.h"

GLuint LoadGLSLShader(GLenum Target, const GLchar *pSource);
GLuint LoadGLSLShaderFromFile(GLenum Target, const char *pFileName);
bool LinkGLSLProgram(GLuint Program);
GLint GetUniformLocation(GLuint Program, const GLchar *pName);

#endif // __SHADER_H__
