#include "CMushroom.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../../engine/audio/CAudioManager.h"

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
        nx = -nx;
        vx = (nx > 0) ? MUSHROOM_SPEED : -MUSHROOM_SPEED;
    }
}

void CMushroom::Render() {
    if (state == MUSHROOM_STATE_HIDDEN) return;
    CAnimations::GetInstance()->Render("ANI_MUSHROOM1_OW", x, y);
}

void CMushroom::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    if (state == MUSHROOM_STATE_HIDDEN) {
        left = bottom = right = top = 0;
        return;
    }
    left = x;
    bottom = y;
    right = x + MUSHROOM_BBOX_WIDTH;
    top = y + MUSHROOM_BBOX_HEIGHT;
}

void CMushroom::SetState(int s) {
    state = s;
}

void CMushroom::OnCollisionX(CGameObject* other, float nx) {
    if (state == MUSHROOM_STATE_HIDDEN) return;
    if (auto mario = dynamic_cast<CMario*>(other)) {
         SetState(MUSHROOM_STATE_HIDDEN); // Mark as consumed
         mario->PowerUpMushroom();
         CAudioManager::GetInstance()->Play("powerup");
         if (scene) scene->AddScore(1000);
    }
}

void CMushroom::OnCollisionY(CGameObject* other, float ny) {
    if (state == MUSHROOM_STATE_HIDDEN) return;
    if (auto mario = dynamic_cast<CMario*>(other)) {
         SetState(MUSHROOM_STATE_HIDDEN); // Mark as consumed
         mario->PowerUpMushroom();
         CAudioManager::GetInstance()->Play("powerup");
         if (scene) scene->AddScore(1000);
    }
}
