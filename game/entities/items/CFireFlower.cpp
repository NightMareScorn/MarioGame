#include "CFireFlower.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"

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

void CFireFlower::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    if (state == FLOWER_STATE_HIDDEN) {
        left = bottom = right = top = 0;
        return;
    }
    left = x;
    bottom = y;
    right = x + FLOWER_BBOX_WIDTH;
    top = y + FLOWER_BBOX_HEIGHT;
}

void CFireFlower::SetState(int s) {
    state = s;
}

#include "../../../engine/audio/CAudioManager.h"
#include "../../scenes/play/CPlayScene.h"

void CFireFlower::OnCollisionX(CGameObject* other, float nx) {
    if (state == FLOWER_STATE_HIDDEN) return;
    if (auto mario = dynamic_cast<CMario*>(other)) {
        SetState(FLOWER_STATE_HIDDEN);
        mario->PowerUpFlower();
        CAudioManager::GetInstance()->Play("powerup");
        if (scene) scene->AddScore(1000);
    }
}

void CFireFlower::OnCollisionY(CGameObject* other, float ny) {
    if (state == FLOWER_STATE_HIDDEN) return;
    if (auto mario = dynamic_cast<CMario*>(other)) {
        SetState(FLOWER_STATE_HIDDEN);
        mario->PowerUpFlower();
        CAudioManager::GetInstance()->Play("powerup");
        if (scene) scene->AddScore(1000);
    }
}
