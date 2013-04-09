#include "chart.h"
#include "../shader/shader.h"

//
// CChart
//
CChart::CChart():
    m_VB(0),
    m_BoundsVB(0),
    m_Program(0),
    m_Mproj(0),
    m_Mworld(0),

    m_xOffset(0.0f),

    m_NumValues(0),
    m_FirstValue(0)
{
    glGenBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CHART_VERTEX) * 10000, NULL, GL_DYNAMIC_DRAW);

    const CHART_VERTEX BoundLines[4] =
    {
        {0.0f, CHART_MIN_Y_COORD},
        {4096.0f, CHART_MIN_Y_COORD},

        {0.0f, CHART_MAX_Y_COORD},
        {4096.0f, CHART_MAX_Y_COORD}
    };

    glGenBuffers(1, &m_BoundsVB);
    glBindBuffer(GL_ARRAY_BUFFER, m_BoundsVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BoundLines), BoundLines, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    const GLchar *pVsh =
    "uniform mat4       Mworld;\n"
    "uniform mat4       Mproj;\n"
    "\n"
    "attribute vec2     position;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position.xyz = (Mworld * vec4(position, 0., 1.)).xyz;\n"
    "    gl_Position = Mproj * vec4(gl_Position.xyz, 1.);\n"
    "}\n";

    GLuint vsh = LoadGLSLShader(GL_VERTEX_SHADER, pVsh);
    if (0 == vsh)
        return;

    const GLchar *pFsh =
    #ifdef GL_ES
    "precision mediump  float;\n"
    #endif
    "uniform vec4       color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(color);\n"
    "}\n";

    GLuint fsh = LoadGLSLShader(GL_FRAGMENT_SHADER, pFsh);
    if (0 == fsh)
        return;

    m_Program = glCreateProgram();
    glAttachShader(m_Program, vsh);
    glAttachShader(m_Program, fsh);
    glBindAttribLocation(m_Program, 0, "position");
    GLboolean bLinked = LinkGLSLProgram(m_Program);
    glDeleteShader(fsh);
    glDeleteShader(vsh);
    if (!bLinked)
    {
        glDeleteProgram(m_Program);
        return;
    }

    m_Mproj = GetUniformLocation(m_Program, "Mproj");
    m_Mworld = GetUniformLocation(m_Program, "Mworld");
    m_Color = GetUniformLocation(m_Program, "color");
}

//
// ~CChart
//
CChart::~CChart()
{
    glDeleteProgram(m_Program);
    glDeleteBuffers(1, &m_VB);
}

//
// AddFpsValue
//
void CChart::AddFpsValue(float Value)
{
    const float CHART_Y_DELTA = CHART_MAX_Y_COORD - CHART_MIN_Y_COORD;

    // Clamp value
    if (Value <= 0.0f)
        Value = 0.0f;
    if (Value > 100.0f)
        Value = 100.0f;

    // Take, for example, 90 fps: (90 - min) / (max - min)
    float ChartValue = ((Value - CHART_MIN_FPS) / (CHART_MAX_FPS - CHART_MIN_FPS));

    if (ChartValue < 0.0f) // Below 60 fps
        ChartValue = 0.0f;

    CHART_VERTEX v = {0.0f, 0.0f};

    v.x = m_NumValues * CHART_X_STEP;
    v.y = ChartValue * CHART_Y_DELTA + CHART_MIN_Y_COORD;

    GLintptr Offset = m_NumValues * sizeof(CHART_VERTEX);

    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferSubData(GL_ARRAY_BUFFER, Offset, sizeof(CHART_VERTEX), &v);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ++m_NumValues;
    m_FirstValue = m_NumValues - 10; //???
}

//
// Draw
//
void CChart::Draw(unsigned Width, unsigned Height)
{
    m_Ortho = XMMatrixOrthographicOffCenterRH(
        0.0f,           // Left
        (float)Width,   // Right
        0.0f,           // Bottom
        (float)Height,  // Top
        -1.0f, 1.0f);

    BeginDraw();
    {
        XMMATRIX World = XMMatrixIdentity();
        GLsizei Stride = 0; // Tightly packed

        float ChartWidth = (m_NumValues - 1) * CHART_X_STEP;
        if (ChartWidth > Width)
        {
            float xOffset = ChartWidth - Width;
            World = XMMatrixTranslation(-xOffset, 0.0f, 0.0f);
        }

        if (m_NumValues >= 2) // Line strip requires at least two values
        {
            glUniformMatrix4fv(m_Mworld, 1, GL_FALSE, (const GLfloat *)&World);
            glUniform4f(m_Color, 1.0f, 0.0f, 0.0, 1.0f);
            glBindBuffer(GL_ARRAY_BUFFER, m_VB);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0));
            glDrawArrays(GL_LINE_STRIP, 0, m_NumValues);
        }

        static XMMATRIX Identity = XMMatrixIdentity();

        glUniformMatrix4fv(m_Mworld, 1, GL_FALSE, (const GLfloat *)&Identity);
        glUniform4f(m_Color, 0.75f, 0.75f, 0.75f, 0.5f);
        glBindBuffer(GL_ARRAY_BUFFER, m_BoundsVB);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0));
        glDrawArrays(GL_LINES, 0, 2);
        glDrawArrays(GL_LINES, 2, 2);
    }
    EndDraw();
}

//
// BeginDraw
//
void CChart::BeginDraw()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(0);

    glUseProgram(m_Program);
    glUniformMatrix4fv(m_Mproj, 1, GL_FALSE, (const GLfloat *)&m_Ortho);
}

//
// EndDraw
//
void CChart::EndDraw()
{
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
