#include "CKoopa.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"
#include "CGoomba.h"

CKoopa::CKoopa(float x, float y, std::string type, float patrolLeft, float patrolRight) 
    : CGameObject(), type(type) {
    this->x = x;
    this->y = y;
    SetState(KOOPA_STATE_WALKING);
    this->nx = -1;
}

void CKoopa::SetState(int s) {
    state = s;
    if (state == KOOPA_STATE_WALKING) {
        vx = -0.03f;
    } else if (state == KOOPA_STATE_SHELL) {
        vx = 0;
        vy = 0;
    } else if (state == KOOPA_STATE_SHELL_RUNNING) {
        vx = 0.15f * nx;
    }
}

void CKoopa::Update(float dt) {
    vy -= 0.002f * dt;
}

void CKoopa::Render() {
    std::string ani = "ANI_KOOPA_WALK";
    if (state == KOOPA_STATE_SHELL || state == KOOPA_STATE_SHELL_RUNNING) {
        ani = "ANI_KOOPA_SHELL";
    }
    CAnimations::GetInstance()->Render(ani, x, y, nx);
}

void CKoopa::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x;
    bottom = y;
    right = x + 16.0f;
    if (state == KOOPA_STATE_WALKING) {
        top = y + 24.0f;
    } else {
        top = y + 16.0f;
    }
}

#include "../../scenes/play/CPlayScene.h"

void CKoopa::OnCollisionX(CGameObject* other, float nx) {
    if (state == KOOPA_STATE_SHELL_RUNNING) {
        if (auto goomba = dynamic_cast<CGoomba*>(other)) {
            goomba->SetState(GOOMBA_STATE_DIE);
            if (scene) scene->AddScore(100);
        } else if (auto koopa = dynamic_cast<CKoopa*>(other)) {
            koopa->SetState(KOOPA_STATE_SHELL);
            if (scene) scene->AddScore(100);
        } else if (auto mario = dynamic_cast<CMario*>(other)) {
            mario->Hurt();
        }
    } else if (state == KOOPA_STATE_SHELL) {
        if (auto mario = dynamic_cast<CMario*>(other)) {
            this->nx = (mario->x < this->x) ? 1 : -1;
            SetState(KOOPA_STATE_SHELL_RUNNING);
        }
    } else if (state == KOOPA_STATE_WALKING) {
        if (auto mario = dynamic_cast<CMario*>(other)) {
            mario->Hurt();
        }
    }
}

void CKoopa::OnCollisionY(CGameObject* other, float ny) {
    if (ny > 0) { // Hit from top
        if (auto mario = dynamic_cast<CMario*>(other)) {
            if (state == KOOPA_STATE_WALKING || state == KOOPA_STATE_SHELL_RUNNING) {
                SetState(KOOPA_STATE_SHELL);
                if (scene) scene->AddScore(100);
                mario->StompBounce();
            } else if (state == KOOPA_STATE_SHELL) {
                this->nx = (mario->x < this->x) ? 1 : -1;
                SetState(KOOPA_STATE_SHELL_RUNNING);
                mario->StompBounce();
            }
        }
    } else {
        // Hit from below or sides when falling
        if (auto mario = dynamic_cast<CMario*>(other)) {
            if (state == KOOPA_STATE_WALKING || state == KOOPA_STATE_SHELL_RUNNING) {
                mario->Hurt();
            }
        }
    }
}

bool CKoopa::IsBlocking(CGameObject* other) {
    if (state == KOOPA_STATE_SHELL_RUNNING && dynamic_cast<CGoomba*>(other)) {
        return false; // don't block physics when plowing
    }
    return true;
}
