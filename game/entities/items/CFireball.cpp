#include "CFireball.h"
#include "../player/MarioConfig.h"
#include "../../../engine/Graphics/Animations.h"

static const float FIREBALL_W = 8.0f;
static const float FIREBALL_H = 8.0f;

CFireball::CFireball(float x, float y, int direction)
    : lifetimeMs(MarioConfig::FIREBALL_LIFETIME_MS), dead(false) {
    this->x = x;
    this->y = y;
    this->nx = direction;
    this->vx = MarioConfig::FIREBALL_SPEED * (float)direction;
    this->vy = 0.0f;
}

void CFireball::Update(float dt) {
    if (dead) return;
    lifetimeMs -= dt;
    if (lifetimeMs <= 0.0f) dead = true;
}

void CFireball::Render() {
    if (dead) return;
    CAnimations::GetInstance()->Render("ANI_FLOWER1_OW", x, y, nx);
}

void CFireball::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    if (dead) {
        left = bottom = right = top = 0;
        return;
    }
    left = x;
    bottom = y;
    right = x + FIREBALL_W;
    top = y + FIREBALL_H;
}
