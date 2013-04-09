#include "font.h"
#include "../shader/shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

enum
{
    GLYPH_WIDTH = 16,
    GLYPH_HEIGHT = 16,
    MAX_STRING_LENGTH = 256
};

//
// CFont
//
CFont::CFont(const char *pFileName):
    m_VB(0),
    m_IB(0),
    m_Texture(0),
    m_Program(0),
    m_Mproj(0),
    m_Tex(0)
{
    FILE *pFile = fopen(pFileName, "rb");

    if (!pFile)
    {
        puts("Failed to open font file.");
        return;
    }

    TARGA_IMAGE img = {0};

    if (LoadTGA(pFile, &img))
    {
#       ifdef GL_ES
        GLenum InternalFormat = GL_RGBA;
        GLenum Format = GL_RGBA;
#       else
        GLenum InternalFormat = GL_RGBA8;
        GLenum Format = GL_RGBA;
#       endif

        // Generate font texture
        glGenTextures(1, &m_Texture);
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

        free(img.pData);
    }

    fclose(pFile);

    glGenBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FONT_VERTEX) * 4 * MAX_STRING_LENGTH, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
    GLushort *pData = new GLushort[6 * MAX_STRING_LENGTH];
    GLushort *pIndices = pData;
    if (pIndices)
    {
        // CCW order
        // 2|\  |3
        //  | \ |
        // 0|  \|1
        const GLushort GlyphIndices[6] = {0, 1, 2, 2, 1, 3};
        GLushort StartIndex = 0;

        for (int i = 0; i < MAX_STRING_LENGTH; ++i)
        {
            for (int j = 0; j < 6; ++j)
                pIndices[j] = StartIndex + GlyphIndices[j];
            pIndices += 6;
            StartIndex += 4;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * MAX_STRING_LENGTH, pData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        delete[] pData;
    }

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
    #ifdef GL_ES
    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" // There is no GL_BGRA format in OpenGL ES
    #else
    "    gl_FragColor = vec4(1.0f, 1.0f, 1.0f, color.a);\n"
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
// ~CFont
//
CFont::~CFont()
{
    glDeleteProgram(m_Program);
    glDeleteTextures(1, &m_Texture);
    glDeleteBuffers(1, &m_IB);
    glDeleteBuffers(1, &m_VB);
}

//
// SetScreenSize
//
void CFont::SetScreenSize(unsigned Width, unsigned Height)
{
    m_Ortho = XMMatrixOrthographicOffCenterRH(
        0.0f,           // Left
        (float)Width,   // Right
        0.0f,           // Bottom
        (float)Height,  // Top
        -1.0f, 1.0f);
}

//
// DrawString
//
void CFont::DrawString(int x, int y, const char *pFmt, ...)
{
    unsigned NumGlyphs = 0;
    FONT_VERTEX *pData = new FONT_VERTEX[4 * MAX_STRING_LENGTH];
    FONT_VERTEX *pVertices = pData;

    if (pVertices)
    {
        const float du = 1.0f / 16.0f;
        const float dv = 1.0f / 16.0f;
        size_t Length = strlen(pFmt);

        for (int i = 0; i < (int)Length; ++i)
        {
            unsigned Char = pFmt[i] - 32 + 128;

            float u = float(Char % 16) / 16.0f;
            float v = 1.0f - float(Char / 16) / 16.0f;
            v -= dv; // We need bottom left corner, not top left corner.

            AddGlyph(pVertices, (float)x, (float)y, 12, 12, u, v, u + du, v + dv);

            ++NumGlyphs;
            pVertices += 4;
            x += 11;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FONT_VERTEX) * 4 * NumGlyphs, pData);
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
void CFont::BeginDraw()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    GLsizei Stride = sizeof(FONT_VERTEX);

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
void CFont::EndDraw()
{
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

//
// AddGlyph
//
void CFont::AddGlyph(FONT_VERTEX v[4], float x, float y, float w, float h, float u1, float v1, float u2, float v2)
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

//
// LoadTGA
//
bool CFont::LoadTGA(FILE *pFile, TARGA_IMAGE *pImage) const
{
    GLubyte TGAHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    GLubyte TGACompare[12];
    GLubyte Header[6];
    GLuint BytesPerPixel;
    GLuint ImageSize;
    GLuint Type = GL_RGBA;

    if (fread(TGACompare, 1, sizeof(TGACompare), pFile) != sizeof(TGACompare) ||
        fread(Header, 1, sizeof(Header), pFile) != sizeof(Header))
    {
        puts("Invalid TGA header");
        return false;
    }

    if (memcmp(TGAHeader, TGACompare, sizeof(TGAHeader)) != 0)
        return false;

    pImage->Width = Header[1] * 256 + Header[0];
    pImage->Height = Header[3] * 256 + Header[2];
    pImage->BitsPerPixel = Header[4];

    if (pImage->BitsPerPixel != 32)
        return false;

    BytesPerPixel = pImage->BitsPerPixel/8;
    ImageSize = pImage->Width * pImage->Height * BytesPerPixel;

    pImage->pData = (GLubyte *)malloc(ImageSize);
    if (!pImage->pData)
        return false;

    if (fread(pImage->pData, 1, ImageSize, pFile) != ImageSize)
        return false;

    for (GLuint i = 0; i < ImageSize; i += BytesPerPixel)
    {
        // Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
        GLubyte tmp = pImage->pData[i];
        pImage->pData[i] = pImage->pData[i + 2];
        pImage->pData[i + 2] = tmp;
    }

    return true;
}
