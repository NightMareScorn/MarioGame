#include "CKoopa.h"
#include "../../../engine/Graphics/Animations.h"

CKoopa::CKoopa(float x, float y, std::string type, float patrolLeft, float patrolRight) 
    : CGameObject(), type(type) {
    this->x = x;
    this->y = y;
    this->vx = -0.03f;
    this->nx = -1;
}

void CKoopa::SetState(int s) {
    state = s;
    if (state == KOOPA_STATE_SHELL) {
        vx = 0;
        dieTimer = 500.0f;
    }
}

void CKoopa::Update(float dt) {
    if (state == KOOPA_STATE_SHELL) {
        dieTimer -= dt;
        if (dieTimer <= 0) {
            isDead = true;
        }
    }
    vy -= 0.002f * dt;
    // Movement resolution happens in scene
}

void CKoopa::Render() {
    std::string ani = (state == KOOPA_STATE_WALKING) ? "ANI_KOOPA_WALK" : "ANI_KOOPA_SHELL";
    CAnimations::GetInstance()->Render(ani, x, y, nx);
}

void CKoopa::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x;
    bottom = y;
    right = x + 16.0f;
    top = (state == KOOPA_STATE_SHELL) ? (y + 14.0f) : (y + 16.0f);
}
