#include "bg.h"
#include "../shader/shader.h"

enum
{
    BG_WIDTH = 1920,
    BG_HEIGHT = 1080
};

//
// CBackground
//
CBackground::CBackground():
    CHudBase(1, false)
{
    FILE *pFile;

    pFile = fopen("bg", "rb");
    if (!pFile)
    {
        puts("Error : Failed to open bitmap file.");
        return;
    }

    GLubyte *pData = new GLubyte[BG_WIDTH  * BG_HEIGHT * 4];
    if (!pData)
    {
        fclose(pFile);
        return;
    }

    fread(pData, 1, BG_WIDTH * BG_HEIGHT * 4, pFile);
    fclose(pFile);

#ifdef GL_ES
    GLenum InternalFormat = GL_RGBA;
    GLenum Format = GL_RGBA;
#else
    GLenum InternalFormat = GL_RGBA8;
    GLenum Format = GL_BGRA;
#endif

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,
        0, // Level
        InternalFormat,
        BG_WIDTH,
        BG_HEIGHT,
        0, // Border
        Format,
        GL_UNSIGNED_BYTE,
        pData);

    delete[] pData;
}

//
// ~CBackground
//
CBackground::~CBackground()
{
}

//
// Draw
//
void CBackground::Draw()
{
    // Make sure that ortho matrix is correct
    assert(m_Width);
    assert(m_Height);

    // Draw only if texture was loaded successfully.
    assert(m_Texture);
    if (0 == m_Texture)
        return;

    HUD_VERTEX v[4];

    AddGlyph(v, 0.0f, 0.0f, (float)m_Width, (float)m_Height, 0.0f, 0.0f, 1.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(HUD_VERTEX) * 4, v);

    BeginDraw();
    {
        glUniform1i(m_Tex, 0); // Texture unit
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    }
    EndDraw();
}
