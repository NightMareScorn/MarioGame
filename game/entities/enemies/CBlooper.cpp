#include "CBlooper.h"

CBlooper::CBlooper(float x, float y) : CGameObject() {
    this->x = x;
    this->y = y;
    this->state = BLOOPER_STATE_SINK;
    this->timer = 0.0f;
    this->vx = 0.0f;
    this->vy = -0.015f;
    this->nx = 1;
}

void CBlooper::Update(float dt) {
    if (state == BLOOPER_STATE_SINK) {
        vy = -0.015f;
        vx = 0.0f;
        timer += dt;
        if (timer >= 1200.0f) {
            state = BLOOPER_STATE_SWIM;
            timer = 0.0f;
        }
    } else if (state == BLOOPER_STATE_SWIM) {
        vy = 0.045f;
        CPlayScene* scene = (CPlayScene*)CSceneManager::GetInstance()->GetCurrentScene();
        if (scene && scene->GetPlayer()) {
            nx = (scene->GetPlayer()->x < x) ? 1 : -1;
            vx = (nx == 1) ? -0.035f : 0.035f;
        }
        timer += dt;
        if (timer >= 700.0f) {
            state = BLOOPER_STATE_SINK;
            timer = 0.0f;
        }
    }

    // Apply movement
    x += vx * dt;
    y += vy * dt;

    // Boundaries
    if (y > 192.0f) {
        y = 192.0f;
        state = BLOOPER_STATE_SINK;
        timer = 0.0f;
    }
    if (y < 32.0f) {
        y = 32.0f;
        vy = 0.0f;
    }
}

void CBlooper::Render() {
    std::string aniName = (state == BLOOPER_STATE_SWIM) ? "ANI_BLOOPER_OPEN" : "ANI_BLOOPER_CLOSED";
    CResourceRegistry::GetInstance()->RenderAnimation(aniName, x, y, nx);
}

void CBlooper::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    left = x;
    bottom = y;
    right = x + 16.0f;
    top = y + 24.0f;
}
