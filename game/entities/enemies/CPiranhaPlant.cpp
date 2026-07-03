#include "CPiranhaPlant.h"
#include <cmath>

CPiranhaPlant::CPiranhaPlant(float x, float y) : CGameObject() {
    this->x = x;
    this->start_y = y;
    this->y = y - 24.0f;
    this->state = PIRANHA_STATE_RETRACTED;
    this->timer = 0.0f;
    this->vx = 0.0f;
    this->vy = 0.0f;
    this->nx = 1;
}

void CPiranhaPlant::Update(float dt) {
    CPlayScene* scene = (CPlayScene*)CSceneManager::GetInstance()->GetCurrentScene();
    
    if (state == PIRANHA_STATE_RETRACTED) {
        vy = 0.0f;
        timer += dt;
        
        // If Mario is too close, keep resetting timer so we stay hidden
        if (scene && scene->GetPlayer()) {
            float mx = scene->GetPlayer()->x;
            if (std::fabs(mx - (x + 8.0f)) < 32.0f) {
                timer = 0.0f;
            }
        }
        
        if (timer >= 2000.0f) {
            state = PIRANHA_STATE_EMERGING;
            timer = 0.0f;
        }
    } else if (state == PIRANHA_STATE_EMERGING) {
        vy = 0.02f;
        if (y >= start_y) {
            y = start_y;
            vy = 0.0f;
            state = PIRANHA_STATE_EMERGED;
            timer = 0.0f;
        }
    } else if (state == PIRANHA_STATE_EMERGED) {
        vy = 0.0f;
        timer += dt;
        if (timer >= 2000.0f) {
            state = PIRANHA_STATE_RETRACTING;
            timer = 0.0f;
        }
    } else if (state == PIRANHA_STATE_RETRACTING) {
        vy = -0.02f;
        if (y <= start_y - 24.0f) {
            y = start_y - 24.0f;
            vy = 0.0f;
            state = PIRANHA_STATE_RETRACTED;
            timer = 0.0f;
        }
    }

    y += vy * dt;
}

void CPiranhaPlant::Render() {
    if (state != PIRANHA_STATE_RETRACTED) {
        CResourceRegistry::GetInstance()->RenderAnimation("ANI_PIRANHA_PLANT", x, y, nx);
    }
}

void CPiranhaPlant::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    if (state == PIRANHA_STATE_RETRACTED) {
        left = bottom = right = top = 0.0f;
        return;
    }
    left = x;
    bottom = y;
    right = x + 16.0f;
    top = y + 24.0f;
}

void CPiranhaPlant::OnCollisionX(CGameObject *other, float nx) {
    if (state == PIRANHA_STATE_RETRACTED)
        return;
    if (auto mario = dynamic_cast<CMario *>(other)) {
        mario->Hurt();
    }
}

void CPiranhaPlant::OnCollisionY(CGameObject *other, float ny) {
    if (state == PIRANHA_STATE_RETRACTED)
        return;
    if (auto mario = dynamic_cast<CMario *>(other)) {
        mario->Hurt();
    }
}
