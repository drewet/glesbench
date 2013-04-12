#ifndef __BG_H__
#define __BG_H__

#include "hud_base.h"

// Simple background bitmap

class CBackground : public CHudBase
{
public:
    CBackground();
    ~CBackground();

    void Draw();
};

#endif // __BG_H__
