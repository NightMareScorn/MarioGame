#include "CFireball.h"
#include "../player/MarioConfig.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/physics/CCollision.h"
#include "../../scenes/CSceneManager.h"
#include "../../scenes/play/CPlayScene.h"

static const float FIREBALL_W = 8.0f;
static const float FIREBALL_H = 8.0f;

CFireball::CFireball(float x, float y, int direction)
    : lifetimeMs(3000.0f), dead(false) {
    this->x = x;
    this->y = y;
    this->nx = direction;
    this->vx = 0.15f * (float)direction; 
    this->vy = -0.1f; 
}

void CFireball::Update(float dt) {
    if (dead) return;

    // Cập nhật vị trí
    x += vx * dt;
    vy += -0.0008f * dt; // Trọng lực riêng cho đạn
    y += vy * dt;

    // Va chạm với mặt đất để nảy
    CPlayScene* scene = (CPlayScene*)CSceneManager::GetInstance()->GetCurrentScene();
    if (scene) {

        if (y < 32.0f) { // Giả sử 32 là mặt đất
            y = 32.0f;
            vy = 0.15f; // Nảy lên
        }
    }

    lifetimeMs -= 3 * dt;
    if (lifetimeMs <= 0.0f) dead = true;
}

void CFireball::Render() {
    if (dead) return;
    CAnimations::GetInstance()->Render("ANI_COLLECTABLE_COIN", x, y, nx);
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
