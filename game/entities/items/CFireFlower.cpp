#include "CFireFlower.h"
#include "../../../engine/Graphics/Animations.h"

CFireFlower::CFireFlower(float x, float y) : CGameObject() {
    this->x = x;
    this->y = y;
    this->vx = 0;
    this->vy = 0;
    SetState(FLOWER_STATE_HIDDEN);
}

void CFireFlower::Update(float dt) {
    // Fire Flower stays stationary — no physics needed
}

void CFireFlower::Render() {
    if (state == FLOWER_STATE_HIDDEN) return;
    CAnimations::GetInstance()->Render("ANI_FLOWER_OW", x, y);
}

void CFireFlower::GetBoundingBox(float& l, float& t, float& r, float& b) {
    if (state == FLOWER_STATE_HIDDEN) {
        l = t = r = b = 0;
        return;
    }
    l = x;
    t = y;
    r = x + FLOWER_BBOX_WIDTH;
    b = y + FLOWER_BBOX_HEIGHT;
}

void CFireFlower::SetState(int s) {
    state = s;
}
