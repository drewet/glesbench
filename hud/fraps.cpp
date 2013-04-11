#include "fraps.h"
#include "fraps_font.h"
#include "chart.h"
//#include "../shader/shader.h"

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
    CHudBase(FRAPS_MAX_STRING, false),
    m_bFirstPresent(true),
    m_ElapsedTime(0.0f),
    m_NumFrames(0),
    m_fps(0)
{
    ftime(&m_Prev);
    ftime(&m_Curr);

#ifdef GL_ES
    GLenum InternalFormat = GL_RGBA;
    GLenum Format = GL_RGBA;
#else
    GLenum InternalFormat = GL_RGBA8;
    GLenum Format = GL_BGRA;
#endif

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,
        0, // Level
        InternalFormat,
        FRAPS_GLYPH_WIDTH * 10,
        FRAPS_GLYPH_HEIGHT,
        0, // Border
        Format,
        GL_UNSIGNED_BYTE,
        g_FRAPSFont);
}

//
// ~CFraps
//
CFraps::~CFraps()
{
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
void CFraps::Draw()
{
    // Make sure that ortho matrix is correct
    assert(m_Width);
    assert(m_Height);

    unsigned NumGlyphs = 0;
    int x = m_Width - (FRAPS_GLYPH_WIDTH + 2);
    int y = m_Height - (FRAPS_GLYPH_HEIGHT + 2);

    HUD_VERTEX *pData = new HUD_VERTEX[4 * FRAPS_MAX_STRING];
    HUD_VERTEX *pVertices = pData;

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
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(HUD_VERTEX) * 4 * NumGlyphs, pData);
        delete[] pData;
    }

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
    CHudBase::BeginDraw();

    glUniform1i(m_Tex, 0); // Texture unit
}

//
// EndDraw
//
void CFraps::EndDraw()
{
    CHudBase::EndDraw();
}
