#ifndef __FONT_H__
#define __FONT_H__

#include "../ogl_es.h"

#ifdef _WIN32
#include <xnamath.h>
#else
#include "../xna/gccxnamath.h"
#endif

struct FONT_VERTEX
{
    XMFLOAT2 Pos;
    XMFLOAT2 Tex;
};

struct TARGA_IMAGE
{
    GLubyte *pData;
    GLuint Width;
    GLuint Height;
    GLuint BitsPerPixel;
};

// Simple bitmap font

class CFont
{
public:
    CFont(const char *pFileName);
    ~CFont();

    void DrawString(const char *pFmt, ...);

private:
    bool LoadTGA(FILE *pFile, TARGA_IMAGE *pImage) const;

    GLuint m_VB;
    GLuint m_IB;
    GLuint m_Texture;
    GLuint m_Program;
    GLint m_Mproj;
    GLint m_Tex;
};

#endif // __FONT_H__
