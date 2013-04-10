#ifndef __BFF_H__
#define __BFF_H__

#include "hud_base.h"

// BFF bitmap font
// http://www.codehead.co.uk/cbfg

class CBffFont : public CHudBase
{
public:
    CBffFont(const char *pFileName);
    ~CBffFont();

    void DrawString(int x, int y, const char *pFmt, ...);

private:
    void BeginDraw();
    void EndDraw();

    bool LoadFont(const char *fname);

    int CellX,CellY,YOffset,RowPitch;
    char Base;
    char Width[256];
    int CurX,CurY;
    float RowFactor,ColFactor;
    int RenderStyle;
    float Rd,Gr,Bl;
    bool InvertYAxis;
};

#endif // __BFF_H__
