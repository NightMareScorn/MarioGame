#include "CBridge.h"
#include "../../../engine/Graphics/Animations.h"

void CBridge::Render()
{
    CAnimations::GetInstance()->Render(aniName, x, y);
}

void CBridge::GetBoundingBox(float &l, float &t, float &r, float &b)
{
    const float BLOCK_W = 16.0f;
    l = x;
    t = y;
    r = x + BLOCK_W;

    if (aniName == "ANI_WHITE_RED_STEEL_BRIDGE")
    {
        b = y + 16.0f;
    }
    else
    {
        b = y + 8.0f;
    }
}
