#ifndef __HUD_BASE_H__
#define __HUD_BASE_H__

#include "../ogl_es.h"

#ifdef _WIN32
#include <xnamath.h>
#else
#include "../xna/gccxnamath.h"
#endif

#include <assert.h>

struct HUD_VERTEX
{
    XMFLOAT2 Pos;
    XMFLOAT2 Tex;
};

// Base HUD class

class CHudBase
{
public:
    CHudBase(unsigned MaxChars, bool bAlphaBlend);
    ~CHudBase();

    void SetScreenSize(unsigned Width, unsigned Height);

protected:
    void BeginDraw();
    void EndDraw();
    void AddGlyph(HUD_VERTEX v[4], float x, float y, float w, float h, float u1, float v1, float u2, float v2);

    GLuint m_VB;
    GLuint m_IB;
    GLuint m_Texture;
    GLuint m_Program;
    GLint m_Mproj;
    GLint m_Color;
    GLint m_Tex;

    XMMATRIX m_Ortho;
    unsigned m_Width;
    unsigned m_Height;
};

#endif // __HUD_BASE_H__
