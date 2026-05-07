#include "CMushroom.h"
#include "../../../engine/Graphics/Animations.h"

CMushroom::CMushroom(float x, float y) : CGameObject() {
    this->x = x;
    this->y = y;
    this->vx = MUSHROOM_SPEED; // typically moves right initially
    SetState(MUSHROOM_STATE_HIDDEN);
}

void CMushroom::Update(float dt) {
    if (state == MUSHROOM_STATE_HIDDEN) return;

    vy -= MUSHROOM_GRAVITY * dt;

    if (vx == 0) { // Bumped into wall horizontally
        vx = (nx > 0) ? MUSHROOM_SPEED : -MUSHROOM_SPEED;
        nx = -nx;
    }
}

void CMushroom::Render() {
    if (state == MUSHROOM_STATE_HIDDEN) return;
    CAnimations::GetInstance()->Render("ANI_MUSHROOM1_OW", x, y);
}

void CMushroom::GetBoundingBox(float& l, float& t, float& r, float& b) {
    if (state == MUSHROOM_STATE_HIDDEN) {
        l = t = r = b = 0;
        return;
    }
    l = x;
    t = y;
    r = x + MUSHROOM_BBOX_WIDTH;
    b = y + MUSHROOM_BBOX_HEIGHT;
}

void CMushroom::SetState(int s) {
    state = s;
}
