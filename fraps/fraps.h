#ifndef __FRAPS_H__
#define __FRAPS_H__

#include "../ogl_es.h"

#ifdef _WIN32
#include <xnamath.h>
#else
#include "../xna/gccxnamath.h"
#endif

#include <sys/timeb.h>

class CChart;

struct FRAPS_VERTEX
{
    XMFLOAT2 Pos;
    XMFLOAT2 Tex;
};

// Our own FRAPS-like fps counter

class CFraps
{
public:
    CFraps();
    ~CFraps();

    void OnPresent(CChart *pChart = NULL);
    void Draw(unsigned Width, unsigned Height);

private:
    void BeginDraw();
    void EndDraw();
    void AddGlyph(FRAPS_VERTEX v[4], float x, float y, float w, float h, float u1, float v1, float u2, float v2);

    GLuint m_VB;
    GLuint m_IB;
    GLuint m_Texture;
    GLuint m_Program;
    GLint m_Mproj;
    GLint m_Tex;

    XMMATRIX m_Ortho;

    timeb m_Prev;
    timeb m_Curr;
    bool m_bFirstPresent;
    float m_ElapsedTime;
    UINT m_NumFrames;
    UINT m_fps;
};

#endif // __FRAPS_H__
