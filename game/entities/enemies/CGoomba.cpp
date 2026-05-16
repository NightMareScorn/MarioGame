#include "CGoomba.h"
#include "../../../engine/Graphics/Animations.h"

CGoomba::CGoomba(float x, float y) : CGameObject() {
    this->x = x;
    this->y = y;
    this->start_x = x;
    this->vx = -GOOMBA_WALKING_SPEED;
    SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::Update(float dt) {
    if (state == GOOMBA_STATE_DIE) return;

    vy -= GOOMBA_GRAVITY * dt;

    // Movement handled by CCollision inside CPlayScene,
    // but we can flip direction if we hit something horizontally
    // Since CCollision zeroes out vx on horizontal collision:
    if (vx == 0) {
        vx = (nx > 0) ? GOOMBA_WALKING_SPEED : -GOOMBA_WALKING_SPEED;
        nx = -nx; // flip facing
    }
}

void CGoomba::Render() {
    std::string ani = (state == GOOMBA_STATE_WALKING) ? "ANI_GOOMBA_WALK" : "ANI_GOOMBA_DIE";
    CAnimations::GetInstance()->Render(ani, x, y);
}

void CGoomba::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x;
    t = y;
    r = x + GOOMBA_BBOX_WIDTH;
    b = y + GOOMBA_BBOX_HEIGHT;
}

#include "../../../engine/audio/CAudioManager.h"

void CGoomba::SetState(int s) {
    state = s;
    if (state == GOOMBA_STATE_DIE) {
        vx = 0;
        vy = 0;
        CAudioManager::GetInstance()->Play("stomp");
    }
}
