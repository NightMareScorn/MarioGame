#pragma once
#include "../CGameObject.h"
#include "../../../engine/Graphics/Animations.h"

class CBrickDebris : public CGameObject
{
    float lifeTimer = 1500.0f;

public:
    CBrickDebris(float x, float y, float vx, float vy) : CGameObject(x, y)
    {
        this->vx = vx;
        this->vy = vy;
    }
    void Update(float dt) override
    {
        vy -= 0.002f * dt;
        x += vx * dt;
        y += vy * dt;
        lifeTimer -= dt;
        if (lifeTimer <= 0)
            SetIsDead(true);
    }
    void Render() override
    {
        CAnimations::GetInstance()->Render("ANI_BRICK_EXPLODE_SHARD", x, y);
    }
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override
    {
        left = bottom = right = top = 0.0f;
    }
};