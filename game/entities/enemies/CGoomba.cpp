#include "CGoomba.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/audio/CAudioManager.h"

CGoomba::CGoomba(float x, float y, float patrolLeft, float patrolRight) : CGameObject() {
    this->x = x;
    this->y = y;
    this->vx = -GOOMBA_WALKING_SPEED;
    SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::Update(float dt) {
    if (state == GOOMBA_STATE_DIE) return;
    vy -= GOOMBA_GRAVITY * dt;
}

void CGoomba::Render() {
    std::string ani = (state == GOOMBA_STATE_WALKING) ? "ANI_GOOMBA_WALK" : "ANI_GOOMBA_DIE";
    CAnimations::GetInstance()->Render(ani, x, y);
}

void CGoomba::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x;
    bottom = y;
    right = x + GOOMBA_BBOX_WIDTH;

    if (state == GOOMBA_STATE_DIE)
        top = y + GOOMBA_BBOX_HEIGHT_DIE;
    else
        top = y + GOOMBA_BBOX_HEIGHT;
}

void CGoomba::SetState(int s) {
    state = s;
    if (state == GOOMBA_STATE_DIE) {
        vx = 0;
        vy = 0;
        CAudioManager::GetInstance()->Play("stomp");
    }
}
