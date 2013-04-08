#ifndef __CHART_H__
#define __CHART_H__

#include "../ogl_es.h"

#ifdef _WIN32
#include <xnamath.h>
#else
#include "../xna/gccxnamath.h"
#endif

struct CHART_VERTEX
{
    float x;
    float y;
};

enum
{
    CHART_MIN_Y_COORD = 10,
    CHART_MAX_Y_COORD = 60
};

// Simple chart drawing

class CChart
{
public:
    CChart();
    ~CChart();

    void AddValue(float Value);
    void Draw(unsigned Width, unsigned Height);

private:
    void BeginDraw();
    void EndDraw();

    GLuint m_VB;
    GLuint m_BoundsVB;
    GLuint m_Program;
    GLint m_Mproj;
    GLint m_Mworld;
    GLint m_Color;

    float m_xOffset;
    XMMATRIX m_World;
    XMMATRIX m_Ortho;

    GLuint m_NumValues;
    GLuint m_FirstValue;

};

#endif // __CHART_H__
