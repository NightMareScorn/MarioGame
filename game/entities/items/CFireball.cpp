#include "CFireball.h"
#include "../player/MarioConfig.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/physics/CCollision.h"
#include "../../scenes/CSceneManager.h"
#include "../../scenes/play/CPlayScene.h"

static const float FIREBALL_W = 8.0f;
static const float FIREBALL_H = 8.0f;

CFireball::CFireball(float x, float y, int direction)
    : lifetimeMs(3000.0f), dead(false), bounceCount(0) {
    this->x = x;
    this->y = y;
    this->nx = direction;
    this->vx = 0.15f * (float)direction; 
    this->vy = -0.1f; 
}

void CFireball::Update(float dt) {
    if (dead) return;

    // Cập nhật gia tốc trọng lực (ResolveCollision sẽ cập nhật tọa độ x, y từ vx, vy)
    vy += -0.0008f * dt; 

    lifetimeMs -= 3 * dt;
    if (lifetimeMs <= 0.0f) dead = true;
}

void CFireball::OnCollisionX(CGameObject *other, float nx) {
    // Chạm tường/vật cản dọc -> Nổ đạn biến mất
    if (dynamic_cast<CBlock *>(other)) {
        this->Kill();
    }
}

void CFireball::OnCollisionY(CGameObject *other, float ny) {
    // Chạm sàn/gạch -> Nảy lên
    if (dynamic_cast<CBlock *>(other)) {
        if (ny > 0) { // Chạm đất/đầu gạch
            vy = 0.15f; // Nảy lên
            bounceCount++;
            if (bounceCount >= 3) { // Nảy tối đa 2 lần, chạm sàn lần 3 sẽ biến mất
                this->Kill();
            }
        } else if (ny < 0) { // Chạm trần
            vy = -0.05f;
        }
    }
}

void CFireball::Render() {
    if (dead) return;
    CAnimations::GetInstance()->Render("ANI_EFFECT_FIREBALL_SHOOT", x, y, nx);
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
