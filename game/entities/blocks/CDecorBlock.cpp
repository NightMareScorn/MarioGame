#include "CDecorBlock.h"
#include "../../../engine/Graphics/Animations.h"

CDecorBlock::CDecorBlock(float x, float y, std::string aniName) : aniName(aniName) {
    this->x = x;
    this->y = y;
}

void CDecorBlock::Render() {
    CAnimations::GetInstance()->Render(aniName, x, y);
}

void CDecorBlock::Update(float dt) {
    if (isDrifting) {
        driftTime += dt * 0.001f; // Convert ms to s for smoother freq
        // Small oscillation: +-4 pixels, cycle every ~4 seconds
        x = baseX + sin(driftTime * 1.5f) * 4.0f; 
    }
}

void CDecorBlock::GetBoundingBox(float &l, float &t, float &r, float &b) {
    // No collision for decorative blocks
    l = t = r = b = 0;
}
