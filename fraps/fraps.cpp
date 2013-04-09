#include "fraps.h"
#include "font.h"
#include "../shader/shader.h"
#include "../chart/chart.h"

enum
{
    FRAPS_NUM_GLYPHS = 10,
    FRAPS_GLYPH_WIDTH = 16,
    FRAPS_GLYPH_HEIGHT = 26,
    FRAPS_GLYPH_PITCH = 20,
    FRAPS_MAX_STRING = 256,

    FRAPS_TEXTURE_BYTEWIDTH = FRAPS_GLYPH_WIDTH * 10 * FRAPS_GLYPH_HEIGHT * sizeof(BYTE) * 4
};

//
// CFraps
//
CFraps::CFraps():
    m_VB(0),
    m_IB(0),
    m_Texture(0),
    m_Program(0),
    m_Mproj(0),
    m_Tex(0),
    m_bFirstPresent(true),
    m_ElapsedTime(0.0f),
    m_NumFrames(0),
    m_fps(0)
{
    puts("CFraps::CFraps");
    ftime(&m_Prev);
    ftime(&m_Curr);

    glGenBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FRAPS_VERTEX) * 4 * FRAPS_MAX_STRING, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
    GLushort *pData = new GLushort[6 * FRAPS_MAX_STRING];
    GLushort *pIndices = pData;
    if (pIndices)
    {
        // CCW order
        // 2|\  |3
        //  | \ |
        // 0|  \|1
        const GLushort GlyphIndices[6] = {0, 1, 2, 2, 1, 3};
        GLushort StartIndex = 0;

        for (int i = 0; i < FRAPS_MAX_STRING; ++i)
        {
            for (int j = 0; j < 6; ++j)
                pIndices[j] = StartIndex + GlyphIndices[j];
            pIndices += 6;
            StartIndex += 4;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * FRAPS_MAX_STRING, pData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        delete[] pData;
    }

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#ifdef GL_ES
    GLenum InternalFormat = GL_RGBA;
    GLenum TexelFormat = GL_RGBA;
#else
    GLenum InternalFormat = GL_RGBA8;
    GLenum TexelFormat = GL_BGRA;
#endif

    glTexImage2D(GL_TEXTURE_2D,
        0, // Level
        InternalFormat,
        FRAPS_GLYPH_WIDTH * 10,
        FRAPS_GLYPH_HEIGHT,
        0, // Border
        TexelFormat,
        GL_UNSIGNED_BYTE,
        g_FRAPSFont);

    const GLchar *pVsh =
    "uniform mat4       Mproj;\n"
    "\n"
    "attribute vec2     position;\n"
    "attribute vec2     texcoord;\n"
    "\n"
    "varying vec2       oTexcoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = Mproj * vec4(position, 0., 1.);\n"
    "    oTexcoord = texcoord;\n"
    "}\n";

    GLuint vsh = LoadGLSLShader(GL_VERTEX_SHADER, pVsh);
    if (0 == vsh)
        return;

    const GLchar *pFsh =
    #ifdef GL_ES
    "precision mediump float;\n"
    #endif
    "uniform sampler2D  font;\n"
    "\n"
    "varying vec2       oTexcoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 color = texture2D(font, oTexcoord);\n"
    "    if (color.a < 1.0)\n"
    "        discard;\n"
    #ifdef GL_ES
    "    gl_FragColor = vec4(color.bgr, 1.);\n" // There is no GL_BGRA format in OpenGL ES
    #else
    "    gl_FragColor = vec4(color.rgb, 1.);\n"
    #endif
    "}\n";

    GLuint fsh = LoadGLSLShader(GL_FRAGMENT_SHADER, pFsh);
    if (0 == fsh)
        return;

    m_Program = glCreateProgram();
    glAttachShader(m_Program, vsh);
    glAttachShader(m_Program, fsh);
    glBindAttribLocation(m_Program, 0, "position");
    glBindAttribLocation(m_Program, 1, "texcoord");
    GLboolean bLinked = LinkGLSLProgram(m_Program);
    glDeleteShader(fsh);
    glDeleteShader(vsh);
    if (!bLinked)
    {
        glDeleteProgram(m_Program);
        return;
    }

    m_Mproj = GetUniformLocation(m_Program, "Mproj");
    m_Tex = GetUniformLocation(m_Program, "font");
}

