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
    // Movement resolution happens in scene
}

void CKoopa::Render() {
    std::string ani = "ANI_KOOPA_WALK";
    if (state == KOOPA_STATE_SHELL || state == KOOPA_STATE_SHELL_RUNNING) {
        ani = "ANI_KOOPA_SHELL"; // Assuming you have this animation
    }
    CAnimations::GetInstance()->Render(ani, x, y, nx);
}

void CKoopa::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x;
    bottom = y;
    right = x + 16.0f;
    if (state == KOOPA_STATE_WALKING) {
        top = y + 24.0f; // Walk bbox
    } else {
        top = y + 16.0f; // Shell bbox
    }
}

#include "../../scenes/play/CPlayScene.h"

void CKoopa::OnCollisionX(CGameObject* other, float nx) {
    if (state == KOOPA_STATE_SHELL_RUNNING) {
        if (auto goomba = dynamic_cast<CGoomba*>(other)) {
            goomba->SetState(GOOMBA_STATE_DIE);
            if (scene) scene->AddScore(100);
        } else if (auto koopa = dynamic_cast<CKoopa*>(other)) {
            koopa->SetState(KOOPA_STATE_SHELL); // or whatever
            if (scene) scene->AddScore(100);
        } else if (auto mario = dynamic_cast<CMario*>(other)) {
            // mario takes hit if it's running? Actually Mario handles his own hits.
        }
    } else if (state == KOOPA_STATE_SHELL) {
        if (auto mario = dynamic_cast<CMario*>(other)) {
            this->nx = (mario->x < this->x) ? 1 : -1;
            SetState(KOOPA_STATE_SHELL_RUNNING);
        }
    }
}

void CKoopa::OnCollisionY(CGameObject* other, float ny) {
    if (ny > 0) { // Hit from top (other landed on this)
        if (auto mario = dynamic_cast<CMario*>(other)) {
            if (state == KOOPA_STATE_WALKING || state == KOOPA_STATE_SHELL_RUNNING) {
                SetState(KOOPA_STATE_SHELL);
                if (scene) scene->AddScore(100);
                mario->vy = 0.2f; // Mario bounces up
            } else if (state == KOOPA_STATE_SHELL) {
                this->nx = (mario->x < this->x) ? 1 : -1;
                SetState(KOOPA_STATE_SHELL_RUNNING);
                mario->vy = 0.2f;
            }
        }
    }
}

bool CKoopa::IsBlocking(CGameObject* other) {
    if (auto mario = dynamic_cast<CMario*>(other)) {
        // If Koopa is Shell running, or Mario runs into Shell, does it physically block?
        // Usually Mario takes damage or kicks it, but we can let it be non-blocking physically
        // so Mario overlaps slightly, and trigger event pushes/damages Mario?
        // Or keep it blocking. Let's just return true except maybe if Koopa is running and other is enemy
    }
    if (state == KOOPA_STATE_SHELL_RUNNING && dynamic_cast<CGoomba*>(other)) {
        return false; // don't block physics when plowing
    }
    return true;
}
