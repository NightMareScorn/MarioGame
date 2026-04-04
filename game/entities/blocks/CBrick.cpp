#include "CBrick.h"
#include "../../../engine/Graphics/Animations.h"

#include "../../registry/CResourceRegistry.h"

void CBrick::Render() {
    CAnimations::GetInstance()->Render("ANI_BRICK_IDLE", x, y);
}

void CBrick::GetBoundingBox(float &l, float &t, float &r, float &b) {
    const float BLOCK_W = 16.0f;
    const float BLOCK_H = 16.0f;
    l = x;
    t = y;
    r = x + BLOCK_W;
    b = y + BLOCK_H;
}
