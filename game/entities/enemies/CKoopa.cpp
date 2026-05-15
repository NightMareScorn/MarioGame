#include "CKoopa.h"
#include "../../../engine/Graphics/Animations.h"

CKoopa::CKoopa(float x, float y, std::string type, float patrolLeft, float patrolRight) 
    : CGameObject(), type(type) {
    this->x = x;
    this->y = y;
    this->vx = -0.03f;
    this->nx = -1;
}

void CKoopa::Update(float dt) {
    vy -= 0.002f * dt;
    // Movement resolution happens in scene
}

void CKoopa::Render() {
    std::string ani = "ANI_KOOPA_WALK";
    CAnimations::GetInstance()->Render(ani, x, y, nx);
}

void CKoopa::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x;
    bottom = y;
    right = x + 16.0f;
    top = y + 16.0f;
}
