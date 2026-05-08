#include "CBrick.h"
#include "../../../engine/Graphics/Animations.h"

CBrick::CBrick(float x, float y, std::string aniName, bool isInvisible) : CBlock(x, y), aniName(aniName), isInvisible(isInvisible) {}

void CBrick::Render() {
    if (isInvisible) return;
    CAnimations::GetInstance()->Render(aniName, x, y);
}

void CBrick::GetBoundingBox(float &l, float &t, float &r, float &b) {
    const float BLOCK_W = 16.0f;
    const float BLOCK_H = 16.0f;
    l = x;
    t = y;
    r = x + BLOCK_W;
    b = y + BLOCK_H;
}

void CBrick::OnHitFromBelow(CGameObject* hitter) {
    if (isInvisible) {
        isInvisible = false;
        // Optionally trigger item spawn here if hidden_in_block was true
    }
}
