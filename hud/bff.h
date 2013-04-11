#ifndef __BFF_H__
#define __BFF_H__

#include "hud_base.h"

struct FONT_IMAGE
{
    GLint Width;
    GLint Height;
    GLubyte *pData;
};

enum
{
    ASCII_CHARS = 256
};

// BFF bitmap font
// http://www.codehead.co.uk/cbfg

class CBffFont : public CHudBase
{
public:
    CBffFont(const char *pFileName);
    ~CBffFont();

    void SetColor(XMFLOAT3 Color);
    void DrawString(int x, int y, const char *pFmt, ...);

private:
    void BeginDraw();
    void EndDraw();

    bool LoadBff(FILE *pFile, FONT_IMAGE *pImage);

    char m_BaseIndex;
    char m_CharWidths[ASCII_CHARS];
    float m_ColFactor;
    float m_RowFactor;
    int m_RowPitch;
    int m_CharX;
    int m_CharY;

    XMFLOAT3 m_TextColor;
};

#endif // __BFF_H__
