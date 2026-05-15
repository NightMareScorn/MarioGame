#include "CDecorBlock.h"
#include "../../../engine/Graphics/Animations.h"

CDecorBlock::CDecorBlock(float x, float y, std::string aniName) : aniName(aniName) {
    this->x = x;
    this->y = y;
}

void CDecorBlock::Render() {
    CAnimations::GetInstance()->Render(aniName, x, y);
}

void CDecorBlock::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    left = bottom = right = top = 0;
}
