#ifndef __FRAPS_H__
#define __FRAPS_H__

#include "hud_base.h"
#include <sys/timeb.h>

class CChart;

// Our own FRAPS-like fps counter

class CFraps : public CHudBase
{
public:
    CFraps();
    ~CFraps();

    void OnPresent(CChart *pChart = NULL);
    void Draw();

private:
    void BeginDraw();
    void EndDraw();

    timeb m_Prev;
    timeb m_Curr;
    bool m_bFirstPresent;
    float m_ElapsedTime;
    UINT m_NumFrames;
    UINT m_fps;
};

#endif // __FRAPS_H__
