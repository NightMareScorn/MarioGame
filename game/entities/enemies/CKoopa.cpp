#include "CKoopa.h"
#include "../../../engine/Graphics/Animations.h"

CKoopa::CKoopa(float x, float y) : CGameObject() {
    this->x = x;
    this->y = y;
    this->start_x = x;
    this->vx = -KOOPA_WALKING_SPEED;
    this->nx = -1;
    SetState(KOOPA_STATE_WALKING);
}

void CKoopa::Update(float dt) {
    if (state == KOOPA_STATE_SHELL) return;

    vy -= KOOPA_GRAVITY * dt;

    if (vx == 0) {
        vx = (nx > 0) ? KOOPA_WALKING_SPEED : -KOOPA_WALKING_SPEED;
        nx = -nx;
    }
}

void CKoopa::Render() {
    std::string ani = (state == KOOPA_STATE_WALKING) ? "ANI_KOOPA_WALK" : "ANI_KOOPA_SHELL";
    CAnimations::GetInstance()->Render(ani, x, y, nx); // nx < 0 to flip horizontally if needed
}

void CKoopa::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x;
    t = y;
    r = x + KOOPA_BBOX_WIDTH;
    b = y + ((state == KOOPA_STATE_SHELL) ? 16.0f : KOOPA_BBOX_HEIGHT);
}

void CKoopa::SetState(int s) {
    state = s;
    if (state == KOOPA_STATE_SHELL) {
        vx = 0;
        vy = 0;
    }
}
