#include "CCoin.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"
#include "../../scenes/play/CPlayScene.h"

CCoin::CCoin(float x, float y, bool hidden_in_block) : CGameObject() {
    this->x = x;
    this->y = y;
    this->start_y = y;
    this->in_block = hidden_in_block;
    
    SetState(hidden_in_block ? COIN_STATE_HIDDEN : COIN_STATE_POPPING);
    if (!hidden_in_block) {
        vy = 0; // Stationary coin
    }
}

void CCoin::Update(float dt) {
    if (state == COIN_STATE_HIDDEN || state == COIN_STATE_DONE) return;

    if (in_block && state == COIN_STATE_POPPING) {
        y += vy * dt;
        vy -= COIN_GRAVITY * dt;

        // If it falls back down below its start position
        if (y < start_y) {
            y = start_y;
            vy = 0;
            SetState(COIN_STATE_DONE); // Disappear after popping
            if (scene) {
                scene->AddCoin();
                scene->AddScore(200);
            }
        }
    }
    else if (state == COIN_STATE_COLLECTED) {
        y += vy * dt;
        vy -= COIN_GRAVITY * dt;
        // Effect duration: until it falls back to start_y or a bit of time
        if (y < start_y) {
            SetState(COIN_STATE_DONE);
        }
    }
}

void CCoin::Render() {
    if (state == COIN_STATE_HIDDEN || state == COIN_STATE_DONE) return;

    std::string ani = (in_block || state == COIN_STATE_COLLECTED) ? "ANI_POPPING_COIN" : "ANI_COLLECTABLE_COIN";
    CAnimations::GetInstance()->Render(ani, x, y);
}

void CCoin::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    if (state == COIN_STATE_HIDDEN || state == COIN_STATE_DONE || in_block) {
        left = bottom = right = top = 0;
        return;
    }
    
    left = x;
    bottom = y;
    right = x + 16.0f;
    top = y + 16.0f;
}

void CCoin::SetState(int s) {
    state = s;
    if (state == COIN_STATE_POPPING && in_block) {
        vy = COIN_POP_SPEED;
    }
    else if (state == COIN_STATE_COLLECTED) {
        vy = COIN_POP_SPEED * 0.8f;
        in_block = false;
    }
}

void CCoin::OnCollision(CGameObject* other) {
    if (state == COIN_STATE_POPPING && !in_block) {
        if (dynamic_cast<CMario*>(other)) {
            SetState(COIN_STATE_COLLECTED);
        }
    }
}

void CCoin::OnCollisionX(CGameObject* other, float nx) {
    if (state == COIN_STATE_DONE || state == COIN_STATE_HIDDEN || in_block) return;
    if (dynamic_cast<CMario*>(other)) {
        SetState(COIN_STATE_DONE);
        if (scene) {
            scene->AddCoin();
            scene->AddScore(200);
        }
    }
}

void CCoin::OnCollisionY(CGameObject* other, float ny) {
    if (state == COIN_STATE_DONE || state == COIN_STATE_HIDDEN || in_block) return;
    if (dynamic_cast<CMario*>(other)) {
         SetState(COIN_STATE_DONE);
         if (scene) {
             scene->AddCoin();
             scene->AddScore(200);
         }
    }
}