//
// ~CFraps
//
CFraps::~CFraps()
{
    glDeleteProgram(m_Program);
    glDeleteTextures(1, &m_Texture);
    glDeleteBuffers(1, &m_IB);
    glDeleteBuffers(1, &m_VB);
}

//
// OnPresent
//
void CFraps::OnPresent(CChart *pChart /* = NULL */)
{
    float ElapsedTime = 0.0f;

    ++m_NumFrames;
    if (m_bFirstPresent)
    {
        ftime(&m_Prev);
        m_bFirstPresent = false;
    }
    else
    {
        ftime(&m_Curr);
        ElapsedTime = (m_Curr.time - m_Prev.time) * 1000.0f;
        ElapsedTime += (float)(m_Curr.millitm - m_Prev.millitm);
    }

    if (ElapsedTime > 0.0f)
    {
        m_Prev = m_Curr;
        m_ElapsedTime += ElapsedTime;
    }

    if (m_ElapsedTime >= 1000.0f)
    {
        m_fps = (unsigned)(m_NumFrames * (m_ElapsedTime / 1000.0f));
        // Restart FPS counter
        m_Prev = m_Curr;
        m_ElapsedTime = 0;
        m_NumFrames = 0;

        if (pChart)
            pChart->AddFpsValue((float)m_fps);
    }
}

//
// Draw
//
void CFraps::Draw(unsigned Width, unsigned Height)
{
    unsigned NumGlyphs = 0;
    int x = Width - (FRAPS_GLYPH_WIDTH + 2);
    int y = Height - (FRAPS_GLYPH_HEIGHT + 2);

    FRAPS_VERTEX *pData = new FRAPS_VERTEX[4 * FRAPS_MAX_STRING];
    FRAPS_VERTEX *pVertices = pData;

    if (pVertices)
    {
        const float du = 1.0f / FRAPS_NUM_GLYPHS; // We have 10 digits (0..9) in UV range 0..1
        float u;
        unsigned fps = m_fps;

        while (fps)
        {
            // "Eat" new digit
            unsigned Digit = fps % 10;
            fps /= 10;
            u = Digit * du;

            AddGlyph(pVertices, (float)x, (float)y, FRAPS_GLYPH_WIDTH, FRAPS_GLYPH_HEIGHT, u, 0.0f, u + du, 1.0f);
            ++NumGlyphs;

            pVertices += 4;
            x -= FRAPS_GLYPH_PITCH;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FRAPS_VERTEX) * 4 * NumGlyphs, pData);
        delete[] pData;
    }

    m_Ortho = XMMatrixOrthographicOffCenterRH(
        0.0f,           // Left
        (float)Width,   // Right
        0.0f,           // Bottom
        (float)Height,  // Top
        -1.0f, 1.0f);

    BeginDraw();
    {
        glDrawElements(GL_TRIANGLES, NumGlyphs * 6, GL_UNSIGNED_SHORT, 0);
    }
    EndDraw();
}

//
// BeginDraw
//
void CFraps::BeginDraw()
{
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    GLsizei Stride = sizeof(FRAPS_VERTEX);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(0));
    glEnableVertexAttribArray(0);
    // TexCoords
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Stride, ATTRIB_OFFSET(sizeof(GL_FLOAT) * 2));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);

    glUseProgram(m_Program);
    glUniformMatrix4fv(m_Mproj, 1, GL_FALSE, (const GLfloat *)&m_Ortho);
    glUniform1i(m_Tex, 0); // Texture unit
}

//
// EndDraw
//
void CFraps::EndDraw()
{
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

//
// AddGlyph
//
void CFraps::AddGlyph(FRAPS_VERTEX v[4], float x, float y, float w, float h, float u1, float v1, float u2, float v2)
{
    // CCW order
    // 2|\  |3
    //  | \ |
    // 0|  \|1
    v[0].Pos = XMFLOAT2(x, y);
    v[0].Tex = XMFLOAT2(u1, v1);
    v[1].Pos = XMFLOAT2(x + w, y);
    v[1].Tex = XMFLOAT2(u2, v1);
    v[2].Pos = XMFLOAT2(x, y + h);
    v[2].Tex = XMFLOAT2(u1, v2);
    v[3].Pos = XMFLOAT2(x + w, y + h);
    v[3].Tex = XMFLOAT2(u2, v2);
}
