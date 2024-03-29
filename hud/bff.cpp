#include "bff.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>

enum
{
    WIDTH_DATA_OFFSET = 20, // Offset to width data with BFF file
    MAP_DATA_OFFSET = 276, // Offset to texture image data with BFF file
    MAX_STRING_LENGTH = 256
};

//
// CBffFont
//
CBffFont::CBffFont(const char *pFileName):
    CHudBase(MAX_STRING_LENGTH, true),
    m_BaseIndex(0),
    m_ColFactor(0.0f),
    m_RowFactor(0.0f),
    m_RowPitch(0),
    m_CharX(0),
    m_CharY(0),

    m_TextColor(1.0f, 1.0f, 1.0f),
    m_Scale(1.0f)
{
    memset(m_CharWidths, 0, sizeof(m_CharWidths));

    FILE *pFile;
    FONT_IMAGE img;

    pFile = fopen(pFileName, "rb");
    if (!pFile)
    {
        puts("Error : Failed to open font file.");
        return;
    }

    if (!LoadBff(pFile, &img))
    {
        fclose(pFile);
        return;
    }

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,
        0, // Level
        InternalFormat,
        img.Width,
        img.Height,
        0, // Border
        Format,
        GL_UNSIGNED_BYTE,
        img.pData);

    delete[] img.pData;
}

//
// ~CBffFont
//
CBffFont::~CBffFont()
{
}

//
// SetColor
//
void CBffFont::SetColor(XMFLOAT3 Color)
{
    m_TextColor = Color;
}

//
// SetScale
//
void CBffFont::SetScale(float Scale)
{
    GLint Filter;

    m_Scale = Scale;
    if (m_Scale == 1.0f)
        Filter = GL_NEAREST;
    else
        Filter = GL_LINEAR;

    if (m_Texture)
    {
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

//
// CalcStringWidth
//
float CBffFont::CalcStringWidth(const char *pFormat, ...)
{
    char s[MAX_STRING_LENGTH];
    va_list val;

    // Compose string
    va_start(val, pFormat);
    vsnprintf(s, MAX_STRING_LENGTH, pFormat, val);
    va_end(val);

    const int n = (int)strlen(s);
    float Width = 0.0f;

    for (int i = 0; i < n; ++i)
    {
        char chr = s[i];

        Width += m_CharWidths[chr] * m_Scale;
    }

    return Width;
}

//
// DrawString
//
void CBffFont::DrawString(int x, int y, const char *pFormat, ...)
{
    // Make sure that ortho matrix is correct
    assert(m_Width);
    assert(m_Height);

    // Draw only if texture was loaded successfully.
    if (0 == m_Texture)
        return;

    char s[MAX_STRING_LENGTH];
    va_list val;

    // Compose string
    va_start(val, pFormat);
    vsnprintf(s, MAX_STRING_LENGTH, pFormat, val);
    va_end(val);

    static HUD_VERTEX Data[4 * MAX_STRING_LENGTH];
    HUD_VERTEX *pVertices = Data;
    unsigned NumGlyphs = 0;

    if (pVertices)
    {
        const int n = (int)strlen(s);

        float fx = x;
        float fy = y;

        for (int i = 0; i < n; ++i)
        {
            char chr = s[i];
            int Row = (chr - m_BaseIndex) / m_RowPitch;
            int Col = (chr - m_BaseIndex) - Row * m_RowPitch;

            float u = Col * m_ColFactor;
            float v = Row * m_RowFactor;
            float u1 = u + m_ColFactor;
            float v1 = v + m_RowFactor;

            AddGlyph(pVertices, fx, fy, m_CharX * m_Scale, m_CharY * m_Scale, u, v1, u1, v);

            ++NumGlyphs;
            pVertices += 4;

            fx += m_CharWidths[chr] * m_Scale;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(HUD_VERTEX) * 4 * NumGlyphs, Data);
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
void CBffFont::BeginDraw()
{
    CHudBase::BeginDraw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform3fv(m_Color, 1, (const GLfloat *)&m_TextColor);
    glUniform1i(m_Tex, 0); // Texture unit
}

//
// EndDraw
//
void CBffFont::EndDraw()
{
    glDisable(GL_BLEND);

    CHudBase::EndDraw();
}

//
// LoadBff
//
bool CBffFont::LoadBff(FILE *pFile, FONT_IMAGE *pImage)
{
    fseek(pFile, 0L, SEEK_END);
    long Size = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    // Allocate space for file data
    char *pData = new char[Size];
    if (!pData)
        return false;

    fread(pData, 1, Size, pFile);

    // Check ID is 'BFF2'
    if((unsigned char)pData[0] != 0xBF ||
       (unsigned char)pData[1] != 0xF2)
    {
        delete[] pData;
        return false;
    }

    // Grab the rest of the header
    memcpy(&pImage->Width, &pData[2], sizeof(GLint));
    memcpy(&pImage->Height, &pData[6], sizeof(GLint));
    memcpy(&m_CharX, &pData[10], sizeof(int));
    memcpy(&m_CharY, &pData[14], sizeof(int));
    char BitsPerPixel = pData[18];
    m_BaseIndex = pData[19];

    if (BitsPerPixel != 32)
    {
        delete[] pData;
        return false;
    }

    const int ImageSize = (pImage->Width * pImage->Height) * (BitsPerPixel / 8);

    // Check filesize
    if (Size != (MAP_DATA_OFFSET + ImageSize))
        return false;

    // Calculate font params
    m_ColFactor = (float)m_CharX / (float)pImage->Width;
    m_RowFactor = (float)m_CharY / (float)pImage->Height;
    m_RowPitch = pImage->Width / m_CharX;

    // Allocate space for image
    pImage->pData = new GLubyte[ImageSize];
    if (!pImage->pData)
    {
        delete[] pData;
        return false;
    }

    memcpy(m_CharWidths, &pData[WIDTH_DATA_OFFSET], ASCII_CHARS); // Grab char widths
    memcpy(pImage->pData, &pData[MAP_DATA_OFFSET], ImageSize);
    delete[] pData;

    return true;
}
