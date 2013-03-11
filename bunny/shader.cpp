#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>

//
// LoadGLSLShader
//
GLuint LoadGLSLShader(GLenum Target, const GLchar *pSource)
{
    GLint Length = (GLint)strlen(pSource);
    GLuint Shader;
    GLint CompileStatus;

    Shader = glCreateShader(Target);
    glShaderSource(Shader, 1, &pSource, &Length);
    glCompileShader(Shader);  
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
    
    if (CompileStatus != GL_TRUE)
    {
        GLint InfoLogLength = 0;

        puts("Failed to compile shader");
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 1)
        {
            GLchar *pInfoLog = new GLchar[InfoLogLength + 1];
            if (pInfoLog)
            {
                glGetShaderInfoLog(Shader, InfoLogLength, NULL, pInfoLog);
                puts(pInfoLog);
                delete[] pInfoLog;
            }
        }

        glDeleteShader(Shader);
        return 0;
    }

    return Shader;
}

//
// LoadGLSLShaderFromFile
//
GLuint LoadGLSLShaderFromFile(GLenum Target, const char *pFileName)
{
    FILE *pFile = fopen(pFileName, "rb");
    
    if (pFile)
    {
	    fseek(pFile, 0L, SEEK_END);
	    long Size = ftell(pFile);
        
        if (Size > 0)
        {
            GLchar *pSource = (GLchar *)calloc(Size + 1, 1);
            GLuint Shader = 0;

	        fseek(pFile, 0, SEEK_SET);
            fread(pSource, 1, Size, pFile);
            fclose(pFile);

            Shader = LoadGLSLShader(Target, pSource);
            free(pSource);

            return Shader;
        }

        fclose(pFile);
    }

    return 0;
}

//
// LinkGLSLProgram
//
bool LinkGLSLProgram(GLuint Program)
{
    GLint LinkStatus = GL_FALSE;

    glLinkProgram(Program);
    glGetProgramiv(Program, GL_LINK_STATUS, &LinkStatus);
    if (LinkStatus != GL_TRUE)
    {
        GLint InfoLogLength = 0;

        puts("Failed to link GLSL program");
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 1)
        {
            GLchar *pInfoLog = new GLchar[InfoLogLength + 1];
            if (pInfoLog)
            {
                glGetProgramInfoLog(Program, InfoLogLength, NULL, pInfoLog);
                puts(pInfoLog);
                delete[] pInfoLog;
            }
        }

        return false;
    }

    return true;
}

//
// GetUniformLocation
//
GLint GetUniformLocation(GLuint Program, const GLchar *pName)
{
    GLint Uniform = glGetUniformLocation(Program, pName);

    if (-1 == Uniform)
        printf("Failed to retrieve uniform \"%s\"\n", pName);
    //assert(Uniform != -1);

    return Uniform;
}
