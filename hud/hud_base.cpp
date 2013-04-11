#include "hud_base.h"
#include "../shader/shader.h"

//
// CFraps
//
CHudBase::CHudBase(unsigned MaxChars, bool bAlphaBlend):
    m_VB(0),
    m_IB(0),
    m_Texture(0),
    m_Program(0),
    // Default values for invalid uniforms
    m_Mproj(-1),
    m_Color(-1),
    m_Tex(-1),

    m_Width(0),
    m_Height(0)
{
    glGenBuffers(1, &m_VB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(HUD_VERTEX) * 4 * MaxChars, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
    GLushort *pData = new GLushort[6 * MaxChars];
    GLushort *pIndices = pData;
    if (pIndices)
    {
        // CCW order
        // 2|\  |3
        //  | \ |
        // 0|  \|1
        const GLushort GlyphIndices[6] = {0, 1, 2, 2, 1, 3};
        GLushort StartIndex = 0;

        for (unsigned i = 0; i < MaxChars; ++i)
        {
            for (unsigned j = 0; j < 6; ++j)
                pIndices[j] = StartIndex + GlyphIndices[j];
            pIndices += 6;
            StartIndex += 4;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * MaxChars, pData, GL_STATIC_DRAW);
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

    const GLchar *pFsh;

    if (bAlphaBlend)
    {
        pFsh =
        #ifdef GL_ES
        "precision mediump float;\n"
        #endif
        "uniform vec3       color;\n"
        "uniform sampler2D  font;\n"
        "\n"
        "varying vec2       oTexcoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    float a = texture2D(font, oTexcoord).a;\n"
        "    gl_FragColor = vec4(color.rgb, a);\n"
        "}\n";
    }
    else
    {
        pFsh =
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
    }

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
    if (bAlphaBlend)
        m_Color = GetUniformLocation(m_Program, "color");
    m_Tex = GetUniformLocation(m_Program, "font");
}

//
// ~CHudBase
//
CHudBase::~CHudBase()
{
    glDeleteProgram(m_Program);
    glDeleteTextures(1, &m_Texture);
    glDeleteBuffers(1, &m_IB);
    glDeleteBuffers(1, &m_VB);
}

//
// SetScreenSize
//
void CHudBase::SetScreenSize(unsigned Width, unsigned Height)
{
    m_Ortho = XMMatrixOrthographicOffCenterRH(
        0.0f,           // Left
        (float)Width,   // Right
        0.0f,           // Bottom
        (float)Height,  // Top
        -1.0f, 1.0f);

    m_Width = Width;
    m_Height = Height;
}

//
// BeginDraw
//
void CHudBase::BeginDraw()
{
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    GLsizei Stride = sizeof(HUD_VERTEX);

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
}

//
// EndDraw
//
void CHudBase::EndDraw()
{
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

//
// AddGlyph
//
void CHudBase::AddGlyph(HUD_VERTEX v[4], float x, float y, float w, float h, float u1, float v1, float u2, float v2)
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
