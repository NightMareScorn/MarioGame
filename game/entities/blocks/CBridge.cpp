#include "CBridge.h"
#include "../../../engine/Graphics/Animations.h"

void CBridge::Render() {
    CAnimations::GetInstance()->Render(aniName, x, y);
}

void CBridge::GetBoundingBox(float& l, float& t, float& r, float& b) {
    const float BLOCK_W = 16.0f;
    // Nửa dưới (bottom half) of the sprite is the logs, which has collision.
    // Nửa trên (top half) is the rail, which has no collision and renders over Mario.
    // With y increasing upwards, the bottom half is from y to y + 8.
    l = x;
    t = y;
    r = x + BLOCK_W;
    b = y + 8.0f; // Top edge of collision is in the middle of the sprite!
}
