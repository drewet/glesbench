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

    void SetScreenSize(unsigned Width, unsigned Height);
    void DrawString(int x, int y, const char *pFmt, ...);

private:
    void BeginDraw();
    void EndDraw();
    void AddGlyph(FONT_VERTEX v[4], float x, float y, float w, float h, float u1, float v1, float u2, float v2);

    bool LoadTGA(FILE *pFile, TARGA_IMAGE *pImage) const;

    GLuint m_VB;
    GLuint m_IB;
    GLuint m_Texture;
    GLuint m_Program;
    GLint m_Mproj;
    GLint m_Tex;

    XMMATRIX m_Ortho;
};

#endif // __FONT_H__
